#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

int main() {
    ifstream file("data.csv");
    vector<double> dataX, dataY;
    string header;
    getline(file, header); // skip header

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string valX, valY;
        if (!getline(ss, valX, ',') || !getline(ss, valY, ',')) continue;
        dataX.push_back(stod(valX));
        dataY.push_back(stod(valY));
    }
    file.close();

    int n = dataX.size();
    if (n == 0) {
        cout << "No data found!" << endl;
        return 0;
    }

    // Step 1: Calculate sums
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    for (int i = 0; i < n; i++) {
        sumX += dataX[i];
        sumY += dataY[i];
        sumXY += dataX[i] * dataY[i];
        sumX2 += dataX[i] * dataX[i];
    }

    cout << fixed << setprecision(4);
    cout << "\nStep 1: Calculate sums\n";
    cout << "Sum of X: " << sumX << endl;
    cout << "Sum of Y: " << sumY << endl;
    cout << "Sum of X*Y: " << sumXY << endl;
    cout << "Sum of X^2: " << sumX2 << endl;

    // Step 2: Calculate slope (m)
    double slope = ((n * sumXY) - (sumX * sumY)) / ((n * sumX2) - (sumX * sumX));
    cout << "\nStep 2: Calculate slope (m)\n";
    cout << "Slope (m) = " << slope << endl;

    // Step 3: Calculate intercept (b)
    double intercept = (sumY - slope * sumX) / n;
    cout << "\nStep 3: Calculate intercept (b)\n";
    cout << "Intercept (b) = " << intercept << endl;

    // Step 4: Make prediction
    double xInput;
    cout << "\nStep 4: Predict Y for a given X\n";
    cout << "Enter X value: ";
    cin >> xInput;
    double yPred = intercept + slope * xInput;
    cout << "Predicted Y = " << yPred << endl;

    return 0;
}
