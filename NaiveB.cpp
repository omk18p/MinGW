#include <bits/stdc++.h>
using namespace std;

// Split CSV line by delimiter
vector<string> split(const string &s, char d) {
    vector<string> tokens; string temp; stringstream ss(s);
    while (getline(ss, temp, d)) tokens.push_back(temp);
    return tokens;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file.\n";
        return 0;
    }

    string line;
    getline(file, line);
    vector<string> headers = split(line, ',');
    int featureCount = headers.size() - 1;  // Last column = Class

    vector<vector<string>> data;
    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (row.size() == headers.size())
            data.push_back(row);
    }
    file.close();

    cout << "\n--- Dataset Loaded Successfully ---\n";
    cout << "Total records: " << data.size() << endl;
    cout << "Features: " << featureCount << endl;
    cout << "Target (Class): " << headers.back() << "\n";

    // Count class occurrences
    map<string, int> classCount;
    for (auto &r : data) classCount[r.back()]++;

    int total = data.size();
    cout << "\n=== PRIOR PROBABILITIES ===\n";
    map<string, double> prior;
    for (auto &c : classCount) {
        prior[c.first] = (double)c.second / total;
        cout << "P(" << c.first << ") = " << c.second << "/" << total
             << " = " << fixed << setprecision(3) << prior[c.first] << endl;
    }

    // Calculate conditional probabilities P(feature=value | class)
    cout << "\n=== CONDITIONAL PROBABILITIES ===\n";
    map<string, map<string, map<string, double>>> condProb;

    for (int i = 0; i < featureCount; i++) {
        map<string, map<string, int>> freq;
        for (auto &r : data) {
            freq[r.back()][r[i]]++;
        }

        for (auto &cls : classCount) {
            cout << "\nFor Class = " << cls.first
                 << " (" << headers[i] << "):\n";
            for (auto &pair : freq[cls.first]) {
                condProb[headers[i]][cls.first][pair.first] =
                    (double)pair.second / classCount[cls.first];
                cout << "P(" << pair.first << " | " << cls.first << ") = "
                     << pair.second << "/" << classCount[cls.first]
                     << " = " << condProb[headers[i]][cls.first][pair.first] << endl;
            }
        }
    }

    // Get test case input
    cout << "\n=== ENTER TEST CASE ===\n";
    map<string, string> test;
    for (int i = 0; i < featureCount; i++) {
        cout << headers[i] << ": ";
        cin >> test[headers[i]];
    }

    cout << "\n=== POSTERIOR PROBABILITIES ===\n";
    map<string, double> posterior;

    for (auto &cls : classCount) {
        double prob = prior[cls.first];
        cout << "\nFor class = " << cls.first << ":\n";
        cout << "Start with prior P(" << cls.first << ") = " << prob << endl;

        for (int i = 0; i < featureCount; i++) {
            string feat = headers[i], val = test[feat];
            double cond = condProb[feat][cls.first].count(val)
                              ? condProb[feat][cls.first][val]
                              : 0.0;
            cout << "P(" << val << " | " << cls.first << ") = " << cond << endl;
            prob *= cond;
        }

        posterior[cls.first] = prob;
        cout << "Final P(" << cls.first << " | Case) = " << prob << endl;
    }

    cout << "\n=== COMPARISON ===\n";
    for (auto &p : posterior)
        cout << "P(" << p.first << " | Case) = " << p.second << endl;

    string prediction = max_element(posterior.begin(), posterior.end(),
                                    [](auto &a, auto &b) {
                                        return a.second < b.second;
                                    })
                            ->first;

    cout << "\nPredicted Class = " << prediction << endl;

    return 0;
}
