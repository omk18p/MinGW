#include <bits/stdc++.h>
using namespace std;

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(s);
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void generateCubes(const vector<vector<string>> &data, int measureIndex) {
    int rows = data.size();
    if (rows == 0) return;

    int cols = data[0].size();
    vector<int> dimIndexes;
    for (int i = 0; i < cols; ++i)
        if (i != measureIndex) dimIndexes.push_back(i);

    map<vector<string>, double> cubeResults;

    int dimCount = dimIndexes.size();
    for (int mask = 0; mask < (1 << dimCount); ++mask) {
        for (int r = 0; r < rows; ++r) {
            vector<string> key;
            for (int d = 0; d < dimCount; ++d) {
                if (mask & (1 << d))
                    key.push_back(data[r][dimIndexes[d]]);
                else
                    key.push_back("ALL");
            }
            double val = stod(data[r][measureIndex]);
            cubeResults[key] += val;
        }
    }

    cout << "\n=== Dynamic OLAP Cube Results ===\n";
    for (auto &entry : cubeResults) {
        for (auto &dimVal : entry.first)
            cout << setw(10) << left << dimVal << " ";
        cout << " -> " << entry.second << "\n";
    }
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!\n";
        return 1;
    }

    string line;
    vector<vector<string>> data;
    vector<string> headers;

    bool isHeader = true;
    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (isHeader) {
            headers = row;
            isHeader = false;
        } else {
            data.push_back(row);
        }
    }

    cout << "\nAvailable columns:\n";
    for (int i = 0; i < headers.size(); ++i)
        cout << i << ": " << headers[i] << endl;

    int measureIndex;
    cout << "Enter index of measure (numeric) column: ";
    cin >> measureIndex;

    generateCubes(data, measureIndex);

    return 0;
}
