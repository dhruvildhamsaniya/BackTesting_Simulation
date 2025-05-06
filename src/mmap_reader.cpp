#include "memory_map.h"
#include "constants.h"
#include <iostream>
#include <unistd.h> // for usleep

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <token> [refresh_ms]\n";
        std::cerr << "Example: " << argv[0] << " 1001 500\n";
        return 1;
    }

    int token = std::stoi(argv[1]);
    int refresh_ms = argc > 2 ? std::stoi(argv[2]) : 1000; // default 1 second
    
    std::cout << "Reading token " << token << " every " << refresh_ms << "ms\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    while (true) {
        Backtesting::ContractData data;
        if (Backtesting::MemoryMap::readSharedContractData("/backtesting_mmap", token, data)) {
            std::cout << "----- " << data.ticker << " -----\n";
            std::cout << "Token: " << data.token << "\n";
            std::cout << "Type: " << (data.type == Backtesting::InstrumentType::OPTION ? "OPTION" : 
                                     data.type == Backtesting::InstrumentType::FUTURE ? "FUTURE" : "SPOT") << "\n";
            std::cout << "LTP: " << data.ltp << "\n";
            std::cout << "Bid: " << data.bid << "\n";
            std::cout << "Ask: " << data.ask << "\n";
            std::cout << "Time: " << data.timestamp << " (" 
                      << (data.timestamp/3600) << "h " 
                      << ((data.timestamp%3600)/60) << "m "
                      << (data.timestamp%60) << "s)\n\n";
        } else {
            std::cerr << "Failed to read contract data for token " << token << "\n";
        }
        
        usleep(refresh_ms * 1000); // microseconds
    }
    
    return 0;
}