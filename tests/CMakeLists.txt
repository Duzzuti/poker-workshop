set(COMMON_SRC ${SRC_DIR}/deck.cpp ${SRC_DIR}/hand_strengths.cpp ${SRC_DIR}/player.cpp ${SRC_DIR}/pot.cpp)

# Add the executable target
add_executable(testc testc.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${CHECK_PLAYER} ${RAND_PLAYER})
# Include headers
target_include_directories(testc PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR})

# Link with plog library
target_link_libraries(testc plog)

add_subdirectory(unittests)
# Check if the directory exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gametests/CMakeLists.txt")
    add_subdirectory(gametests)
else()
    message(STATUS "CMakeLists.txt not found in gametests, skipping...")
endif()