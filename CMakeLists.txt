cmake_minimum_required(VERSION 3.12)

option(HUNTER_STATUS_PRINT "Print working status" OFF)
option(HUNTER_KEEP_PACKAGE_SOURCES "Keep hunter source in folders" ON)
option(BUILD_ENCLAVE "build enclave code" ON)
option(BUILD_HOST "build host code" ON)
option(BUILD_EXE "build exe code" ON)
option(BUILD_DC_APPS "build dc apps" ON)
option(BUILD_TEST "build test code" ON)

option(CMAKE_VERBOSE_MAKEFILE "verbose build step" OFF)
option(CMAKE_RULE_MESSAGES "verbose cmake" OFF)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(BUILD_TEST)
  include(GoogleTest)
endif()

if("${BUILD_TYPE}" STREQUAL "")
	set(BUILD_TYPE "release")
endif()

if(WIN32)
    #we dont want os specific libraries for our enclaves
    set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "override default windows libraries" FORCE)
    set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "override default windows libraries" FORCE)
    set(CMAKE_CXX_FLAGS "" CACHE STRING "override default windows flags" FORCE)
    set(CMAKE_C_FLAGS "" CACHE STRING "override default windows flags" FORCE)
    set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "override default windows flags" FORCE)
    set(CMAKE_C_FLAGS_DEBUG "" CACHE STRING "override default windows flags" FORCE)

else()
  #this is generally frowned apon as it forces all libraries to include this 
  #it is needed to force feed gtest with the right c++ runtime
  #perhaps moving this command to a sub project would be sensible
  include_directories(BEFORE SYSTEM "/usr/lib/llvm-10/include/c++/v1")
endif()

#[[include(HunterGate)

huntergate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.245.tar.gz" SHA1
  "36e7f44ee4b3f8e25683e9d700f05a27ecb0a848" FILEPATH
  "${CMAKE_CURRENT_LIST_DIR}/cmake/Hunter/config.cmake")]]

# ##############################################################################
# settings

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENCLAVE_TARGET "SGX")
set(SGX_HW ON)

