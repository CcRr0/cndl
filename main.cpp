#include <iostream>
#include <print>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <ranges>
#include <regex>

const std::regex re_import(R"(\s*//\s*@import\s*\(\s*(.+)\s*\)\s*)");
const std::regex re_ignore(R"(.*//\s*@ignore.*)");
std::filesystem::path root;

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
                        | std::views::filter([](const std::string &li) {
                            return !std::regex_match(li, re_ignore);
                        })
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
        } else {
            oss << line << "\n";
            flag = false;
        }
    }

    return oss.str();
}

void pbcopy(const std::string &str) {
    FILE *pipe = popen("pbcopy", "w");
    fwrite(str.c_str(), sizeof(char), str.size(), pipe);
    pclose(pipe);
}

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
