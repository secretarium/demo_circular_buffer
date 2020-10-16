cmake_minimum_required(VERSION 3.12)

if(${ENCLAVE_TARGET} STREQUAL "SGX")
  message(STATUS "doing SGX edgerator")
  
  if(${BUILD_ENCLAVE})

    enclave_edl_library(
      switchless_enclave_edl_enclave 
      ${CMAKE_SOURCE_DIR}/Interface/switchless_enclave.edl
      ""
      OFF
      ""
    )
  
  endif()

  if(${BUILD_HOST})
 
    host_edl_library(
      switchless_enclave_edl_host
      ${CMAKE_SOURCE_DIR}/Interface/switchless_enclave.edl
      ""
      OFF
      ""
    )

  endif()
endif()
