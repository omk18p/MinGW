#include <bits/stdc++.h>
using namespace std;

// ---------- Helper Functions ----------
double entropy(const vector<string>& y) {
    map<string,int> freq;
    for (auto &v : y) freq[v]++;
    double e = 0, n = y.size();
    for (auto &p : freq) {
        double prob = (double)p.second / n;
        e -= prob * log2(prob);
    }
    return e;
}

double gini(const vector<string>& y) {
    map<string,int> freq;
    for (auto &v : y) freq[v]++;
    double g = 1, n = y.size();
    for (auto &p : freq) {
        double prob = (double)p.second / n;
        g -= prob * prob;
    }
    return g;
}

// ---------- Main ----------
int main() {
    string fname;
    cout << "Enter CSV filename (with extension): ";
    cin >> fname;

    ifstream f(fname);
    if (!f.is_open()) {
        cerr << "Error: File not found!\n";
        return 1;
    }

    string line, cell;
    vector<string> headers;
    vector<vector<string>> data;

    // Read header row
    if (getline(f, line)) {
        stringstream ss(line);
        while (getline(ss, cell, ',')) headers.push_back(cell);
    }

    // Read data rows
    while (getline(f, line)) {
        stringstream ss(line);
        vector<string> row;
        while (getline(ss, cell, ',')) row.push_back(cell);
        if (row.size() == headers.size()) data.push_back(row);
    }
    f.close();

    if (data.empty()) {
        cerr << "Error: No valid data found in file.\n";
        return 1;
    }

    int targetIdx = headers.size() - 1;
    cout << "\nTarget column: " << headers[targetIdx] << "\n";

    // Collect target values
    vector<string> targetVals;
    for (auto &r : data) targetVals.push_back(r[targetIdx]);

    double totalEntropy = entropy(targetVals);

    cout << fixed << setprecision(4);
    cout << "\n=== Attribute Evaluation ===\n";

    double bestGain = -1;
    string bestAttr;

    // Evaluate each attribute (except target)
    for (int a = 0; a < targetIdx; a++) {
        map<string, vector<string>> groups;
        for (auto &r : data)
            groups[r[a]].push_back(r[targetIdx]);

        double weightedEntropy = 0.0;
        double weightedGini = 0.0;

        for (auto &grp : groups) {
            double w = (double)grp.second.size() / data.size();
            weightedEntropy += w * entropy(grp.second);
            weightedGini += w * gini(grp.second);
        }

        double infoGain = totalEntropy - weightedEntropy;

        cout << "\nAttribute: " << headers[a]
             << "\n  Info Gain = " << infoGain
             << "\n  Gini Index = " << weightedGini << "\n";

        if (infoGain > bestGain) {
            bestGain = infoGain;
            bestAttr = headers[a];
        }
    }

    cout << "\n=== Best Attribute for Split ===\n";
    cout << "Based on Information Gain -> " << bestAttr
         << " (Gain = " << bestGain << ")\n";

    cout << "\nEvaluation Complete.\n";
    return 0;
}