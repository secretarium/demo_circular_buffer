#pragma once
#include <array>
#include "queue.h"

#define LOG_BLOCK_SIZE 1024
#define LOG_BUFFER_SIZE 1024

using block = std::array<uint8_t, LOG_BLOCK_SIZE>;
using queue = spsc_queue<block, LOG_BUFFER_SIZE>;
