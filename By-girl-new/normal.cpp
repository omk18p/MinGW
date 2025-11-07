// #include <iostream>
// #include <vector>
// using namespace std;

// double absValue(double x) {
//     return (x < 0) ? -x : x;
// }

// double powFunction(double base, int exp) {
//     double result = 1;
//     for (int i = 0; i < exp; i++) {
//         result *= base;
//     }
//     return result;
// }

// double squareRootFunction(double x) {
//     if (x == 0) return 0;
//     double guess = x;
//     for (int i = 0; i < 20; i++) { 
//         guess = (guess + x / guess) / 2.0;
//     }
//     return guess;
// }

// void minMaxNormalization(vector<double> arr, double new_min, double new_max) {
//     double minA = arr[0], maxA = arr[0];

    
//     for (int i = 1; i < arr.size(); i++) {
//         if (arr[i] < minA) minA = arr[i];
//         if (arr[i] > maxA) maxA = arr[i];
//     }

//     cout << "\nMin-Max Normalization (Range [" << new_min << ", " << new_max << "]):\n";
//     for (int i = 0; i < arr.size(); i++) {

//         double v_normalized = ((arr[i] - minA) / (maxA - minA)) * (new_max - new_min) + new_min;

//         cout << v_normalized << " ";
//     }
//     cout << endl;
// }


// void z_score_Normalization(vector<double> arr) {
//     double sum = 0, mean,  std_deviation= 0;
//     int n = arr.size();

//     for (int i = 0; i < n; i++)
//       sum += arr[i];
//     mean = sum / n;

//     for (int i = 0; i < n; i++) {
//         double difference = arr[i] - mean;
//        std_deviation +=  difference * difference ;//variance
//     }

//     std_deviation = squareRootFunction(std_deviation/ n); 

//     cout << "\nZ-Score Normalization:\n";
//     for (int i = 0; i < n; i++) {

//         double v_normalized = (arr[i] - mean) /  std_deviation;

//         cout <<  v_normalized << " ";
//     }
//     cout << endl;
// }


// void decimalScalingNormalization(vector<double> arr) {
//     double maxVal = 0;

    
//     for (int i = 0; i < arr.size(); i++) {
//         if (absValue(arr[i]) > maxVal) maxVal = absValue(arr[i]);
//     }

//     int j = 0;
//     while (maxVal >= 1) {
//         maxVal /= 10;
//         j++;
//     }

//     cout << "\nDecimal Scaling Normalization:\n";
//     for (int i = 0; i < arr.size(); i++) {
//         double v_norm = arr[i] / powFunction(10, j); 
//         cout << v_norm << " ";
//     }
//     cout << endl;
// }

// int main() {
//     int n, choice;
//     cout << "Enter size of array: ";
//     cin >> n;

//     vector<double> v(n);
//     cout << "Enter elements:\n";
//     for (int i = 0; i < n; i++) 
//     cin >> v[i];

//     cout << "\nChoose Normalization Method:\n";
//     cout << "1) Min-Max Normalization\n";
//     cout << "2) Z-Score Normalization\n";
//     cout << "3) Decimal Scaling Normalization\n";
//     cout << "Enter Choice: ";
//     cin >> choice;

//     switch (choice) {
//         case 1: {
//             double new_min, new_max;
//             cout << "Enter new_min and new_max: ";
//             cin >> new_min >> new_max;
//             minMaxNormalization(v, new_min, new_max);
//             break;
//         }
//         case 2:
//             z_score_Normalization(v);
//             break;
//         case 3:
//             decimalScalingNormalization(v);
//             break;
//         default:
//             cout << "Your choice is Invalid." << endl;
//     }

//     return 0;
// }

// with csv
#include <bits/stdc++.h>
using namespace std;

// Check if a string is numeric
bool isFloat(const string &s) {
    try {
        stod(s);
        return true;
    } catch (...) {
        return false;
    }
}

// Normalize a single column
vector<double> normalizeColumn(const vector<double>& col, int method) {
    vector<double> normCol;
    if (method == 1) { // Min-Max
        double minVal = *min_element(col.begin(), col.end());
        double maxVal = *max_element(col.begin(), col.end());
        for (double x : col)
            normCol.push_back((maxVal != minVal) ? (x - minVal) / (maxVal - minVal) : 0.0);
    } 
    else if (method == 2) { // Z-Score
        double mean = accumulate(col.begin(), col.end(), 0.0) / col.size();
        double sqSum = 0;
        for (double x : col) sqSum += (x - mean) * (x - mean);
        double stddev = sqrt(sqSum / col.size());
        for (double x : col)
            normCol.push_back((stddev != 0) ? (x - mean) / stddev : 0.0);
    } 
    else if (method == 3) { // Decimal Scaling
        double maxAbs = 0;
        for (double x : col) maxAbs = max(maxAbs, fabs(x));
        int j = (maxAbs != 0) ? to_string((long long)maxAbs).length() : 1;
        for (double x : col) normCol.push_back(x / pow(10, j));
    }
    return normCol;
}

