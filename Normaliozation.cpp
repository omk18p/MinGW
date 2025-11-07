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
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF DATA NORMALIZATION TECHNIQUES PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program demonstrates **three major Data Normalization techniques** used in **Data Preprocessing**
// to scale numeric data into a specific range or format before applying data mining or ML algorithms.
//
// Normalization ensures that all features contribute equally and prevents attributes with large
// numerical ranges from dominating smaller ones.
//
// Techniques implemented:
//   1️⃣ Min-Max Normalization
//   2️⃣ Z-Score Normalization (Standardization)
//   3️⃣ Decimal Scaling Normalization
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ split()
//     - Splits each line from the CSV file using commas as delimiters.
//     - Returns a vector of string tokens (each representing a column value).
//
// ➤ isNumeric()
//     - Checks whether a given string value is numeric.
//     - Used to skip textual headers or string-type columns.
//
// ➤ readCSV()
//     - Reads a CSV file and stores numeric data in a 2D vector `data`.
//     - Automatically detects if the first line contains column headers.
//
// ➤ calcStats()
//     - Computes for each numeric column:
//           ▪ Minimum value (minVal)
//           ▪ Maximum value (maxVal)
//           ▪ Mean value (meanVal)
//           ▪ Standard deviation (stdVal)
//     - These statistics are essential for normalization formulas.
//
// ➤ minMaxNorm()
//     - Applies **Min-Max Normalization** using the formula:
//           X' = ((X - Min) / (Max - Min)) * (NewMax - NewMin) + NewMin
//     - Scales all values to a user-defined range (e.g., [0, 1] or [-1, 1]).
//
// ➤ zScoreNorm()
//     - Applies **Z-Score Normalization (Standardization)** using the formula:
//           X' = (X - Mean) / StdDev
//     - Centers data around zero with a standard deviation of 1.
//
// ➤ decScaleNorm()
//     - Applies **Decimal Scaling Normalization** using the formula:
//           X' = X / 10^k
//       where k is the smallest integer such that max(|X'|) < 1.
//
// ➤ showData()
//     - Displays the first few normalized rows for quick verification.
//
// ➤ saveCSV()
//     - Saves normalized data into separate CSV files for each method.
//
// ➤ main()
//     - Handles user interaction, method selection, and file saving.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ STEP-BY-STEP EXECUTION
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → INPUT
//     - The user enters the CSV file name.
//     - The program reads numeric columns and ignores text-based columns (like labels or IDs).
//
// Example Input CSV:
//     Age, Income, Score
//     25, 50000, 70
//     30, 60000, 90
//     45, 80000, 85
//
// STEP 2️⃣ → STATISTICS CALCULATION
//     - The program computes column-wise Min, Max, Mean, and StdDev.
//
// Example Output:
//     Age -> Min=25, Max=45, Mean=33.33, StdDev=8.49
//     Income -> Min=50000, Max=80000, Mean=63333.3, StdDev=12472.2
//     Score -> Min=70, Max=90, Mean=81.67, StdDev=8.50
//
// STEP 3️⃣ → METHOD SELECTION
//     - User selects one of the following:
//           1 → Min-Max
//           2 → Z-Score
//           3 → Decimal Scaling
//           4 → All Methods
//
// STEP 4️⃣ → NORMALIZATION
//
// ▪ **Min-Max Normalization**
//     Formula: X' = ((X - Min) / (Max - Min)) * (NewMax - NewMin) + NewMin
//     Example (for [0,1] range):
//         Age = (25 - 25) / (45 - 25) = 0
//         Income = (50000 - 50000) / (80000 - 50000) = 0
//         Score = (70 - 70) / (90 - 70) = 0
//
// ▪ **Z-Score Normalization**
//     Formula: X' = (X - Mean) / StdDev
//     Example:
//         Age = (25 - 33.33) / 8.49 = -0.981
//         Income = (50000 - 63333.3) / 12472.2 = -1.070
//         Score = (70 - 81.67) / 8.50 = -1.373
//
// ▪ **Decimal Scaling Normalization**
//     Formula: X' = X / 10^k, where k = smallest integer s.t. max(|X'|) < 1
//     Example:
//         For Income (max = 80000), k = ceil(log10(80000+1)) = 5
//         So, Income = 50000 / 10^5 = 0.5
//
// STEP 5️⃣ → OUTPUT
//     - Displays a preview of normalized data.
//     - Saves results to separate CSV files:
//         → minmax_normalized.csv
//         → zscore_normalized.csv
//         → decimalscaling_normalized.csv
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ FORMULAS USED
// --------------------------------------------------------------------------------------------------
//
// ▪ Min-Max Normalization:
//       X' = ((X - Min) / (Max - Min)) * (NewMax - NewMin) + NewMin
//
// ▪ Z-Score Normalization:
//       X' = (X - Mean) / StdDev
//
// ▪ Decimal Scaling Normalization:
//       X' = X / 10^k,  where k = ceil(log10(Max(|X|) + 1))
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ USE CASES AND SIGNIFICANCE
// --------------------------------------------------------------------------------------------------
//
// 🔹 Why Normalization is Needed:
//     - Many data mining algorithms (e.g., K-Means, KNN, Neural Networks) are distance-based.
//     - Attributes with larger ranges dominate smaller ones without normalization.
//     - Normalization brings all features to a **comparable scale**.
//
// 🔹 Typical Use Cases:
//     - Preprocessing before clustering (e.g., K-Means, DBSCAN).
//     - Data preparation for Machine Learning models.
//     - Feature scaling in predictive analytics.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ COMPARISON OF METHODS
// --------------------------------------------------------------------------------------------------
//
// | Technique               | Range / Distribution     | When to Use                                         |
// |--------------------------|--------------------------|-----------------------------------------------------|
// | Min-Max Normalization    | [newMin, newMax] (usually [0,1]) | When minimum and maximum values are known.           |
// | Z-Score Normalization    | Mean = 0, StdDev = 1     | When data distribution varies or contains outliers. |
// | Decimal Scaling          | Depends on magnitude     | When you want a simple scale-based normalization.   |
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ ADVANTAGES
// --------------------------------------------------------------------------------------------------
//
// ✅ All techniques reduce feature magnitude differences.
// ✅ Makes models converge faster and perform better.
// ✅ Simple, efficient, and easily interpretable.
// ✅ Handles datasets of varying scales and ranges.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Min-Max Normalization: Sensitive to outliers.
// ⚠️ Z-Score Normalization: Requires normally distributed data for best results.
// ⚠️ Decimal Scaling: Simple but less precise for diverse ranges.
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ WHY THESE METHODS WERE CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Objective:
//     - To demonstrate key **data preprocessing methods** that prepare datasets for data mining algorithms.
//
// 🔹 Why These Three:
//     1️⃣ They represent the **three most common normalization approaches**.
//     2️⃣ Each one scales data differently, showing varied effects.
//     3️⃣ They are foundational in **data cleaning and transformation** steps.
//
// 🔹 Importance in Data Mining:
//     - Normalization improves clustering and classification accuracy by reducing bias.
//     - Helps in creating balanced, comparable feature spaces.
//
// --------------------------------------------------------------------------------------------------
// 🔸 9️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ This experiment successfully demonstrates **three major normalization methods**.
// ➤ It computes descriptive statistics and scales the dataset accordingly.
// ➤ Results are stored in CSV files for further analysis.
//
// Example Conclusion Statement:
//     “Normalization was applied to the dataset using Min-Max, Z-Score, and Decimal Scaling methods.
//      Each method transformed the original data into a comparable scale, improving suitability for
//      data mining algorithms such as K-Means and Naïve Bayes.”
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This program effectively illustrates **Data Preprocessing in Data Mining**, focusing on normalization.
// It standardizes raw data into defined scales, enhancing model performance and interpretability.
//
// ==================================================================================================
