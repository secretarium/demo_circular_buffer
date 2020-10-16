cmake_minimum_required(VERSION 3.12)

message("sfx enclave")

if(${BUILD_ENCLAVE})
  add_enclave_library(Enclave
    
    SRCS
    Enclave.h
    Enclave.cpp
    
    HEADER_ONLY_LIBS

    TRUSTED_LIBS
  )

  target_compile_definitions(Enclave PRIVATE ${ENCLAVE_DEFINES})
  target_include_directories(Enclave PUBLIC include PRIVATE ${ENCLAVE_LIBCXX_INCLUDES})
  target_compile_options(Enclave PRIVATE ${ENCLAVE_COMPILE_OPTIONS})
  set_property(TARGET Enclave PROPERTY COMPILE_PDB_NAME Enclave)
    
  target_link_libraries(Enclave
    PRIVATE
    
    switchless_enclave_edl_enclave
    shared_defs

    sgx_tswitchless.lib
  )

  set_property(TARGET Enclave PROPERTY COMPILE_PDB_NAME Enclave) 
  enclave_sign(Enclave KEY Enclave_private.pem CONFIG Enclave.config.xml OUTPUT Enclave.signed.dll)
  

endif()