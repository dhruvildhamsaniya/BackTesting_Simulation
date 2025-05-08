#include <iostream>

void generate_token_map(const std::string &, const std::string &);
void run_simulation(const std::string &, const std::string &);

int main() {
    try {
        std::string contractsFolder = "data/contracts";
        std::string tokenMapPath = "token_map.csv";

        generate_token_map(contractsFolder, tokenMapPath);
        run_simulation(tokenMapPath, contractsFolder);
    } catch (const std::exception &ex) {
        std::cerr << "[Error] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
