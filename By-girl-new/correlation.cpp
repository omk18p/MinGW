// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <string>
// #include <cmath>

// using namespace std;

// // Function to compute Pearson correlation
// double pearsonCorrelation(const vector<double>& X, const vector<double>& Y) {
//     int n = X.size();
//     double meanX = 0, meanY = 0;
//     for (int i = 0; i < n; i++) {
//         meanX += X[i];
//         meanY += Y[i];
//     }
//     meanX /= n;
//     meanY /= n;

//     double sumXY = 0, sumX2 = 0, sumY2 = 0;
//     for (int i = 0; i < n; i++) {
//         sumXY += (X[i] - meanX) * (Y[i] - meanY);
//         sumX2 += (X[i] - meanX) * (X[i] - meanX);
//         sumY2 += (Y[i] - meanY) * (Y[i] - meanY);
//     }

//     return sumXY / sqrt(sumX2 * sumY2);
// }

// int main() {
//     string filename;
//     cout << "Enter CSV file name (with .csv): ";
//     cin >> filename;

//     ifstream file(filename);
//     if (!file.is_open()) {
//         cerr << "Error opening file!" << endl;
//         return 1;
//     }

//     string line;
//     vector<string> headers;
//     getline(file, line); // Read header
//     stringstream ss(line);
//     string col;
//     while (getline(ss, col, ',')) headers.push_back(col);

//     // Display column options
//     cout << "Columns found: ";
//     for (int i = 0; i < headers.size(); i++)
//         cout << i << ":" << headers[i] << "  ";
//     cout << "\nEnter index of first column: ";
//     int col1, col2;
//     cin >> col1;
//     cout << "Enter index of second column: ";
//     cin >> col2;

//     vector<double> X, Y;
//     while (getline(file, line)) {
//         stringstream ss(line);
//         string val;
//         vector<double> row;
//         while (getline(ss, val, ',')) row.push_back(stod(val));
//         if (row.size() > max(col1, col2)) {
//             X.push_back(row[col1]);
//             Y.push_back(row[col2]);
//         }
//     }
//     file.close();

//     if (X.empty() || Y.empty()) {
//         cerr << "No data found for selected columns!" << endl;
//         return 1;
//     }

//     double r = pearsonCorrelation(X, Y);
//     cout << "Pearson correlation coefficient (r) = " << r << endl;

//     if (fabs(r) < 1e-6) 
//         cout << "Correlation is approximately zero (no linear relationship)." << endl;
//     else if (r > 0)
//         cout << "Positive correlation." << endl;
//     else
//         cout << "Negative correlation." << endl;

//     return 0;
// }


// code that will print intermidiate step as well
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
using namespace std;
// Function to compute Pearson correlation with detailed steps
double pearsonCorrelation(const vector<double>& X, const vector<double>& Y) {
    int n = X.size();
    double meanX = 0, meanY = 0;
    
    // Compute means
    for (int i = 0; i < n; i++) {
        meanX += X[i];
        meanY += Y[i];
    }
    meanX /= n;
    meanY /= n;

    cout << "\nMean of X = " << meanX << endl;
    cout << "Mean of Y = " << meanY << endl;

    double sumXY = 0, sumX2 = 0, sumY2 = 0;

    // Print calculation table
    cout << "\n-------------------------------------------------------------\n";
    cout << setw(10) << "X"
         << setw(10) << "Y"
         << setw(15) << "X-meanX"
         << setw(15) << "Y-meanY"
         << setw(15) << "(X-meanX)(Y-meanY)"
         << setw(12) << "(X-meanX)^2"
         << setw(12) << "(Y-meanY)^2"
         << endl;
    cout << "-------------------------------------------------------------\n";

    for (int i = 0; i < n; i++) {
        double dx = X[i] - meanX;
        double dy = Y[i] - meanY;
        double prod = dx * dy;
        double dx2 = dx * dx;
        double dy2 = dy * dy;

        sumXY += prod;
        sumX2 += dx2;
        sumY2 += dy2;

        cout << setw(10) << X[i]
             << setw(10) << Y[i]
             << setw(15) << dx
             << setw(15) << dy
             << setw(15) << prod
             << setw(12) << dx2
             << setw(12) << dy2
             << endl;
    }

    cout << "-------------------------------------------------------------\n";
    cout << "\nSum[(X-meanX)(Y-meanY)] = " << sumXY << endl;
    cout << "Sum[(X-meanX)^2] = " << sumX2 << endl;
    cout << "Sum[(Y-meanY)^2] = " << sumY2 << endl;

    // Pearson correlation formula
    return sumXY / sqrt(sumX2 * sumY2);
}

int main() {
    string filename;
    cout << "Enter CSV file name : ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
    vector<string> headers;
    getline(file, line); // Read header
    stringstream ss(line);
    string col;
    while (getline(ss, col, ',')) headers.push_back(col);

    // Display column options
    cout << "Columns found:\n";
    for (int i = 0; i < headers.size(); i++)
        cout << i << " -> " << headers[i] << endl;

    int col1, col2;
    cout << "\nEnter index of first column: ";
    cin >> col1;
    cout << "Enter index of second column: ";
    cin >> col2;

    vector<double> X, Y;
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        vector<double> row;
        while (getline(ss, val, ',')) row.push_back(stod(val));
        if (row.size() > max(col1, col2)) {
            X.push_back(row[col1]);
            Y.push_back(row[col2]);
        }
    }
    file.close();

    if (X.empty() || Y.empty()) {
        cerr << "No data found for selected columns!" << endl;
        return 1;
    }

    double r = pearsonCorrelation(X, Y);

    cout << "\nPearson correlation coefficient (r) = " << r << endl;

    if (fabs(r) < 1e-6) 
        cout << "Correlation is approximately zero (no linear relationship)." << endl;
    else if (r > 0)
        cout << "Positive correlation." << endl;
    else
        cout << "Negative correlation." << endl;

    return 0;
}
