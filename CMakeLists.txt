#
# Copyright (c) 2012-2020 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 2.8)
project(Boost.DI CXX)
set(CXX_STANDARD 14 CACHE STRING "C++ Standard Version. [14|17|20]")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(
        ${PROJECT_NAME} INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/include
)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(COMPILER_SPECIFIC_CXX_STANDARD "-std=c++${CXX_STANDARD}")
    string(REPLACE "14" "1y" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})
    string(REPLACE "17" "1z" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})
    string(REPLACE "20" "2a" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})    

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_SPECIFIC_CXX_STANDARD}  -fno-omit-frame-pointer -fsanitize=address,undefined -fno-exceptions -pedantic -pedantic-errors -Wall -Wextra -Werror")

elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
    set(COMPILER_SPECIFIC_CXX_STANDARD "-std=c++${CXX_STANDARD}")
    string(REPLACE "14" "1y" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})
    string(REPLACE "17" "1z" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})
    string(REPLACE "20" "2a" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})    

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_SPECIFIC_CXX_STANDARD} -fno-omit-frame-pointer -fsanitize=address,undefined -fno-exceptions -pedantic -pedantic-errors -Wall -Wextra -Werror")
    
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(COMPILER_SPECIFIC_CXX_STANDARD "/std:c++${CXX_STANDARD}")
    string(REPLACE "20" "latest" COMPILER_SPECIFIC_CXX_STANDARD ${COMPILER_SPECIFIC_CXX_STANDARD})

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_SPECIFIC_CXX_STANDARD} /W3 /EHsc /Zc:__cplusplus")

else()
    message(WARNING "Unsupported compiler!")
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else ()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif ()

option(BOOST_DI_OPT_BUILD_TESTS "Build and perform Boost.DI tests" ${IS_TOPLEVEL_PROJECT})
if (BOOST_DI_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(extension/test)
    add_subdirectory(test)
endif ()

option(BOOST_DI_OPT_BUILD_EXAMPLES "Build perform Boost.DI examples" ${IS_TOPLEVEL_PROJECT})
if (BOOST_DI_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()
