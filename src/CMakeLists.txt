# Add the executable target
add_executable(PokerWorkshop main.cpp deck.cpp game.cpp player.cpp ${CHECK_PLAYER} ${RAND_PLAYER} ${HUMAN_PLAYER} ${TEST_PLAYER})
# Include headers
target_include_directories(PokerWorkshop PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR})

# Link with plog library
target_link_libraries(PokerWorkshop plog)