cmake_minimum_required(VERSION 3.12)

message("App")

add_executable(
  App

  App.cpp
  App.h)

target_compile_definitions(App PRIVATE ${HOST_DEFINES})
target_include_directories(App PUBLIC include ${HOST_INCLUDES})
target_compile_options(App PUBLIC ${HOST_COMPILE_OPTIONS})
target_link_directories(App PUBLIC ${SGX_LIBRARY_PATH})
target_link_options(App PUBLIC 
  /WHOLEARCHIVE:sgx_uswitchless.lib 
  /NODEFAULTLIB:libcmtd.lib)

target_link_libraries(App

  switchless_enclave_edl_host
  
  shared_defs
  
  sgx_uswitchless.lib
  sgx_tcrypto.lib
  sgx_uae_service.lib
  sgx_capable.lib 
  sgx_urts.lib
  Ws2_32.Lib
  kernel32.lib
  user32.lib
  gdi32.lib
  winspool.lib
  shell32.lib
  ole32.lib
  oleaut32.lib
  uuid.lib
  comdlg32.lib
  advapi32.lib  
)

#target_link_options(App PRIVATE /LIBPATH:${SGX_LIBRARY_PATH})