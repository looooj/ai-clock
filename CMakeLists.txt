cmake_minimum_required(VERSION 3.12)

set(_BASE_DIR_ "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(_BASE_DIR_ ${_BASE_DIR_} ABSOLUTE)
message("_BASE_DIR_ ${_BASE_DIR_}")

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_BASE_DIR_}/bin)

include(${_BASE_DIR_}/cmake/custom-var.cmake)

message("====VCPKG_ROOT ${VCPKG_ROOT}")
if(WIN32)
  set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH};${QJS_LIB_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CPP_UTILS_INC})
project(sclock)




find_package(wxWidgets CONFIG REQUIRED)
add_executable(ai_wx WIN32
src/ai_wx_5.cpp
)
target_link_libraries(ai_wx PRIVATE wx::core wx::base)
target_compile_definitions(ai_wx PRIVATE UNICODE _UNICODE WXSUPPRESS_SIZER_FLAGS_CHECK=1)

