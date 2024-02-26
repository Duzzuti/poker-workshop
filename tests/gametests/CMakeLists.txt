FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# tests
add_executable(gtestc_test main_test.cpp test_gametest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${RAND_PLAYER} ${CHECK_PLAYER})
target_link_libraries(gtestc_test gtest_main)
target_include_directories(gtestc_test PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(gtestc_illegal_moves main_test.cpp illegal_moves_gametest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${RAND_PLAYER} ${CHECK_PLAYER})
target_link_libraries(gtestc_illegal_moves gtest_main)
target_include_directories(gtestc_illegal_moves PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(gtestc_normal main_test.cpp normal_gametest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${RAND_PLAYER} ${CHECK_PLAYER})
target_link_libraries(gtestc_normal gtest_main)
target_include_directories(gtestc_normal PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(gtestc_showdowns main_test.cpp showdowns_gametest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${RAND_PLAYER} ${CHECK_PLAYER})
target_link_libraries(gtestc_showdowns gtest_main)
target_include_directories(gtestc_showdowns PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_test(GTEST_TEST gtestc_test)
add_test(GTEST_ILLEGAL_MOVES gtestc_illegal_moves)
add_test(GTEST_NORMAL gtestc_normal)
add_test(GTEST_SHOWDOWNS gtestc_showdowns)
