# BackTesting Simulator

This project is a backtesting simulator for evaluating trading strategies.

---

## 🔧 How to Build the Project

Open your terminal and run the following commands:

```bash
mkdir build
cd build
cmake ..
make
./backtesting_simulator NIFTY02JAN2521600PE.NFO

open another terminal and run
./mmap_reader 1001 500

backtesting_simulator/
├── build/
│   ├── data/
│   │   └── contracts/
│   │       └── [your contract files go here]
│   │       └── token_map.csv
│   ├── [build output files like binaries, CMake files]
├── src/
├── include/
├── CMakeLists.txt
├── .gitignore
└── README.md

