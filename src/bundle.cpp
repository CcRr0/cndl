#include "bundle.hpp"
#include "utility.hpp"

#include <iostream>
#include <print>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <ranges>
#include <regex>

extern std::filesystem::path root;

const std::regex re_import(R"(\s*//\s*@import\s*\(\s*(.+)\s*\)\s*)");
const std::regex re_ignore(R"(.*//\s*@ignore.*)");
const std::regex re_replace(R"(\s*//\s*@replace\s*\(\s*(.+)\s*\)\s*)");

std::string bundle(std::ifstream &file, const std::filesystem::path &base, int &success, int &total) {
    std::ostringstream oss;
    std::string line;

    bool flag = false;
    while(std::getline(file, line)) {
        if(std::smatch match; std::regex_match(line, match, re_import)) {
            const std::string path = match[1];
            const std::filesystem::path fs_path(base / path);
            std::ifstream source(fs_path);

            const auto rel_path = relative(fs_path, root);
            total += 1;

            if(!source.is_open()) {
                std::println("Failed: ./{}", rel_path.string());
                oss << line << "\n";
                continue;
            }

            const std::string src = bundle(source, fs_path.parent_path(), success, total);

            std::println("Import: ./{} ({} bytes)", rel_path.string(), src.size());
            success += 1;

            const auto lines = split_lines(src);
            auto view = lines
                        | std::views::drop_while(is_blank)
                        | std::views::reverse
                        | std::views::drop_while(is_blank)
                        | std::views::reverse;

            if(flag) {
                oss << "\n";
            }
            std::ranges::copy(view, std::ostream_iterator<std::string>(oss, "\n"));

            source.close();
            flag = true;
        } else if(std::regex_match(line, match, re_replace)) {
            const std::string repl = match[1];
            oss << repl << "\n";
            flag = false;
        } else if(!std::regex_match(line, re_ignore)) {
            oss << line << "\n";
            flag = false;
        }
    }

    return oss.str();
}
