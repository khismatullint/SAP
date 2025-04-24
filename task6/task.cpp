#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <cmath>
#include <sstream>
#include <iomanip>

static std::vector<std::string> flatten(const std::string& json)
{
    static const std::regex re(R"([A-Za-z0-9_]+)");
    std::sregex_iterator it(json.begin(), json.end(), re), end;
    std::vector<std::string> out;
    for (; it != end; ++it) out.push_back(it->str());
    return out;
}



double kendall_tau(const std::string& json1, const std::string& json2)
{
    // 1. Разворачиваем ранжировки
    std::vector<std::string> r1 = flatten(json1);
    std::vector<std::string> r2 = flatten(json2);

    const size_t n = r1.size();
    if (n != r2.size() || n < 2) return std::nan("1");

    // 2. Проверяем, что множества объектов совпадают
    std::unordered_map<std::string, int> pos1, pos2;
    for (size_t i = 0; i < n; ++i) pos1[r1[i]] = static_cast<int>(i);
    for (size_t i = 0; i < n; ++i) pos2[r2[i]] = static_cast<int>(i);
    if (pos1.size() != n || pos2.size() != n || pos1.size() != pos2.size())
        return std::nan("1");                       // дубликаты или разные множества

    // 3. Подсчитываем согласованные / несогласованные пары
    long long concordant = 0, discordant = 0;
    for (size_t i = 0; i < n - 1; ++i)
        for (size_t j = i + 1; j < n; ++j)
        {
            const auto& a = r1[i];
            const auto& b = r1[j];
            int sign1 = pos1[a] - pos1[b];          // всегда < 0 (i < j)
            int sign2 = pos2[a] - pos2[b];          // знак во второй ранжировке
            (sign1 * sign2 > 0 ? concordant : discordant)++;
        }

    const long long total_pairs = static_cast<long long>(n) * (n - 1) / 2;
    double tau = static_cast<double>(concordant - discordant) / total_pairs;

    // Округляем до двух знаков
    return std::round(tau * 100.0) / 100.0;
}

#include <iostream>
int main() {
    double tau = kendall_tau(
        "[1,[2,3],4,[5,6,7],8,9,10]",
        "[[1,2],[3,4,5,],6,7,9,[8,10]]"
    );
    std::cout << std::fixed << std::setprecision(2) << tau << '\n'; // -0.33
}

