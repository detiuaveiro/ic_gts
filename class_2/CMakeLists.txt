cmake_minimum_required(VERSION 3.16)
project(image_processing)

#SET (CMAKE_BUILD_TYPE "Release")
SET (CMAKE_BUILD_TYPE "Debug")

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
#SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

# Get Google test
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

# Use the usually pre-installed one
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Get Google test automatically
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz
)
FetchContent_MakeAvailable(googletest)


SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin)
# Source Code Folder
SET (SOURCE_FOLDER ${BASE_DIR}/src)

# Header folder
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)


add_executable(system_test ${SOURCE_FOLDER}/systemCheck.cpp)
target_link_libraries( system_test ${OpenCV_LIBS} )

add_executable(extract_image ${SOURCE_FOLDER}/extract_image.cpp)
target_link_libraries( extract_image ${OpenCV_LIBS} )

#    PRIVATE
#        GTest::GTest
#        GTest::Main
#        # add any necessary libraries here
#)
#add_test(<library> <file_name>)
#target_link_libraries(<executable> gtest)
#target_link_libraries(<executable> gtest_main)
