# BackTesting Simulator

This project is a backtesting simulator for evaluating trading strategies.

---

## 🔧 How to Build the Project

Open your terminal and run the following commands:

```bash
mkdir build
cd build
mkdir data
cd data
mkdir contracts (store all contract files here)
cmake ..
make
./simulator 

backtesting_simulator/
├── build/
│   ├── data/
│   │   └── contracts/
│   │       └── [your contract files go here]
│   │── token_map.csv
│   ├── [build output files like binaries, CMake files]
├── src/
├── include/
├── CMakeLists.txt
├── .gitignore
└── README.md

