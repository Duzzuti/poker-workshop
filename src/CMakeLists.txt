

set(PLAYER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/players)
set(CHECK_PLAYER ${PLAYER_DIR}/check_player/check_player.cpp)
set(RAND_PLAYER ${PLAYER_DIR}/rand_player/rand_player.cpp)

# Add the executable target
add_executable(PokerWorkshop main.cpp deck.cpp game.cpp player.cpp utils.cpp hand_strengths.cpp ${CHECK_PLAYER} ${RAND_PLAYER})
# Include headers
target_include_directories(PokerWorkshop PUBLIC ${INCLUDE_DIR})
target_include_directories(PokerWorkshop PUBLIC ${PLAYER_DIR}) 


# Link with plog library
target_link_libraries(PokerWorkshop plog)