#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <csv_path> <row_index> <col_index>\n";
        return 1;
    }

    const std::string csv_path = argv[1];
    long row_idx = std::stol(argv[2]);
    long col_idx = std::stol(argv[3]);

    if (row_idx < 0 || col_idx < 0) {
        std::cerr << "Row and column indices must be non‑negative.\n";
        return 1;
    }

    std::ifstream file(csv_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << csv_path << "\n";
        return 1;
    }

    std::string line;
    long current_row = 0;
    while (std::getline(file, line)) {
        if (current_row == row_idx) {
            std::vector<std::string> cells;
            std::stringstream ss(line);
            std::string cell;

            while (std::getline(ss, cell, ',')) {
                cells.push_back(cell);
            }

            if (col_idx >= static_cast<long>(cells.size())) {
                std::cerr << "Column index out of bounds.\n";
                return 1;
            }

            std::cout << cells[col_idx] << std::endl;
            return 0;
        }
        ++current_row;
    }

    std::cerr << "Row index out of bounds.\n";
    return 1;
}