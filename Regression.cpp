#include <bits/stdc++.h>
using namespace std;

// ---------- Read CSV File ----------
vector<vector<string>> readCSV(const string &filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            // Remove extra spaces
            while (!cell.empty() && cell.front() == ' ') cell.erase(cell.begin());
            while (!cell.empty() && cell.back() == ' ') cell.pop_back();
            row.push_back(cell);
        }
        data.push_back(row);
    }
    file.close();
    return data;
}

// ---------- Check if String is Numeric ----------
bool isNumeric(const string &s) {
    if (s.empty()) return false;
    try {
        stod(s);
        return true;
    } catch (...) {
        return false;
    }
}

// ---------- Main ----------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string filename;
    cout << "Enter CSV filename (e.g. data.csv): " << flush;
    getline(cin >> ws, filename); // handles spaces properly

    vector<vector<string>> data = readCSV(filename);
    if (data.empty()) {
        cerr << "Error: Empty or invalid CSV file.\n";
        return 1;
    }

    vector<string> headers = data[0];
    cout << "\nColumns detected:\n";
    for (size_t i = 0; i < headers.size(); ++i)
        cout << i + 1 << ". " << headers[i] << endl;

    int colX, colY;
    cout << "\nSelect column number for X: " << flush;
    cin >> colX;
    cout << "Select column number for Y: " << flush;
    cin >> colY;

    colX--; colY--; // Convert 1-based index to 0-based
    if (colX >= headers.size() || colY >= headers.size()) {
        cerr << "Error: Invalid column numbers.\n";
        return 1;
    }

    vector<double> x, y;
    for (size_t i = 1; i < data.size(); ++i) {
        if (colX < data[i].size() && colY < data[i].size()) {
            string sx = data[i][colX];
            string sy = data[i][colY];
            if (isNumeric(sx) && isNumeric(sy)) {
                x.push_back(stod(sx));
                y.push_back(stod(sy));
            }
        }
    }

    int n = x.size();
    if (n == 0) {
        cerr << "Error: No valid numeric data found in selected columns.\n";
        return 1;
    }

    // ---------- Intermediate Steps ----------
    double sumx = accumulate(x.begin(), x.end(), 0.0);
    double sumy = accumulate(y.begin(), y.end(), 0.0);
    double sumxy = 0, sumx2 = 0;
    for (int i = 0; i < n; i++) {
        sumxy += x[i] * y[i];
        sumx2 += x[i] * x[i];
    }

    cout << "\n---------- Intermediate Calculations ----------\n";
    cout << "ΣX = " << sumx << ", ΣY = " << sumy << "\n";
    cout << "ΣXY = " << sumxy << ", ΣX² = " << sumx2 << "\n";
    cout << "n = " << n << "\n";

    // ---------- Regression Equation ----------
    double b1 = (n * sumxy - sumx * sumy) / (n * sumx2 - sumx * sumx);
    double b0 = (sumy - b1 * sumx) / n;

    cout << fixed << setprecision(4);
    cout << "\n========== Linear Regression Result ==========\n";
    cout << "Selected Columns: X = " << headers[colX]
         << ", Y = " << headers[colY] << "\n";
    cout << "Regression Equation: Y = " << b0 << " + " << b1 << " * X\n";

    // ---------- Prediction ----------
    double x_pred;
    cout << "\nEnter X to predict Y: " << flush;
    cin >> x_pred;
    double y_pred = b0 + b1 * x_pred;
    cout << "Predicted Y = " << y_pred << "\n";

    cout << "==============================================\n";
    return 0;
}