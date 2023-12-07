
# Add the executable target
add_executable(PokerWorkshop main.cpp deck.cpp)
# Include headers
target_include_directories(PokerWorkshop PUBLIC ${INCLUDE_DIR})

# Link with plog library
target_link_libraries(PokerWorkshop plog)