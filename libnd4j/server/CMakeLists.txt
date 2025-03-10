cmake_minimum_required(VERSION 3.9)
project(dev_tests)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake" ${CMAKE_MODULE_PATH})

#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")
#SET(BUILD_SHARED_LIBS OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(../include ../include/helpers ../include/array ../include/memory ../include/loops ../include/graph ../include/ops ../include/types ../include/cnpy ../blas)
if(LINUX)
    link_directories(/usr/local/lib)
    link_directories(/usr/lib)
    link_directories(/lib)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)

if(APPLE)
    message("Using apple")
    link_directories(/usr/local/lib)
    link_directories(/usr/lib)
    link_directories(/lib)
endif()
if(WIN32)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(dir ${dirs})
        message(STATUS "dir='${dir}'")
    endforeach()
endif()


if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS  "-O3  -fPIC -std=c++11 -fmax-errors=2")
    if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "ppc64*")
        set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -mcpu=native")
    else()
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mmmx  -msse  -msse2  -msse3  -mssse3  -msse4.1  -msse4.2  -msse4 -mavx -mavx2")
    endif()
else()
    if (APPLE)
        set(CMAKE_CXX_FLAGS  " -O0 -g -fPIC -std=c++11 -fmax-errors=2 -D__APPLE_OS__=true")
    elseif(WIN32)
		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
			set(CMAKE_CXX_FLAGS  " -O0 -g --fPIC -std=c++11 -fmax-errors=2")
		endif()
    else()
        set(CMAKE_CXX_FLAGS  " -g -O0 -fPIC -std=c++11 -fmax-errors=2")
        if (SD_CPU)
            SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fsanitize=address")
        endif()
    endif()
endif()


# tests are always compiled with all ops included
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DSD_ALL_OPS=1")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using Clang
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_TUNE}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARCH_TUNE} -fp-model fast")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
endif()


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include_directories("/usr/include")
    include_directories("/usr/local/include")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS 4.9)
    message(FATAL_ERROR "You need at least GCC 4.9")
endif()

if (NOT MSVC_DEV)
	message("Looking for OpenMP")
	find_package(OpenMP)
endif()
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message("OPENMP NOT FOUND")
endif()


if(APPLE)
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    #SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -static")
elseif(MSYS)
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -static")
endif()


file(GLOB_RECURSE TYPES_SOURCES false ../include/types/*.cpp ../include/types/*.h)
file(GLOB_RECURSE ARRAY_SOURCES false ../include/array/*.cpp ../include/array/*.h)
file(GLOB_RECURSE MEMORY_SOURCES false ../include/memory/*.cpp ../include/memory/*.h)
file(GLOB_RECURSE GRAPH_SOURCES false ../include/graph/*.cpp ../include/graph/*.h)
file(GLOB_RECURSE CUSTOMOPS_SOURCES false ../include/ops/declarable/generic/*.cpp)
file(GLOB_RECURSE CUSTOMOPS_HELPERS_SOURCES false ../include/ops/declarable/helpers/cpu/*.cpp)
file(GLOB_RECURSE OPS_SOURCES false ../include/ops/impl/*.cpp ../include/ops/declarable/impl/*.cpp  ../include/ops/*.h)
file(GLOB_RECURSE INDEXING_SOURCES false ../include/indexing/*.cpp ../include/indexing/*.h)
file(GLOB_RECURSE HELPERS_SOURCES false ../include/helpers/*.cpp ../include/helpers/*.h)
file(GLOB_RECURSE LOOPS_SOURCES false ../include/loops/*.cpp ../include/loops/*.h)

message("CPU BLAS")
add_definitions(-D__CPUBLAS__=true)

find_package(GRPC REQUIRED)
message("gRPC found, building GraphServer")
add_executable(GraphServer ./GraphServer.cpp ../include/graph/generated/graph.grpc.fb.cc ../blas/cpu/NativeOps.cpp ../blas/cpu/GraphExecutioner.cpp
        ../blas/cpu/NativeOpExecutioner.cpp ../blas/cpu/NDArray.cpp
        ../include/cnpy/cnpy.cpp ../include/system/nd4jmemset.h ../include/system/nd4jmalloc.h
        ../blas/Environment.cpp ../blas/Environment.h ${LOOPS_SOURCES}  ${ARRAY_SOURCES} ${TYPES_SOURCES}
        ${MEMORY_SOURCES} ${GRAPH_SOURCES} ${CUSTOMOPS_SOURCES} ${INDEXING_SOURCES} ${HELPERS_SOURCES}  ${CUSTOMOPS_HELPERS_SOURCES} ${OPS_SOURCES})

target_link_libraries(GraphServer ${GRPC_LIBRARIES})

