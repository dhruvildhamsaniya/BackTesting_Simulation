#include "constants.hpp"
#include "memorymap.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>

// Get current RSS memory in bytes
size_t getCurrentRSS() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string label;
            size_t sizeKB;
            std::string unit;
            iss >> label >> sizeKB >> unit;
            return sizeKB * 1024;
        }
    }
    return 0;
}

// Convert HH:MM:SS to seconds offset from 09:15:00
uint64_t time_to_offset(const std::string &timeStr) {
    int h, m, s;
    sscanf(timeStr.c_str(), "%d:%d:%d", &h, &m, &s);
    return (h - 9) * 3600 + (m - 15) * 60 + s;
}

void run_simulation(const std::string &tokenMapPath, const std::string &contractDir) {
    std::ifstream tokenFile(tokenMapPath);
    std::map<int, std::string> tokenToFile;
    std::string line;

    // Map tokens to contract filenames
    while (std::getline(tokenFile, line)) {
        auto pos = line.find(',');
        int token = std::stoi(line.substr(0, pos));
        std::string file = line.substr(pos + 1);
        tokenToFile[token] = file;
    }

    // Load contract data: filename -> { timestamp -> (ltp, bid, ask) }
    std::unordered_map<std::string, std::unordered_map<int, std::tuple<float, float, float>>> contractData;

    // Track all unique timestamps
    std::set<int> allTimestamps;

    size_t memBefore = getCurrentRSS();

    for (const auto &[token, filename] : tokenToFile) {
        std::ifstream fin(contractDir + "/" + filename);
        std::string row;
        std::getline(fin, row); // skip header

        while (std::getline(fin, row)) {
            std::stringstream ss(row);
            std::string timeStr, ltpStr, bidStr, askStr;
            std::getline(ss, timeStr, ',');
            std::getline(ss, ltpStr, ',');
            std::getline(ss, bidStr, ',');
            std::getline(ss, askStr, ',');

            int timestamp = time_to_offset(timeStr);
            float ltp = std::stof(ltpStr);
            float bid = std::stof(bidStr);
            float ask = std::stof(askStr);

            contractData[filename][timestamp] = std::make_tuple(ltp, bid, ask);
            allTimestamps.insert(timestamp);
        }


    }

    size_t memAfter = getCurrentRSS();
    double usedMB = (memAfter - memBefore) / (1024.0 * 1024.0);

    std::cout << "[Memory Usage] contractData RAM: " << usedMB << " MB\n";

    MemoryMap memMap(tokenToFile.size());

    auto simStart = std::chrono::high_resolution_clock::now();

    std::cout << "[Simulation Started]\n";

    for (int timestamp : allTimestamps) {
        auto tickStart = std::chrono::high_resolution_clock::now();

        for (const auto &[token, filename] : tokenToFile) {
            auto &tsMap = contractData[filename];
            if (tsMap.count(timestamp)) {
                const auto &[ltp, bid, ask] = tsMap[timestamp];
                ContractData cd;
                cd.timestamp = timestamp;
                cd.ltp = ltp;
                cd.bid = bid;
                cd.ask = ask;
                memMap.write(token - 1001, cd);
            }
        }
        ////to test if data is updated at every second after adding some delay 
        // std::cout << "[Timestamp " << timestamp << "]\n";
        // for (size_t i = 0; i < tokenToFile.size(); ++i) {
        //     ContractData cd = memMap.read(i);
        //     std::cout << "Contract " << i
        //             << ": LTP=" << cd.ltp
        //             << ", Bid=" << cd.bid
        //             << ", Ask=" << cd.ask
        //             << ", Timestamp=" << cd.timestamp << "\n";
        // }

        auto tickEnd = std::chrono::high_resolution_clock::now();
        auto tickDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tickEnd - tickStart).count();

        int sleepMs = 1000 - static_cast<int>(tickDuration);
        if (sleepMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs));
        }
    }

    auto simEnd = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(simEnd - simStart).count();

    std::cout << "[Simulation Complete] Total Time: " << totalTime << " ms\n";

    memMap.dump_all(tokenToFile.size());
}
