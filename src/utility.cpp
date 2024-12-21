#include "utility.hpp"

#include <sstream>
#include <ranges>
#include <algorithm>
#include <cstdio>
#include <cctype>

std::vector<std::string> split_lines(const std::string &str) {
    std::istringstream stream(str);
    std::vector<std::string> lines;
    std::string li;
    while(std::getline(stream, li)) {
        lines.push_back(li);
    }
    return lines;
}

bool is_blank(const std::string &str) {
    return std::ranges::all_of(str, [](const char c) {
        return std::isspace(c);
    });
}

void pbcopy(const std::string &str) {
    FILE *pipe = popen("pbcopy", "w");
    fwrite(str.c_str(), sizeof(char), str.size(), pipe);
    pclose(pipe);
}
