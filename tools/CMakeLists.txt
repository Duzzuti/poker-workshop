# Add the executable target
add_executable(hand_strengths ${SRC_DIR}/deck.cpp hand_strengths/main.cpp hand_strengths/hand_utils.cpp)
# Include headers
target_include_directories(hand_strengths PUBLIC ${INCLUDE_DIR})

# Link with plog library
target_link_libraries(hand_strengths plog)