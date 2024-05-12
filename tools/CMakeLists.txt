# Add the executable target
add_executable(hand_strengths ${SRC_DIR}/deck.cpp ${THAND_STRENGTHS_DIR}/main.cpp ${THAND_STRENGTHS_DIR}/hand_utils.cpp)
add_executable(data_analytics ${TDATA_ANALYTICS_DIR}/main.cpp)

# Include headers
target_include_directories(hand_strengths PUBLIC ${INCLUDE_DIR})
target_include_directories(data_analytics PUBLIC ${INCLUDE_DIR})

# Link with plog library
target_link_libraries(hand_strengths plog)
target_link_libraries(data_analytics plog)