// Apply normalization to selected columns
vector<vector<string>> applyNormalization(
    int method,
    const vector<string>& header,
    const vector<vector<string>>& rows,
    const vector<int>& numCols)
{
    int colCount = header.size();
    vector<vector<string>> columns(colCount);
    for (int c = 0; c < colCount; c++) {
        for (auto &row : rows) {
            columns[c].push_back(row[c]);
        }
    }

    vector<vector<string>> normalizedCols;

    for (int c = 0; c < colCount; c++) {
        if (find(numCols.begin(), numCols.end(), c) != numCols.end()) {
            vector<double> col;
            for (auto &v : columns[c]) col.push_back(stod(v));

            vector<double> normCol = normalizeColumn(col, method);

            vector<string> normStr;
            for (double val : normCol) {
                ostringstream oss;
                oss << fixed << setprecision(6) << val;
                normStr.push_back(oss.str());
            }
            normalizedCols.push_back(normStr);
        }
        else {
            normalizedCols.push_back(columns[c]);
        }
    }

    // Transpose back to rows
    vector<vector<string>> normalizedRows(rows.size(), vector<string>(colCount));
    for (int c = 0; c < colCount; c++) {
        for (int r = 0; r < rows.size(); r++) {
            normalizedRows[r][c] = normalizedCols[c][r];
        }
    }

    return normalizedRows;
}

// Save CSV
void saveCSV(const vector<string>& header, const vector<vector<string>>& normalizedRows, const string& outFile) {
    ofstream out(outFile);
    for (int i = 0; i < header.size(); i++) {
        out << header[i] << (i < header.size() - 1 ? "," : "\n");
    }
    for (auto &row : normalizedRows) {
        for (int i = 0; i < row.size(); i++) {
            out << row[i] << (i < row.size() - 1 ? "," : "\n");
        }
    }
    out.close();
    cout << "Normalized data saved to '" << outFile << "'\n";
}

// Print preview of first 5 rows
void printPreview(const vector<string>& header, const vector<vector<string>>& rows, const string& title) {
    cout << "\n--- " << title << " ---\n";
    for (auto &h : header) cout << h << "\t";
    cout << "\n";
    for (int r = 0; r < min((int)rows.size(), 5); r++) {
        for (auto &cell : rows[r]) cout << cell << "\t";
        cout << "\n";
    }
}

int main() {
    string fileName;
    cout << "Enter the CSV file name (with extension): ";
    cin >> fileName;

    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "File not found." << endl;
        return 1;
    }

    vector<string> header;
    vector<vector<string>> rows;
    string line;

    // Read header
    if (getline(file, line)) {
        stringstream ss(line);
        string cell;
        while (getline(ss, cell, ',')) {
            header.push_back(cell);
        }
    }

    // Read rows
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        rows.push_back(row);
    }
    file.close();

    // Ask user which columns to normalize
    vector<int> numCols;
    cout << "\nColumns in the CSV:\n";
    for (int i = 0; i < header.size(); i++) {
        cout << i << ": " << header[i] << "\n";
    }

    cout << "Enter column numbers to normalize (space-separated, e.g., 0 2 3): ";
    cin.ignore();
    string input;
    getline(cin, input);
    stringstream ss(input);
    int colNum;
    while (ss >> colNum) {
        if (colNum >= 0 && colNum < header.size()) {
            numCols.push_back(colNum);
        } else {
            cout << "Ignoring invalid column: " << colNum << "\n";
        }
    }

    if (numCols.empty()) {
        cout << "No valid columns selected. Exiting.\n";
        return 1;
    }

    // Choose normalization method
    int choice;
    cout << "\nChoose Normalization Method:\n";
    cout << "1. Min-Max Normalization\n";
    cout << "2. Z-Score Normalization\n";
    cout << "3. Decimal Scaling Normalization\n";
    cout << "4. All Methods\n";
    cout << "Enter choice (1/2/3/4): ";
    cin >> choice;

    if (choice >= 1 && choice <= 3) {
        auto normalizedRows = applyNormalization(choice, header, rows, numCols);
        printPreview(header, normalizedRows, "Normalized Data");
        
        string outFile;
        if (choice == 1) outFile = "minmax_normalized.csv";
        else if (choice == 2) outFile = "zscore_normalized.csv";
        else if (choice == 3) outFile = "decimalscaling_normalized.csv";

        saveCSV(header, normalizedRows, outFile);
    } 
    else if (choice == 4) {
        vector<string> titles = {"Min-Max Normalization", "Z-Score Normalization", "Decimal Scaling Normalization"};
        vector<string> outFiles = {"minmax_normalized.csv", "zscore_normalized.csv", "decimalscaling_normalized.csv"};
        for (int m = 1; m <= 3; m++) {
            auto normalizedRows = applyNormalization(m, header, rows, numCols);
            printPreview(header, normalizedRows, titles[m-1]);
            saveCSV(header, normalizedRows, outFiles[m-1]);
        }
    } 
    else {
        cout << "Invalid choice." << endl;
    }

    return 0;
}
