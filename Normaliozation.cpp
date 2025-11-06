#include <bits/stdc++.h>
using namespace std;

// Split a line by comma
vector<string> split(const string &s, char delimiter = ',') {
    vector<string> tokens;
    string token;
    stringstream ss(s);
    while (getline(ss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

// Check if string is numeric
bool isNumeric(const string &s) {
    if (s.empty()) return false;
    bool dotSeen = false;
    for (char c : s) {
        if (isdigit(c) || c == '-' || c == '+') continue;
        if (c == '.' && !dotSeen) { dotSeen = true; continue; }
        return false;
    }
    return true;
}

// Read CSV file and detect header
void readCSV(const string &filename, vector<string> &header, vector<vector<double>> &data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file.\n";
        exit(1);
    }

    string line;
    bool headerChecked = false;

    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (!headerChecked) {
            headerChecked = true;
            bool hasText = false;
            for (auto &v : row) {
                if (!isNumeric(v)) { 
                    hasText = true; 
                    break; 
                }
            }
            if (hasText) {
                header = row;
                continue;
            }
        }

        vector<double> numericRow;
        for (auto &v : row) {
            if (isNumeric(v)) numericRow.push_back(stod(v));
        }
        if (!numericRow.empty()) data.push_back(numericRow);
    }
    file.close();
}

// Compute min, max, mean, and standard deviation
void calcStats(const vector<vector<double>> &data,
               vector<double> &minVal, vector<double> &maxVal,
               vector<double> &meanVal, vector<double> &stdVal) {
    int rows = data.size();
    int cols = data[0].size();

    minVal.assign(cols, numeric_limits<double>::max());
    maxVal.assign(cols, numeric_limits<double>::lowest());
    meanVal.assign(cols, 0.0);
    stdVal.assign(cols, 0.0);

    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            minVal[j] = min(minVal[j], data[i][j]);
            maxVal[j] = max(maxVal[j], data[i][j]);
            meanVal[j] += data[i][j];
        }
        meanVal[j] /= rows;
    }

    for (int j = 0; j < cols; j++) {
        double sumSq = 0;
        for (int i = 0; i < rows; i++)
            sumSq += pow(data[i][j] - meanVal[j], 2);
        stdVal[j] = sqrt(sumSq / rows);
    }
}

// Min-Max normalization
vector<vector<double>> minMaxNorm(const vector<vector<double>> &data,
                                  const vector<double> &minVal,
                                  const vector<double> &maxVal,
                                  double newMin, double newMax) {
    int rows = data.size();
    int cols = data[0].size();
    vector<vector<double>> norm(rows, vector<double>(cols));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            norm[i][j] = ((data[i][j] - minVal[j]) / (maxVal[j] - minVal[j])) 
                         * (newMax - newMin) + newMin;
        }
    }
    return norm;
}

// Z-Score normalization
vector<vector<double>> zScoreNorm(const vector<vector<double>> &data,
                                  const vector<double> &meanVal,
                                  const vector<double> &stdVal) {
    int rows = data.size();
    int cols = data[0].size();
    vector<vector<double>> norm(rows, vector<double>(cols));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (stdVal[j] != 0)
                norm[i][j] = (data[i][j] - meanVal[j]) / stdVal[j];
            else
                norm[i][j] = 0;
        }
    }
    return norm;
}

// Decimal scaling normalization
vector<vector<double>> decScaleNorm(const vector<vector<double>> &data) {
    int rows = data.size();
    int cols = data[0].size();
    vector<vector<double>> norm(rows, vector<double>(cols));

    for (int j = 0; j < cols; j++) {
        double maxAbs = 0;
        for (int i = 0; i < rows; i++)
            maxAbs = max(maxAbs, fabs(data[i][j]));
        int k = (maxAbs == 0) ? 1 : ceil(log10(maxAbs + 1));
        for (int i = 0; i < rows; i++)
            norm[i][j] = data[i][j] / pow(10, k);
    }
    return norm;
}

// Display a preview of data
void showData(const vector<string> &header, const vector<vector<double>> &data, const string &title) {
    cout << "\n--- " << title << " ---\n";
    if (!header.empty()) {
        for (auto &h : header)
            cout << setw(12) << h;
        cout << endl;
    }
    for (int i = 0; i < min((int)data.size(), 5); i++) {
        for (auto &v : data[i])
            cout << setw(12) << fixed << setprecision(4) << v;
        cout << endl;
    }
}

// Save data to CSV
void saveCSV(const string &filename, const vector<string> &header, const vector<vector<double>> &data) {
    ofstream out(filename);
    if (!header.empty()) {
        for (int i = 0; i < header.size(); i++) {
            out << header[i];
            if (i < header.size() - 1) out << ",";
        }
        out << "\n";
    }
    for (auto &row : data) {
        for (int j = 0; j < row.size(); j++) {
            out << row[j];
            if (j < row.size() - 1) out << ",";
        }
        out << "\n";
    }
    out.close();
    cout << "Saved: " << filename << endl;
}

// Main function
int main() {
    string filename;
    cout << "Enter CSV file name: ";
    cin >> filename;

    vector<string> header;
    vector<vector<double>> data;
    readCSV(filename, header, data);

    vector<double> minVal, maxVal, meanVal, stdVal;
    calcStats(data, minVal, maxVal, meanVal, stdVal);

    cout << "\n--- Column Statistics ---\n";
    for (int i = 0; i < minVal.size(); i++) {
        if (!header.empty()) cout << header[i] << " -> ";
        cout << "Min=" << minVal[i]
             << ", Max=" << maxVal[i]
             << ", Mean=" << meanVal[i]
             << ", StdDev=" << stdVal[i] << endl;
    }

    int choice;
    cout << "\nSelect Normalization Method:\n";
    cout << "1. Min-Max Normalization\n";
    cout << "2. Z-Score Normalization\n";
    cout << "3. Decimal Scaling Normalization\n";
    cout << "4. All Methods\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1 || choice == 4) {
        double newMin, newMax;
        cout << "Enter new minimum value: ";
        cin >> newMin;
        cout << "Enter new maximum value: ";
        cin >> newMax;
        auto norm = minMaxNorm(data, minVal, maxVal, newMin, newMax);
        showData(header, norm, "Min-Max Normalization");
        saveCSV("minmax_normalized.csv", header, norm);
    }

    if (choice == 2 || choice == 4) {
        auto norm = zScoreNorm(data, meanVal, stdVal);
        showData(header, norm, "Z-Score Normalization");
        saveCSV("zscore_normalized.csv", header, norm);
    }

    if (choice == 3 || choice == 4) {
        auto norm = decScaleNorm(data);
        showData(header, norm, "Decimal Scaling Normalization");
        saveCSV("decimalscaling_normalized.csv", header, norm);
    }

    return 0;
}
