cmake_minimum_required(VERSION 3.10)
project(BacktestingSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add main simulator executable
add_executable(backtesting_simulator
    src/main.cpp
    src/simulator.cpp
    src/memory_map.cpp
    src/token_map.cpp
    src/utils.cpp
)

# Add mmap_reader executable
add_executable(mmap_reader
    src/mmap_reader.cpp
    src/memory_map.cpp
    src/utils.cpp  # Add if needed for any utility functions
)


# Include directories
target_include_directories(backtesting_simulator PRIVATE include)
target_include_directories(mmap_reader PRIVATE include)

# Link libraries (for mmap, etc.)
target_link_libraries(backtesting_simulator pthread rt)
target_link_libraries(mmap_reader pthread rt)

# Installation
install(TARGETS backtesting_simulator mmap_reader DESTINATION bin)
install(DIRECTORY data DESTINATION share/backtesting_simulator)