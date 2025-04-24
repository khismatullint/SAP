#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

int main() {
    constexpr int SIDES = 6;
    constexpr int TOTAL = SIDES * SIDES;           // 36 исходов
    constexpr double INV_TOTAL = 1.0 / TOTAL;

    // Максимальные значения для сумм и произведений
    const int MAX_SUM  = 12;   // 2..12
    const int MAX_PROD = 36;   // 1..36

    std::vector<std::vector<int>> joint(MAX_SUM + 1, std::vector<int>(MAX_PROD + 1, 0));
    std::vector<int> sum_count(MAX_SUM + 1, 0);
    std::vector<int> prod_count(MAX_PROD + 1, 0);

    // Перебираем все упорядоченные пары (d1,d2)
    for (int d1 = 1; d1 <= SIDES; ++d1) {
        for (int d2 = 1; d2 <= SIDES; ++d2) {
            int s = d1 + d2;
            int p = d1 * d2;
            joint[s][p] += 1;
            sum_count[s] += 1;
            prod_count[p] += 1;
        }
    }

    auto entropy = [](const std::vector<int>& counts, int total) -> double {
        double H = 0.0;
        for (int c : counts) {
            if (c == 0) continue;
            double prob = static_cast<double>(c) / total;
            H -= prob * std::log2(prob);
        }
        return H;
    };

    // Рассчитываем энтропии
    double H_A = entropy(sum_count, TOTAL);

    // entropy of product
    double H_B = entropy(prod_count, TOTAL);

    // Joint entropy
    double H_AB = 0.0;
    for (int s = 2; s <= MAX_SUM; ++s) {
        for (int p = 1; p <= MAX_PROD; ++p) {
            int c = joint[s][p];
            if (c == 0) continue;
            double prob = static_cast<double>(c) / TOTAL;
            H_AB -= prob * std::log2(prob);
        }
    }

    double H_B_given_A = H_AB - H_A;               // условная энтропия
    double I_AB = H_B - H_B_given_A;               // взаимная информация

    // Вывод
    std::cout << std::fixed << std::setprecision(2)
            << '[' << H_AB << ", "
            << H_A  << ", "
            << H_B  << ", "
            << H_B_given_A << ", "
            << I_AB << ']' << std::endl;

    return 0;
}