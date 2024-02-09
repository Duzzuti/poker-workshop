FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# tests
add_executable(gtestc_test main_test.cpp test_gametest.cpp ${COMMON_SRC} ${TEST_PLAYER})
target_link_libraries(gtestc_test gtest_main)
target_include_directories(gtestc_test PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})

add_test(GTEST_TEST gtestc_test)
