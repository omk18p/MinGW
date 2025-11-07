
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

// Function to multiply two matrices
vector<vector<double>> multiply(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    int r = A.size(), c = B[0].size(), k = B.size();
    vector<vector<double>> result(r, vector<double>(c, 0));
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            for (int t = 0; t < k; ++t)
                result[i][j] += A[i][t] * B[t][j];
    return result;
}

// Function to transpose a matrix
vector<vector<double>> transpose(const vector<vector<double>>& A) {
    int r = A.size(), c = A[0].size();
    vector<vector<double>> T(c, vector<double>(r, 0));
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            T[j][i] = A[i][j];
    return T;
}

// Function to invert a 2x2 or 3x3 matrix (for small examples)
vector<vector<double>> inverse(const vector<vector<double>>& A) {
    int n = A.size();
    vector<vector<double>> I(n, vector<double>(n, 0));
    for (int i = 0; i < n; ++i) I[i][i] = 1;

    // Simple Gauss-Jordan elimination
    vector<vector<double>> M = A;
    for (int i = 0; i < n; ++i) {
        double diag = M[i][i];
        for (int j = 0; j < n; ++j) {
            M[i][j] /= diag;
            I[i][j] /= diag;
        }
        for (int k = 0; k < n; ++k) {
            if (k == i) continue;
            double factor = M[k][i];
            for (int j = 0; j < n; ++j) {
                M[k][j] -= factor * M[i][j];
                I[k][j] -= factor * I[i][j];
            }
        }
    }
    return I;
}

int main() {
    string filename = "house.csv"; // CSV with features + price
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 0;
    }

    vector<vector<double>> X; // Features + intercept
    vector<vector<double>> Y; // Dependent variable
    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        vector<double> row = {1}; // Add intercept term
        while (getline(ss, val, ',')) {
            row.push_back(stod(val));
        }
        Y.push_back({row.back()}); // last column is price
        row.pop_back();
        X.push_back(row);
    }
    file.close();

    cout << fixed << setprecision(4);
    cout << "\nStep 1: Feature Matrix X and Y\n";
    cout << "X Matrix:\n";
    for (auto &r : X) { for (auto &v : r) cout << v << " "; cout << endl; }
    cout << "Y Vector:\n";
    for (auto &r : Y) { cout << r[0] << endl; }

    // Step 2: Compute b = (X^T X)^-1 X^T Y
    auto Xt = transpose(X);
    auto XtX = multiply(Xt, X);
    cout << "\nStep 2: X^T * X\n";
    for (auto &r : XtX) { for (auto &v : r) cout << v << " "; cout << endl; }

    auto XtX_inv = inverse(XtX);
    cout << "\nStep 3: (X^T * X)^-1\n";
    for (auto &r : XtX_inv) { for (auto &v : r) cout << v << " "; cout << endl; }

    auto XtY = multiply(Xt, Y);
    cout << "\nStep 4: X^T * Y\n";
    for (auto &r : XtY) cout << r[0] << endl;

    auto B = multiply(XtX_inv, XtY);
    cout << "\nStep 5: Regression Coefficients (b0, b1, ...)\n";
    for (int i = 0; i < B.size(); ++i) {
        cout << "b" << i << " = " << B[i][0] << endl;
    }

    // Step 6: Predict
    vector<double> input;
    cout << "\nEnter feature values separated by space (Size Bedrooms Location): ";
    double val;
    input.push_back(1); // intercept
    while (cin >> val) input.push_back(val);
    double predicted = 0;
    for (int i = 0; i < B.size(); ++i) predicted += B[i][0] * input[i];
    cout << "Predicted Price = " << predicted << endl;

    return 0;
}
