FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(COMMON_SRC ${SRC_DIR}/deck.cpp ${SRC_DIR}/hand_strengths.cpp ${SRC_DIR}/player.cpp)

#tests
add_executable(poker_test_hands main_test.cpp hands_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_hands gtest_main)
target_include_directories(poker_test_hands PUBLIC ${INCLUDE_DIR})

add_executable(poker_test_deck main_test.cpp deck_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_deck gtest_main)
target_include_directories(poker_test_deck PUBLIC ${INCLUDE_DIR})

add_executable(poker_test_utils main_test.cpp utils_unittest.cpp ${SRC_DIR}/game.cpp ${COMMON_SRC} ${CHECK_PLAYER} ${RAND_PLAYER})
target_link_libraries(poker_test_utils gtest_main)
target_include_directories(poker_test_utils PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_test(HAND_TEST poker_test_hands)
add_test(DECK_TEST poker_test_deck)
add_test(UTILS_TEST poker_test_utils)