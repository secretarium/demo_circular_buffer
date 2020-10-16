cmake_minimum_required(VERSION 3.12)

message("sfx enclave")

if(${BUILD_ENCLAVE})
  add_enclave_library(switchless_enclave
    
    SRCS
    Enclave.h
    Enclave.cpp
    
    HEADER_ONLY_LIBS

    TRUSTED_LIBS
  )

  target_compile_definitions(switchless_enclave PRIVATE ${ENCLAVE_DEFINES})
  target_include_directories(switchless_enclave PUBLIC include PRIVATE ${ENCLAVE_LIBCXX_INCLUDES})
  target_compile_options(switchless_enclave PRIVATE ${ENCLAVE_COMPILE_OPTIONS})
  set_property(TARGET switchless_enclave PROPERTY COMPILE_PDB_NAME switchless_enclave)
    
  target_link_libraries(switchless_enclave
    PRIVATE
    
    switchless_enclave_edl_enclave
    shared_defs

    sgx_tswitchless.lib
  )

  set_property(TARGET switchless_enclave PROPERTY COMPILE_PDB_NAME switchless_enclave) 
  enclave_sign(switchless_enclave KEY Enclave_private.pem CONFIG Enclave.config.xml OUTPUT switchless_enclave.signed.dll)
  

endif()