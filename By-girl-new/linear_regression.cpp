// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <sstream>
// #include <string>
// #include <iomanip>
// using namespace std;
// int main() {
//     string filename;
//     cout << "Enter CSV file name: ";
//     cin >> filename;
//     vector<double> xList, yList;
//     ifstream file(filename);
//     if (!file.is_open()) {
//         cout << "Error reading file!" << endl;
//         return 1;
//     }

//     string line;
//     while (getline(file, line)) {
//         stringstream ss(line);
//         string xStr, yStr;
//         if (getline(ss, xStr, ',') && getline(ss, yStr, ',')) {
//             try {
//                 double x = stod(xStr);
//                 double y = stod(yStr);
//                 xList.push_back(x);
//                 yList.push_back(y);
//             } catch (...) {
//                 // Ignore invalid lines
//             }
//         }
//     }
//     file.close();

//     int n = xList.size();
//     if (n == 0) {
//         cout << "No valid data points!" << endl;
//         return 1;
//     }

//     // Calculate mean
//     double sumX = 0, sumY = 0;
//     for (int i = 0; i < n; i++) {
//         sumX += xList[i];
//         sumY += yList[i];
//     }
//     double xBar = sumX / n;
//     double yBar = sumY / n;

  
//     vector<double> xDiff(n), yDiff(n), prodDiff(n), xDiff2(n);
//     double sumProdDiff = 0, sumXDiff2 = 0;

//     for (int i = 0; i < n; i++) {
//         xDiff[i] = xList[i] - xBar;
//         yDiff[i] = yList[i] - yBar;
//         prodDiff[i] = xDiff[i] * yDiff[i];
//         xDiff2[i] = xDiff[i] * xDiff[i];
//         sumProdDiff += prodDiff[i];
//         sumXDiff2 += xDiff2[i];
//     }

//     double m = sumProdDiff / sumXDiff2;
//     double c = yBar - m * xBar;
//     cout << "\nLinear Regression Equation: y = " << m << " * x + " << c << endl;

//     double xNew;
//     cout << "\nEnter x to predict y: ";
//     cin >> xNew;
//     double yPred = m * xNew + c;
//     cout << "Predicted y = " << yPred << endl;
//     return 0;
// }
#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

int main() {
    int n;
    cout << "Enter number of data points: ";
    cin >> n;

    if (n <= 0) {
        cout << "Number of points must be positive!" << endl;
        return 1;
    }

    vector<double> xList(n), yList(n);
    cout << "Enter the data points (x y) separated by space:\n";
    for (int i = 0; i < n; i++) {
        cout << "Point " << i + 1 << ": ";
        cin >> xList[i] >> yList[i];
    }

    // Calculate mean
    double sumX = 0, sumY = 0;
    for (int i = 0; i < n; i++) {
        sumX += xList[i];
        sumY += yList[i];
    }
    double xBar = sumX / n;
    double yBar = sumY / n;

    // Deviation calculations
    vector<double> xDiff(n), yDiff(n), prodDiff(n), xDiff2(n);
    double sumProdDiff = 0, sumXDiff2 = 0;

    for (int i = 0; i < n; i++) {
        xDiff[i] = xList[i] - xBar;
        yDiff[i] = yList[i] - yBar;
        prodDiff[i] = xDiff[i] * yDiff[i];
        xDiff2[i] = xDiff[i] * xDiff[i];
        sumProdDiff += prodDiff[i];
        sumXDiff2 += xDiff2[i];
    }

    double m = sumProdDiff / sumXDiff2;
    double c = yBar - m * xBar;

    cout << "\nLinear Regression Equation: y = " << m << " * x + " << c << endl;

    double xNew;
    cout << "\nEnter x to predict y: ";
    cin >> xNew;
    double yPred = m * xNew + c;
    cout << "Predicted y = " << yPred << endl;

    return 0;
}
