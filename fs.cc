#pragma once
#include <fstream>
#include <filesystem>
#include <vector>

namespace fs {
    void open(std::string filename, std::vector<std::string>& lines) {
        if (!std::filesystem::exists(filename)) {
            throw std::runtime_error("Unable to open the file");
        }

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("No permissions to open the file");
        }

        
        while(file.good()) {
            std::string line;
            std::getline(file, line);
            lines.push_back(line);
        }

        file.close();
    }

    // TODO: this sucks because no "atomic writes" yet - write to some file in temporary dir and swap with original
    void save(std::string filename, const std::vector<std::string>& lines) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to save this file");
        }

        for (int i = 0; i < lines.size(); ++i) {
            file << lines[i];

            if (i < lines.size() - 1) {
                file << '\n';
            }
        }

        file.close();
    }
}
