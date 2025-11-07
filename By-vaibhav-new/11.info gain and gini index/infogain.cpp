#include <bits/stdc++.h>
using namespace std;

double entropy(const map<string, int>& classCount) {
    double total = 0.0;
    for (auto &p : classCount) total += p.second;
    double ent = 0.0;
    for (auto &p : classCount) {
        if (p.second == 0) continue;
        double p_i = p.second / total;
        ent -= p_i * log2(p_i);
    }
    return ent;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename = "data.csv";
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot open " << filename << endl;
        return 1;
    }

    string line;
    getline(file, line);
    vector<string> headers;
    stringstream ss(line);
    string col;
    while (getline(ss, col, ',')) headers.push_back(col);

    int numCols = headers.size();
    vector<vector<string>> data;
    while (getline(file, line)) {
        stringstream s(line);
        vector<string> row;
        string val;
        while (getline(s, val, ',')) row.push_back(val);
        if (row.size() == numCols)
            data.push_back(row);
    }
    file.close();

    int total = data.size();
    string classCol = headers.back();

    map<string, int> classCount;
    for (auto &row : data) classCount[row.back()]++;

    double totalEntropy = entropy(classCount);
    cout << fixed << setprecision(6);
    cout << "Overall Entropy = " << totalEntropy << "\n\n";

    for (int attr = 0; attr < numCols - 1; ++attr) {
        map<string, map<string, int>> subsets;
        for (auto &row : data) {
            string attrVal = row[attr];
            string classVal = row.back();
            subsets[attrVal][classVal]++;
        }

        double weightedEntropy = 0.0;
        for (auto &kv : subsets) {
            double subsetSize = 0;
            for (auto &p : kv.second) subsetSize += p.second;
            weightedEntropy += (subsetSize / total) * entropy(kv.second);
        }

        double infoGain = totalEntropy - weightedEntropy;

        cout << "Attribute: " << headers[attr] << "\n";
        cout << "Weighted Entropy = " << weightedEntropy << "\n";
        cout << "Information Gain = " << infoGain << "\n\n";
    }

    return 0;
}