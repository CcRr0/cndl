#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>

std::vector<std::string> split_lines(const std::string &str);
bool is_blank(const std::string &str);
void pbcopy(const std::string &str);

#endif // UTILITY_HPP
