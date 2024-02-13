#pragma once
/// @brief Compiles the CMakeLists.txt file for the tests
/// @param cmakeLists The file to write to (should be gametests/CMakeLists.txt)
/// @param fileConfigs The file configurations to correctly set up the cmake file
/// @exception Guarantee No-throw
void compileCmake(std::ofstream& cmakeLists, const std::vector<FileConfig>& fileConfigs) noexcept {
    cmakeLists << "FetchContent_Declare(" << std::endl;
    cmakeLists << "    googletest" << std::endl;
    cmakeLists << "    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip" << std::endl;
    cmakeLists << ")" << std::endl;
    cmakeLists << "# For Windows: Prevent overriding the parent project's compiler/linker settings" << std::endl;
    cmakeLists << "set(gtest_force_shared_crt ON CACHE BOOL \"\" FORCE)" << std::endl;
    cmakeLists << "FetchContent_MakeAvailable(googletest)" << std::endl;
    cmakeLists << "" << std::endl;
    cmakeLists << "enable_testing()" << std::endl;
    cmakeLists << "" << std::endl;
    cmakeLists << "# tests" << std::endl;
    for (const FileConfig& fileConfig : fileConfigs) {
        cmakeLists << "add_executable(gtestc_" << fileConfig.cmakeTestName << " main_test.cpp " << fileConfig.fileName << " ${COMMON_SRC} ${TEST_PLAYER})" << std::endl;
        cmakeLists << "target_link_libraries(gtestc_" << fileConfig.cmakeTestName << " gtest_main)" << std::endl;
        cmakeLists << "target_include_directories(gtestc_" << fileConfig.cmakeTestName << " PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})" << std::endl;
        cmakeLists << std::endl;
    }
    for (const FileConfig& fileConfig : fileConfigs) {
        cmakeLists << "add_test(GTEST_" << stringToUpper(fileConfig.cmakeTestName) << " gtestc_" << fileConfig.cmakeTestName << ")" << std::endl;
    }
}

/// @brief Compiles the main_test.cpp file for the tests
/// @param mainTest The file to write to (should be gametests/main_test.cpp)
/// @exception Guarantee No-throw
void compileMainTest(std::ofstream& mainTest) noexcept {
    mainTest << "#include <gtest/gtest.h>" << std::endl;
    mainTest << "" << std::endl;
    mainTest << "#include \"logger.h\"" << std::endl;
    mainTest << "" << std::endl;
    mainTest << "int main(int argc, char** argv) {" << std::endl;
    mainTest << "    srand(time(NULL));  // init random seed" << std::endl;
    mainTest << "    // the main function that is running the tests" << std::endl;
    mainTest << "    // init logger" << std::endl;
    mainTest << "    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;" << std::endl;
    mainTest << "    // add file logger" << std::endl;
    mainTest << "    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(\"data/log_gametest.txt\", 1024 * 1024 * 10, 3);" << std::endl;
    mainTest << "    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);" << std::endl;
    mainTest << "    // plog::init(plog::verbose, &fileAppender);" << std::endl;
    mainTest << "    testing::InitGoogleTest(&argc, argv);" << std::endl;
    mainTest << "    return RUN_ALL_TESTS();" << std::endl;
    mainTest << "}" << std::endl;
}
