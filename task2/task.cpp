#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <unordered_set>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <path_to_csv_file>\n";
        return 1;
    }

    const string filepath = argv[1];
    ifstream in(filepath);
    if (!in) {
        cerr << "Cannot open file: " << filepath << '\n';
        return 1;
    }

    vector<pair<int,int>> edges;
    string line;
    int max_id = 0;

    while (getline(in, line)) {
        if (line.empty()) continue;

        // allow tab‑separated too
        replace(line.begin(), line.end(), '\t', ',');

        stringstream ss(line);
        string token;
        vector<int> nums;
        while (getline(ss, token, ',')) {
            if (!token.empty()) nums.push_back(stoi(token));
        }
        if (nums.size() != 2) {
            cerr << "Malformed line: " << line << '\n';
            return 1;
        }
        int parent = nums[0], child = nums[1];
        edges.emplace_back(parent, child);
        max_id = max({max_id, parent, child});
    }
    in.close();

    int N = max_id;
    vector<vector<int>> children(N + 1);
    vector<vector<int>> parents(N + 1);

    for (auto [p, c] : edges) {
        children[p].push_back(c);
        parents[c].push_back(p);
    }

    vector<vector<int>> L(N + 1, vector<int>(5, 0)); // L[node][r]

    // r1 (children) and r2 (has parent)
    for (int node = 1; node <= N; ++node) {
        L[node][0] = static_cast<int>(children[node].size()); // r1
        L[node][1] = static_cast<int>(parents[node].size());  // r2 (0 or 1)
    }

    // r3 (indirect descendants) and r4 (indirect ancestors)
    for (int node = 1; node <= N; ++node) {
        unordered_set<int> visited;
        queue<pair<int,int>> q;          // (vertex, depth)
        q.emplace(node, 0);
        visited.insert(node);

        while (!q.empty()) {
            auto [cur, depth] = q.front();
            q.pop();
            for (int nxt : children[cur]) {
                if (visited.insert(nxt).second) {
                    q.emplace(nxt, depth + 1);
                    if (depth + 1 >= 2) {
                        ++L[node][2];  // r3 for source
                        ++L[nxt][3];   // r4 for descendant
                    }
                }
            }
        }
    }

    // r5 (siblings)
    for (int node = 1; node <= N; ++node) {
        unordered_set<int> sibs;
        for (int par : parents[node]) {
            for (int sib : children[par]) sibs.insert(sib);
        }
        sibs.erase(node);
        L[node][4] = static_cast<int>(sibs.size());
    }

    // Output CSV: each line r1,r2,r3,r4,r5
    for (int node = 1; node <= N; ++node) {
        cout << L[node][0];
        for (int r = 1; r < 5; ++r)
            cout << ',' << L[node][r];
        cout << '\n';
    }

    return 0;
}