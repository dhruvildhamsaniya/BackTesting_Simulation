# BackTesting Simulator

This project is a backtesting simulator for evaluating trading strategies.

## 🔧 How to Build the Project

Open your terminal and run the following commands:

mkdir build
cd build
cmake ..
make

# project structure

backtesting_simulator/
├── build/
│   ├── data/
│   │   └── contracts/
│   │       └── [your contract files go here]
|   |   |__ token_map.csv
│   ├── [build output files like binaries, CMake files]
├── src/
├── CMakeLists.txt
├── .gitignore
└── include/
