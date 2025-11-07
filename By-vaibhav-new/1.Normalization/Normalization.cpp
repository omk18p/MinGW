#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <cmath>
using namespace std;

// ---------- Read CSV ----------
vector<vector<string>> read_csv(string &filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        vector<string> row;
        string cell;
        stringstream ss(line);
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

// ---------- Min-Max Normalization ----------
vector<vector<double>> minMaxNormalize(vector<vector<double>> &data, double newMin, double newMax) {
    int rows = data.size();
    int cols = data[0].size();

    vector<double> minVal(cols, INT_MAX), maxVal(cols, INT_MIN);

    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            if (data[i][j] < minVal[j]) minVal[j] = data[i][j];
            if (data[i][j] > maxVal[j]) maxVal[j] = data[i][j];
        }
    }

    vector<vector<double>> normalized(rows, vector<double>(cols, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (maxVal[j] != minVal[j])
                normalized[i][j] = (data[i][j] - minVal[j]) / (maxVal[j] - minVal[j]) * (newMax - newMin) + newMin;
            else
                normalized[i][j] = 0;
        }
    }
    return normalized;
}

// ---------- Z-Score Normalization ----------
vector<vector<double>> zScoreNormalize(vector<vector<double>> &data) {
    int rows = data.size();
    int cols = data[0].size();

    vector<double> mean(cols, 0), stddev(cols, 0);

    for (int j = 0; j < cols; j++) {
        double sum = 0;
        for (int i = 0; i < rows; i++) sum += data[i][j];
        mean[j] = sum / rows;

        double sq_sum = 0;
        for (int i = 0; i < rows; i++) sq_sum += pow(data[i][j] - mean[j], 2);
        stddev[j] = sqrt(sq_sum / rows);
    }

    vector<vector<double>> normalized(rows, vector<double>(cols, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (stddev[j] != 0)
                normalized[i][j] = (data[i][j] - mean[j]) / stddev[j];
            else
                normalized[i][j] = 0;
        }
    }
    return normalized;
}

// ---------- Write CSV ----------
void write_csv(string filename, vector<vector<string>> &header, vector<vector<double>> &normalized) {
    ofstream output(filename);
    int cols = header[0].size();

    for (int i = 0; i < cols; i++) {
        output << header[0][i];
        if (i < cols - 1) output << ",";
    }
    output << "\n";

    for (int i = 0; i < normalized.size(); i++) {
        for (int j = 0; j < cols; j++) {
            output << fixed << setprecision(2) << normalized[i][j];
            if (j < cols - 1) output << ",";
        }
        output << "\n";
    }
    output.close();
}

int main() {
    string filename = "data.csv";
    vector<vector<string>> data = read_csv(filename);

    if (data.empty()) {
        cout << "Empty file!" << endl;
        return 0;
    }

    vector<string> header = data[0];
    data.erase(data.begin());

    int rows = data.size();
    int cols = data[0].size();

    vector<vector<double>> double_data(rows, vector<double>(cols, 0));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            try {
                double_data[i][j] = stod(data[i][j]);
            } catch (...) {
                double_data[i][j] = 0;
            }
        }
    }

    // ----- Perform both normalizations -----
    vector<vector<double>> minmax = minMaxNormalize(double_data, 0, 1);
    vector<vector<double>> zscore = zScoreNormalize(double_data);

    // ----- Write results -----
    vector<vector<string>> headerVec = { header };
    write_csv("normalized_minmax.csv", headerVec, minmax);
    write_csv("normalized_zscore.csv", headerVec, zscore);

    cout << "✅ Min-Max and Z-Score normalization completed.\n";
    cout << "Files saved as 'normalized_minmax.csv' and 'normalized_zscore.csv'.\n";

    return 0;
}
