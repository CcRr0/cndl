#include <iostream>
#include <print>
#include <filesystem>
#include <fstream>
#include <string>

#include "bundle.hpp"
#include "utility.hpp"

std::filesystem::path root;

int main(const int argc, const char *argv[]) {
    if(argc != 2) {
        std::println(std::cerr, "Usage: {} <file>", argv[0]);
        return 1;
    }

    const std::string path = argv[1];
    std::filesystem::path fs_path(path);
    std::ifstream file(fs_path);

    if(!file.is_open()) {
        std::println(std::cerr, "Could not open file: {}", absolute(fs_path).string());
        return 1;
    }

    root = fs_path.parent_path();

    int success = 0, total = 0;
    const std::string res = bundle(file, root, success, total);

    std::println("Total: {} bytes ({} / {})", res.size(), success, total);
    pbcopy(res);

    file.close();
    return 0;
}
