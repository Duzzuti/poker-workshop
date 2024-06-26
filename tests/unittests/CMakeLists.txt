FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

#tests
add_executable(poker_test_hands main_test.cpp hands_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_hands gtest_main)
target_include_directories(poker_test_hands PUBLIC ${INCLUDE_DIR})

add_executable(poker_test_deck main_test.cpp deck_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_deck gtest_main)
target_include_directories(poker_test_deck PUBLIC ${INCLUDE_DIR})

add_executable(poker_test_pot main_test.cpp pot_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_pot gtest_main)
target_include_directories(poker_test_pot PUBLIC ${INCLUDE_DIR})

add_executable(poker_test_utils main_test.cpp utils_unittest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${CHECK_PLAYER} ${RAND_PLAYER} ${HUMAN_PLAYER})
target_link_libraries(poker_test_utils gtest_main)
target_include_directories(poker_test_utils PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(poker_test_const main_test.cpp const_unittest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${CHECK_PLAYER} ${RAND_PLAYER} ${HUMAN_PLAYER})
target_link_libraries(poker_test_const gtest_main)
target_include_directories(poker_test_const PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(poker_test_gametest main_test.cpp gametest_unittest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${CHECK_PLAYER} ${RAND_PLAYER})
target_link_libraries(poker_test_gametest gtest_main)
target_include_directories(poker_test_gametest PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_executable(poker_test_thandstrengths main_test.cpp thandutils_unittest.cpp ${THAND_STRENGTHS_DIR}/hand_utils.cpp)
target_link_libraries(poker_test_thandstrengths gtest_main)
target_include_directories(poker_test_thandstrengths PUBLIC ${INCLUDE_DIR} ${THAND_STRENGTHS_DIR})

add_executable(test main_test.cpp test_test.cpp)
target_link_libraries(test gtest_main)
target_include_directories(test PUBLIC ${INCLUDE_DIR})

add_test(HAND_TEST poker_test_hands)
add_test(DECK_TEST poker_test_deck)
add_test(POT_TEST poker_test_pot)
add_test(UTILS_TEST poker_test_utils)
add_test(CONST_TEST poker_test_const)
add_test(GAME_TEST poker_test_gametest)
add_test(THANDSTRENGTHS_TEST poker_test_thandstrengths)
add_test(TEST_TEST test)