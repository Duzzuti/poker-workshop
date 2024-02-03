set(COMMON_SRC ${SRC_DIR}/deck.cpp ${SRC_DIR}/hand_strengths.cpp ${SRC_DIR}/player.cpp ${SRC_DIR}/pot.cpp)

# Add the executable target
add_executable(test_gen test_gen_main.cpp ${COMMON_SRC} ${TEST_PLAYER})
# Include headers
target_include_directories(test_gen PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR})

# Link with plog library
target_link_libraries(test_gen plog)

add_subdirectory(unittests)