# FILEPATH: /home/dev/poker-workshop/CMakeLists.txt
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required(VERSION 3.0)
project(PokerWorkshop)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

include(FetchContent)

FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog
    GIT_TAG 1.1.10
)
FetchContent_MakeAvailable(plog)
# add to includes
include_directories(${plog_SOURCE_DIR}/include)

# Print all the variables
get_cmake_property(_variableNames VARIABLES)

add_subdirectory(src)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
endif()

