# This file is part of the "slavicTales" project.
# You may use, distribute or modify this code under the terms
# of the GNU General Public License version 2.
# See LICENCE.txt in the root directory of the project.
#
# Author: Maxim Atanasov
# E-mail: atanasovmaksim1@gmail.com

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(ST_task_manager)

set(CMAKE_CXX_STANDARD 14)

include(GoogleTest)
enable_testing()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../cmake)


include_directories(${PROJECT_SOURCE_DIR}/src/main)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../../ST_util/include)
include_directories(${PROJECT_SOURCE_DIR}/../ST_message_bus/include)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIR})
include_directories(${SDL2_TTF_INCLUDE_DIR})

add_library(ST_task_manager STATIC
        ${PROJECT_SOURCE_DIR}/src/main/task.hpp
        ${PROJECT_SOURCE_DIR}/src/main/task_manager.cpp
        ${PROJECT_SOURCE_DIR}/include/task_manager.hpp
        ${PROJECT_SOURCE_DIR}/src/main/task_allocator.cpp
        ${PROJECT_SOURCE_DIR}/src/main/task_allocator.hpp)

add_executable(task_manager_test
        ${PROJECT_SOURCE_DIR}/src/test/task_manager_tests.cpp)

target_link_libraries(task_manager_test
        gtest
        ST_task_manager
        ST_message_bus
        ST_util
        ${SDL2_LIBRARY}
        ${SDL2_LIBRARY}
        ${SDL2_IMAGE_LIBRARY}
        ${SDL2_TTF_LIBRARY}
        ${SDL2_MIXER_LIBRARY})

#Run the tests on each build
add_test(NAME task_manager_test COMMAND task_manager_test WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/src/test/output)
add_custom_command(
        TARGET ST_task_manager
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target task_manager_test
)
add_custom_command(
        TARGET ST_task_manager
        POST_BUILD
        COMMAND ctest -VV
)