project(test_switchless VERSION 0.1.0)


  if(WIN32)
    set(ENCLAVE_SSL_INCLUDES "C:/Dev/Libs/VS2015/x64/openssl-1.1.1/include")
    set(HOST_LIBRARIES 
        advapi32.lib
        comdlg32.lib
        gdi32.lib
        kernel32.lib
        ole32.lib
        oleaut32.lib
        shell32.lib
        user32.lib
        uuid.lib
        winspool.lib
      )
    if(${BUILD_TYPE} STREQUAL  "release" OR ${BUILD_TYPE} STREQUAL  "prerelease")
      set(HOST_DEFINES 
        ENCLAVE_STATUS=sgx_status_t
        ENCLAVE_OK=SGX_SUCCESS
        PREVENT_DEBUG_ENCLAVE
        WIN32
        NDEBUG
        _CONSOLE
        _LIB
        NOMINMAX
        )

      set(HOST_COMPILE_OPTIONS 
        /diagnostics:classic 
        /EHsc
        /errorReport:prompt
        /FC
        /fp:precise
        /Gd
        /GL
        /Gm-
        /GS
        /guard:cf
        /Gy
        /MD
        /MP
        /nologo
        /O2
        /Oi
        /Qspectre
        /sdl
        /std:c++17
        /W3
        /WX-
        /Zc:forScope
        /Zc:inline
        /Zc:rvalueCast
        /Zc:wchar_t
        /Zi
      )        
      set(ENCLAVE_DEFINES _IN_ENCLAVE ENCLAVE_STATUS=sgx_status_t
                          ENCLAVE_OK=SGX_SUCCESS DISALLOW_BAD_JUMPS
                          _LIB NOMINMAX USING_CMAKE)
      set(ENCLAVE_COMPILE_OPTIONS
        /diagnostics:classic 
        /EHsc
        /errorReport:prompt
        /Gd
        /GL 
        /Gy 
        /MTd
        /MP 
        /nologo
        /Od
        /Qspectre 
        /W3 
        /WX-
        /Zc:forScope
        /Zc:inline
        /Zc:rvalueCast 
        /Zc:wchar_t 
        /Zi
      )
    else()
      set(HOST_DEFINES 
        _DEBUG
        _LIB
        ENCLAVE_OK=SGX_SUCCESS
        ENCLAVE_STATUS=sgx_status_t
        NOMINMAX
        WIN32
      )

      set(HOST_COMPILE_OPTIONS 
        /bigobj
        /diagnostics:classic 
        /EHsc
        /errorReport:prompt
        /FC
        /FS
        /fp:precise
        /Gd
        /Gm-
        /GS
        /guard:cf
        /JMC 
        /MDd
        /MP
        /nologo
        /Od
        /Qspectre
        /RTC1
        /sdl
        /std:c++17
        /W3
        /WX-
        /Zc:forScope
        /Zc:inline
        /Zc:rvalueCast
        /Zc:wchar_t
        /Zi
      )
      
      set(ENCLAVE_DEFINES 
        ENCLAVE_STATUS=sgx_status_t
        ENCLAVE_OK=SGX_SUCCESS
        VERBOSE=2 #needed in some projects
        _DEBUG  #missing VS
        _IN_ENCLAVE
        _LIB
        NOMINMAX
        USING_CMAKE #needed in some projects as part of the migration to cmake to be removed
        )
      set(ENCLAVE_COMPILE_OPTIONS
        /diagnostics:classic 
        /EHsc
        /errorReport:prompt
        /FC
        /fp:precise
        /Gd
        /Gm-
        /GS
        /JMC
        /MDd
        /MP
        /nologo
        /Od
        /Qspectre
        /sdl
        /W4
        /WX-
        /Zc:forScope
        /Zc:inline
        /Zc:rvalueCast
        /Zc:wchar_t
        /Zi
        )
    endif()
  else()
    set(HOST_DEFINES ENCLAVE_STATUS=sgx_status_t ENCLAVE_OK=SGX_SUCCESS DISALLOW_BAD_JUMPS)
    set(ENCLAVE_DEFINES _IN_ENCLAVE CLEAN_LIBC ENCLAVE_STATUS=sgx_status_t
                        ENCLAVE_OK=SGX_SUCCESS DISALLOW_BAD_JUMPS)
    set(ENCLAVE_COMPILE_OPTIONS
      -Wno-c++17-extensions
      -Wno-nonportable-include-path
      -Wno-conversion
      -Wno-unused-parameter
      -Wno-tautological-undefined-compare
      -Wno-dynamic-class-memaccess
      -Wno-ignored-qualifiers
      -Wno-exceptions
      -Wno-null-dereference
      -Wno-ignored-attributes
      -Wno-implicit-exception-spec-mismatch
      -I/usr/lib/llvm-10/include/c++/v1)
    set(HOST_COMPILE_OPTIONS
      -I/usr/lib/llvm-10/include/c++/v1
      -Wno-nonportable-include-path
      -Wno-conversion
      -Wno-unused-parameter
      -Wno-tautological-undefined-compare
      -Wno-dynamic-class-memaccess
      -Wno-ignored-qualifiers
      -Wno-exceptions
      -Wno-null-dereference
      -Wno-ignored-attributes
      -Wno-implicit-exception-spec-mismatch
      -Wno-trigraphs)
    set(HOST_LINK_OPTIONS -L/usr/lib/llvm-10/lib -Wl,-rpath,/usr/lib/llvm-10/lib -lc++ -L/opt/intel/sgxsdk/lib64 -lsgx_tcrypto
      linux_dependancies_host)
    set(ENCLAVE_SSL_INCLUDES "/opt/intel/sgxssl/include")
    set(OS_DEPENDANCIES_ENCLAVE linux_dependancies_enclave)
  endif()

  find_package(SGX REQUIRED)

  set(HOST_INCLUDES ${SGX_INCLUDE_DIR})
  set(ENCLAVE_LIBC_INCLUDES ${SGX_INCLUDE_DIR} ${SGX_TLIBC_INCLUDE_DIR})
  set(ENCLAVE_LIBCXX_INCLUDES ${ENCLAVE_LIBC_INCLUDES} ${SGX_LIBCXX_INCLUDE_DIR} ${SGX_LIBSTDCXX_INCLUDE_DIR})


# ##############################################################################

include(CTest)
enable_testing()

message("submodules")

add_subdirectory(Interface)
add_subdirectory(Include)
add_subdirectory(Enclave)
add_subdirectory(App)
