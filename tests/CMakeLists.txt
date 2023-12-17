FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(COMMON_SRC ${SRC_DIR}/deck.cpp ${SRC_DIR}/hand_strengths.cpp ${SRC_DIR}/player.cpp)

#tests
add_executable(poker_test_hands main_test.cpp hands_unittest.cpp ${COMMON_SRC})
target_link_libraries(poker_test_hands gtest_main)
target_include_directories(poker_test_hands PUBLIC ${INCLUDE_DIR})

add_test(HAND_TEST poker_test_hands)