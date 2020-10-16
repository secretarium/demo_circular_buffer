#pragma once
#include <atomic>

//single consumer single producer queue, i.e. only one thread for sending and one thread for receiving
template<typename T, size_t Size>
class spsc_queue {
public:
	spsc_queue() : head_(0), tail_(0) {}

	bool push(const T & value)
	{
		size_t head = head_.load(std::memory_order_relaxed);
		size_t next_head = next(head);
		if (next_head == tail_.load(std::memory_order_acquire))
			return false;
		ring_[head] = value;
		head_.store(next_head, std::memory_order_release);
		return true;
	}
	bool pop(T & value)
	{
		size_t tail = tail_.load(std::memory_order_relaxed);
		if (tail == head_.load(std::memory_order_acquire))
			return false;
		value = ring_[tail];
		tail_.store(next(tail), std::memory_order_release);
		return true;
	}
private:
	size_t next(size_t current)
	{
		return (current + 1) % Size;
	}
	T ring_[Size];
	std::atomic<size_t> head_, tail_;
};

//multi consumer single producer queue, i.e. many threads for sending and one thread for receiving
//slightly slower than spsc queues
//cirular buffers cannot be used so buffer needs to be created by the sender.
//if communicating over across enclave boundries garbage collection of that allocated buffer needs 
//to be destroyed at source, as the malloc inside an enclave may be different to outside.
//If an enclave allocates shared data using its own memory manager's arena, then so long as the 
//queues are purged all memory allocated by that arena may be winked out when it is destroyed
template<typename T>
class mpsc_queue {
public:
	struct node {
		T data;
		node * next;
	};
	void push(node* n)
	{
		node * stale_head = head_.load(std::memory_order_relaxed);
		do {
			n->next = stale_head;
		} while (!head_.compare_exchange_weak(stale_head, n, std::memory_order_release));
	}

	node * pop_all(void)
	{
		node* last = pop_all_reverse();
		node* first = 0;
		while (last) {
			node* tmp = last;
			last = last->next;
			tmp->next = first;
			first = tmp;
		}
		return first;
	}

	mpsc_queue() : head_(0) {}

	// alternative interface if ordering is of no importance
	node * pop_all_reverse(void)
	{
		return head_.exchange(0, std::memory_order_consume);
	}
private:
	std::atomic<node *> head_;
};
