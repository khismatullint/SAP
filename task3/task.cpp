#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

static std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delim))
        out.push_back(token);
    return out;
}

double task(std::istream& in) {
    std::vector<std::vector<double>> M;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        for (char& ch : line)
            if (ch == '\t' || ch == ';') ch = ',';
        auto tokens = split(line, ',');
        std::vector<double> row;
        for (const auto& t : tokens)
            if (!t.empty()) row.push_back(std::stod(t));
        if (!row.empty()) M.push_back(std::move(row));
    }
    const std::size_t n = M.size();
    if (n == 0) return 0.0;
    const double denom = (n > 1) ? static_cast<double>(n - 1) : 1.0;

    double H = 0.0;
    for (const auto& row : M)
        for (double l : row)
            if (l > 0.0) {
                double p = l / denom;
                H -= p * std::log2(p);
            }

    return std::round(H * 10.0) / 10.0; 
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <matrix.csv>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << '\n';
        return 1;
    }
    double H = task(file);
    std::cout << std::fixed << std::setprecision(1) << H << '\n';
    return 0;
}