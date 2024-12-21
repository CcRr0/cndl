#ifndef BUNDLE_HPP
#define BUNDLE_HPP

#include <filesystem>
#include <fstream>
#include <string>

std::string bundle(std::ifstream &file, const std::filesystem::path &base, int &success, int &total);

#endif // BUNDLE_HPP
