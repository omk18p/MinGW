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

// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF SIMPLE LINEAR REGRESSION PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements **Simple Linear Regression (SLR)** — one of the fundamental techniques
// in **Prediction and Data Mining**.  
// It establishes a **mathematical relationship between two numeric variables (X and Y)**,
// allowing prediction of Y for any given value of X.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ THEORY OF LINEAR REGRESSION
// --------------------------------------------------------------------------------------------------
//
// ➤ Objective:
// To find a straight line (best fit line) that models the relationship between dependent variable (Y)
// and independent variable (X).
//
// ➤ Equation of Regression Line:
//     Y = b₀ + b₁ * X
//
// where:
//     b₀ = Intercept  (value of Y when X = 0)
//     b₁ = Slope of regression line (change in Y per unit change in X)
//
// ➤ The slope (b₁) and intercept (b₀) are computed using the **Least Squares Method**:
//
//     b₁ = [ n(ΣXY) - (ΣX)(ΣY) ] / [ n(ΣX²) - (ΣX)² ]
//     b₀ = (ΣY - b₁ΣX) / n
//
// The line minimizes the sum of squared vertical distances between observed values and predicted values.
//
// ➤ Prediction:
// Once b₀ and b₁ are known, Y for any input X can be predicted using:
//     Ŷ = b₀ + b₁ * X
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ readCSV()
//     - Reads the input CSV dataset line by line.
//     - Splits each row by commas.
//     - Removes leading/trailing spaces and stores the data in a 2D vector.
//
// ➤ isNumeric()
//     - Checks whether a string represents a valid numeric value (integer or decimal).
//     - Helps skip non-numeric data in chosen columns.
//
// ➤ main()
//     - Handles overall workflow:
//         1️⃣ Reads CSV file
//         2️⃣ Displays available columns
//         3️⃣ User selects X and Y columns
//         4️⃣ Extracts numeric values
//         5️⃣ Computes regression coefficients (b₀, b₁)
//         6️⃣ Prints regression equation
//         7️⃣ Predicts Y for a given X
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ STEP-BY-STEP EXECUTION
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → INPUT DATA
//     - User provides a CSV file (e.g., `student_scores.csv`).
//     - Program displays all columns and allows user to select which will act as:
//         X = independent variable
//         Y = dependent variable
//
// Example CSV:
//     Hours, Marks
//     1, 50
//     2, 55
//     3, 65
//     4, 70
//     5, 80
//
// User chooses: X = Hours, Y = Marks
//
// STEP 2️⃣ → INTERMEDIATE SUMMATIONS
//     The program computes the following intermediate values:
//
//     ΣX   = sum of all X values
//     ΣY   = sum of all Y values
//     ΣXY  = sum of product of X and Y
//     ΣX²  = sum of squares of X
//     n    = number of records
//
// Example Output:
//     ΣX = 15, ΣY = 320
//     ΣXY = 1050, ΣX² = 55, n = 5
//
// STEP 3️⃣ → COMPUTE REGRESSION PARAMETERS
//
//     b₁ = [ n(ΣXY) - (ΣX)(ΣY) ] / [ n(ΣX²) - (ΣX)² ]
//     b₀ = (ΣY - b₁ΣX) / n
//
// Example Calculation:
//     b₁ = [5(1050) - 15(320)] / [5(55) - 15²] = (5250 - 4800) / (275 - 225) = 450 / 50 = 9
//     b₀ = (320 - 9(15)) / 5 = (320 - 135) / 5 = 185 / 5 = 37
//
// Therefore, regression equation is:
//     Y = 37 + 9X
//
// STEP 4️⃣ → PREDICTION
//     User enters a test value for X (e.g., X = 6).
//     The program predicts:
//         Y = 37 + 9 * 6 = 91
//
//     Output:
//         Predicted Y = 91
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ EXAMPLE OUTPUT
// --------------------------------------------------------------------------------------------------
//
// Enter CSV filename (e.g. data.csv): student_scores.csv
//
// Columns detected:
// 1. Hours
// 2. Marks
//
// Select column number for X: 1
// Select column number for Y: 2
//
// ---------- Intermediate Calculations ----------
// ΣX = 15, ΣY = 320
// ΣXY = 1050, ΣX² = 55
// n = 5
//
// ========== Linear Regression Result ==========
// Selected Columns: X = Hours, Y = Marks
// Regression Equation: Y = 37.0000 + 9.0000 * X
//
// Enter X to predict Y: 6
// Predicted Y = 91.0000
// ==============================================
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ VARIABLES USED
// --------------------------------------------------------------------------------------------------
//
// filename         → input CSV filename
// data[][]         → 2D vector containing dataset rows
// headers[]        → column names (from first row)
// colX, colY       → selected column indices for X and Y
// x[], y[]         → numeric values extracted from dataset
// sumx, sumy       → summations ΣX and ΣY
// sumxy, sumx2     → ΣXY and ΣX² respectively
// b0, b1           → regression coefficients (intercept, slope)
// x_pred, y_pred   → user input for prediction and predicted value
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ WHY LINEAR REGRESSION WAS USED (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Type:
//     - Contains two **numeric attributes** with a potential **linear relationship**.
//
// 🔹 Objective:
//     - To **predict** the dependent variable (Y) based on the independent variable (X).
//
// 🔹 Why Simple Linear Regression:
//     1️⃣ It identifies the **best-fit line** through data points.
//     2️⃣ Provides an easy-to-understand mathematical model for prediction.
//     3️⃣ Useful for **forecasting and trend analysis** in numeric datasets.
//     4️⃣ Forms the foundation for more complex models (multiple regression, polynomial regression).
//
// 🔹 Applications:
//     - Predicting student marks from study hours
//     - Estimating sales from advertising spend
//     - Predicting temperature, profit, or growth based on linear trends
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ ADVANTAGES
// --------------------------------------------------------------------------------------------------
//
// ✅ Simple to implement and interpret.
// ✅ Provides clear cause-effect relationship between X and Y.
// ✅ Useful for both explanatory and predictive modeling.
// ✅ Performs well when data shows a linear trend.
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Assumes linear relationship — not suitable for non-linear data.
// ⚠️ Sensitive to outliers, which can distort regression coefficients.
// ⚠️ Works only for numeric data.
// ⚠️ Correlation does not imply causation — linear relation doesn’t always mean dependency.
//
// --------------------------------------------------------------------------------------------------
// 🔸 9️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The Simple Linear Regression model successfully computes the relationship between two numeric
//     variables using the **Least Squares Method**.
//
// ➤ The equation derived (Y = b₀ + b₁X) can predict future or unknown Y values for any given X.
//
// ➤ Example Conclusion:
//     “Based on regression analysis, the dependent variable Y increases linearly with X.
//      The best-fit regression line obtained can be used for accurate numeric prediction.”
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Prediction in Data Mining** using the **Simple Linear Regression**
// technique. It helps discover linear relationships between numeric attributes and predict outcomes
// based on them, forming the basis for more advanced predictive analytics.
//
// ==================================================================================================
