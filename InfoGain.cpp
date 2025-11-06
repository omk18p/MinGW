#include <bits/stdc++.h>
using namespace std;

double entropy(const map<string, double> &classCounts) {
    double total = 0.0;
    for (auto &p : classCounts) total += p.second;
    if (total == 0) return 0.0;

    double e = 0.0;
    for (auto &p : classCounts) {
        double ratio = p.second / total;
        if (ratio > 0)
            e += -ratio * log2(ratio);
    }
    return e;
}

vector<vector<string>> readCSV(const string &filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            if (!cell.empty() && cell.front() == ' ') cell.erase(cell.begin());
            if (!cell.empty() && cell.back() == ' ') cell.pop_back();
            row.push_back(cell);
        }
        data.push_back(row);
    }
    return data;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    vector<vector<string>> data = readCSV(filename);
    if (data.empty()) {
        cout << "Error: File empty or invalid.\n";
        return 0;
    }

    vector<string> headers = data[0];
    int totalRecords = data.size() - 1;
    string target = headers.back();

    cout << fixed << setprecision(4);

    // Step 1: Calculate Parent Entropy
    map<string, double> totalTargetCount;
    for (int i = 1; i < data.size(); i++)
        totalTargetCount[data[i].back()]++;

    double totalEntropy = entropy(totalTargetCount);
    cout << "\nEntropy (Parent) = " << totalEntropy << endl;

    string bestAttr;
    double bestInfoGain = -1;

    // Step 2: For each attribute, calculate Information Gain
    for (int col = 0; col < headers.size() - 1; col++) {
        string attr = headers[col];
        cout << "\nFor Attribute: " << attr << endl;

        // Count how target classes distribute under each attribute value
        map<string, map<string, double>> valueClassCount;
        for (int i = 1; i < data.size(); i++)
            valueClassCount[data[i][col]][data[i].back()]++;

        double weightedEntropy = 0.0;

        for (auto &kv : valueClassCount) {
            string value = kv.first;
            double subsetTotal = 0.0;
            for (auto &cls : kv.second) subsetTotal += cls.second;

            double e = entropy(kv.second);
            weightedEntropy += (subsetTotal / totalRecords) * e;

            cout << "  Entropy(" << value << ") = " << e << " -> ";
            for (auto &cls : kv.second)
                cout << cls.first << "=" << cls.second << " ";
            cout << endl;
        }

        double infoGain = totalEntropy - weightedEntropy;
        cout << "  Information Gain (" << attr << ") = " << infoGain << endl;
        cout << "----------------------------------------" << endl;

        if (infoGain > bestInfoGain) {
            bestInfoGain = infoGain;
            bestAttr = attr;
        }
    }

    cout << "\nBest Attribute = " << bestAttr << "  (Gain = " << bestInfoGain << ")\n";
    return 0;
}