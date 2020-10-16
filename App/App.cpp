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


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <array>
#include <thread>


#include <sgx_urts.h>
#include <sgx_uswitchless.h>
#include "App.h"
#include "switchless_enclave_u.h"
#include "test_queue_type.h"

enum test_type
{
    ordinary,
    switchless,
    circular_buffer
};

std::array<const char*, 3> names =
{
    "ordinary",
    "switchless",
    "circular_buffer"
};

queue ecall_queue;
queue ocall_queue;

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

#define REPEATS 500000

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }

    if (idx == ttl)
        printf("Error: Unexpected error occurred.\n");
}

/* Initialize the enclave:
 *   Call sgx_create_enclave to initialize an enclave instance
 */
int initialize_enclave(const char* executable_name, const sgx_uswitchless_config_t* us_config)
{
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;

    std::filesystem::path enclave_file = std::filesystem::path(executable_name).parent_path() / ENCLAVE_FILENAME;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */

    if (us_config)
    {
        const void* enclave_ex_p[32] = { 0 };

        enclave_ex_p[SGX_CREATE_ENCLAVE_EX_SWITCHLESS_BIT_IDX] = (const void*)us_config;

        ret = sgx_create_enclave_ex(enclave_file.generic_u8string().c_str(), SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL, SGX_CREATE_ENCLAVE_EX_SWITCHLESS, enclave_ex_p);
        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
            return -1;
        }
    }
    else
    {
        ret = sgx_create_enclave(enclave_file.generic_u8string().c_str(), SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
        if (ret != SGX_SUCCESS) {
            print_error_message(ret);
            return -1;
        }
    }
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate
     * the input string to prevent buffer overflow.
     */
    printf("%s", str);
}

void ocall_empty(void) {}
void ocall_empty_switchless(void) {}

void benchmark_empty_ocall(test_type type)
{
    int pop_completed = 0;
	unsigned int count = 0;
	std::thread message_pump;
    if (type == test_type::circular_buffer)
    {
        message_pump = std::thread([&]() {
            block data;
            while (!pop_completed)
            {
				if(ocall_queue.pop(data))
					count++;
			}
			while (ocall_queue.pop(data))
			{
				count++;
			}
			count;
        });
    }
    unsigned long nrepeats = REPEATS;
    DWORD ticks_before, ticks_after;
    printf("Repeating an **%s** OCall that does nothing for %lu times...\n",
        names[type], nrepeats);

    ticks_before = GetTickCount();

    if (type != test_type::circular_buffer)
    {
        sgx_status_t status = ecall_repeat_ocalls(global_eid, nrepeats, type);
        if (status != SGX_SUCCESS) {
            printf("ERROR: ECall failed\n");
            print_error_message(status);
            exit(-1);
        }
    }
    else
    {
        unsigned long ecall_count = 0;
        unsigned long ocall_count = nrepeats;
        ecall_run_queue_tests(global_eid,  &ecall_queue, &ocall_queue, &ecall_count, &ocall_count);
		pop_completed = true;
	}
    
    ticks_after = GetTickCount();
    
    printf("Ticks elapsed: %lu\n", ticks_after - ticks_before);

    if (type == test_type::circular_buffer)
    {
        message_pump.join();
		assert(count == nrepeats);
    }
}

void benchmark_empty_ecall(test_type type)
{
    int push_completed = 0;
    std::thread message_pump;
    if (type == test_type::circular_buffer)
    {
        message_pump = std::thread([&]() {
            block data;
            int count = REPEATS;
            while (count--)
            {
                while (!ecall_queue.push(data))
                {
                }
            }
            push_completed = true;
        });
    }

    unsigned long nrepeats = REPEATS;
    printf("Repeating an **%s** ECall that does nothing for %lu times...\n",
        names[type], nrepeats);

    DWORD ticks_before, ticks_after;
    ticks_before = GetTickCount();

    if (type != test_type::circular_buffer)
    {
        sgx_status_t(*ecall_fn)(sgx_enclave_id_t) = type ? ecall_empty_switchless : ecall_empty;
        while (nrepeats--) {
            ecall_fn(global_eid);
        }
    }
    else
    {
        unsigned long ecall_count = nrepeats;
        unsigned long ocall_count = 0;
        ecall_run_queue_tests(global_eid, &ecall_queue, &ocall_queue, &ecall_count, &ocall_count);
    }

    ticks_after = GetTickCount();
    printf("Ticks elapsed: %lu\n", ticks_after - ticks_before);

    if (type == test_type::circular_buffer)
    {
        message_pump.join();
    }
}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    /* Configuration for Switchless SGX */
    sgx_uswitchless_config_t us_config = SGX_USWITCHLESS_CONFIG_INITIALIZER;
    us_config.num_uworkers = 2;
    us_config.num_tworkers = 2;

    /* Initialize the enclave */
    if(initialize_enclave(argv[0], &us_config) < 0)
    {
        printf("Error: enclave initialization failed\n");
        return -1;
    }

    /* Initialize the enclave */
    /*if (initialize_enclave(nullptr) < 0)
    {
        printf("Error: enclave initialization failed\n");
        return -1;
    }*/

    
    printf("Running a benchmark that compares **ordinary** and **switchless** OCalls...\n");
    benchmark_empty_ocall(test_type::switchless);
    benchmark_empty_ocall(test_type::ordinary);
    benchmark_empty_ocall(test_type::circular_buffer);
    printf("Done.\n");
    

    printf("Running a benchmark that compares **ordinary** and **switchless** ECalls...\n");
    benchmark_empty_ecall(test_type::switchless);
    benchmark_empty_ecall(test_type::ordinary);
    benchmark_empty_ecall(test_type::circular_buffer);
    printf("Done.\n");

    sgx_destroy_enclave(global_eid);
    return 0;
}
