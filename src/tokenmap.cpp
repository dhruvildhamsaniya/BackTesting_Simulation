#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

bool ends_with(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper function to extract strike price from contract name
int extract_strike(const std::string& symbol) {
    std::smatch match;
    std::regex pattern(R"((\d{5})(CE|PE))");  // Extract 5-digit strike before CE/PE
    if (std::regex_search(symbol, match, pattern)) {
        return std::stoi(match[1]);
    }
    return 0;
}

// Comparator for sorting contracts: Symbol, Option Type (CE before PE), and Strike Price
bool contract_comparator(const std::string& a, const std::string& b) {
    // Compare by symbol first
    if (a != b) {
        return a < b;
    }

    // Compare by option type (CE < PE)
    std::string a_opt = a.substr(a.size() - 2);
    std::string b_opt = b.substr(b.size() - 2);
    if (a_opt != b_opt) {
        return a_opt < b_opt;
    }

    // Compare by strike price
    return extract_strike(a) < extract_strike(b);
}

void generate_token_map(const std::string &folder, const std::string &output) {
    std::vector<std::string> files;

    // Read all CSV files in the folder
    for (const auto &entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() == ".csv")
            files.push_back(entry.path().filename().string());
    }

    std::vector<std::string> options, spot, future;
    for (const auto &f : files) {
        if (f.find("SPOT") != std::string::npos)
            spot.push_back(f);
        else if (f.find("FUT") != std::string::npos)
            future.push_back(f);
        else
            options.push_back(f);
    }

    // Sort the options based on the custom comparator
    std::sort(options.begin(), options.end(), contract_comparator);

    std::ofstream out(output);
    int token = 1001;

    // Write spot contracts to output
    for (const auto &s : spot)
        out << token++ << "," << s << "\n";

    // Write future contracts to output
    for (const auto &f : future)
        out << token++ << "," << f << "\n";

    // Write sorted option contracts to output
    for (const auto &o : options)
        out << token++ << "," << o << "\n";

    std::cout << "[TokenMap] Generated with " << (token - 1001) << " entries.\n";
}
