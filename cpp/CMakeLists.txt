# Copyright 2018 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# The majority of this is taken from the C++ grpc route_guide example at:
# https://github.com/grpc/grpc/blob/ff9cc060907e88640a6fcfe78d0dfeb2f3d89ca1/examples/cpp/route_guide/CMakeLists.txt

cmake_minimum_required(VERSION 3.24)

project(PdfToText C CXX)

include(common.cmake)

# Find absl package
find_package(absl CONFIG REQUIRED)

# Proto file
get_filename_component(service_proto "../proto/xpdf_service.proto" ABSOLUTE)
get_filename_component(service_proto_path "${service_proto}" PATH)

# Generated sources
set(service_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/xpdf_service.pb.cc")
set(service_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/xpdf_service.pb.h")
set(service_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/xpdf_service.grpc.pb.cc")
set(service_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/xpdf_service.grpc.pb.h")
add_custom_command(
      OUTPUT "${service_proto_srcs}" "${service_proto_hdrs}" "${service_grpc_srcs}" "${service_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${service_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${service_proto}"
      DEPENDS "${service_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include(FetchContent)
FetchContent_Declare(
  xpdf
  GIT_REPOSITORY https://github.com/adamvduke/xpdf.git
  GIT_TAG        c8000d833223060d4c58c22b827c1d5ad3230996 # 4.0.5
)

FetchContent_MakeAvailable(xpdf)

# Include xpdf directories
include_directories("${xpdf_SOURCE_DIR}")
include_directories("${xpdf_BINARY_DIR}")
include_directories("${xpdf_SOURCE_DIR}/fofi")
include_directories("${xpdf_SOURCE_DIR}/goo")
include_directories("${xpdf_SOURCE_DIR}/splash")
include_directories("${xpdf_SOURCE_DIR}/xpdf")

# service_grpc_proto
add_library(service_grpc_proto
  ${service_grpc_srcs}
  ${service_grpc_hdrs}
  ${service_proto_srcs}
  ${service_proto_hdrs})

target_link_libraries(service_grpc_proto
  absl::absl_log
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# extract lib for use in cgo
add_library(extract
    "extract.cc"
    $<TARGET_OBJECTS:xpdf_objs>
    $<TARGET_OBJECTS:goo_objs>
    $<TARGET_OBJECTS:fofi_objs>
    "${xpdf_SOURCE_DIR}/xpdf/TextOutputDev.cc"
)

add_custom_command(TARGET extract POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/build/libextract.a"
        "${CMAKE_CURRENT_SOURCE_DIR}/../go/cgoclient/extract/")

add_custom_command(TARGET extract POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/extract.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../go/cgoclient/extract/")

target_link_libraries(extract
  absl::absl_log
  absl::log_initialize
  absl::log_globals
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets 
foreach(_target
  xpdf_grpc_server)

  add_executable(${_target}
    "${_target}.cc"
    "extract.cc"
    $<TARGET_OBJECTS:xpdf_objs>
    $<TARGET_OBJECTS:goo_objs>
    $<TARGET_OBJECTS:fofi_objs>
    "${xpdf_SOURCE_DIR}/xpdf/TextOutputDev.cc"
  )

  target_link_libraries(${_target}
    service_grpc_proto
    absl::flags_parse
    absl::absl_log
    absl::log_initialize
    absl::log_globals
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()