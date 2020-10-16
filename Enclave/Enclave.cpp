/*
 * Copyright (C) 2011-2016 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "switchless_enclave_t.h"
#include "test_queue_type.h"

void ecall_repeat_ocalls(unsigned long nrepeats, int use_switchless) {
    sgx_status_t(*ocall_fn)(void) = use_switchless ? ocall_empty_switchless : ocall_empty;
    while (nrepeats--) {
        ocall_fn();
    }
}

void ecall_empty(void) {}
void ecall_empty_switchless(void) {}

void ecall_run_queue_tests(void* ecall_queue, void* ocall_queue, unsigned long* ecall_tests, unsigned long* ocall_tests)
{
    queue* in_queue = (queue*)ecall_queue;
    queue* out_queue = (queue*)ocall_queue;

    {
        block data;
        while (*ecall_tests)
        {
            while (in_queue->pop(data) == false)
            {
                //retrying 
            }
            (*ecall_tests)--;
        }
    }
    {
        while (*ocall_tests)
        {
            block data;
            while (out_queue->push(data) == false)
            {
                //retrying 
            }
			(*ocall_tests)--;
        }
    }
}