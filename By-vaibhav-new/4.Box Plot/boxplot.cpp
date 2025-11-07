#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <climits>
#include <iomanip>
#include <cmath>
#include <algorithm>
using namespace std;

vector<double> read_csv(string &filename) {
    ifstream file(filename);
    vector<double> data;
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return data;
    }

    string line, header;
    getline(file, header); // skip header

    while (getline(file, line)) {
        if (!line.empty())
            data.push_back(stod(line));
    }

    file.close();
    return data;
}

double median(vector<double> &data) {
    int n = data.size();
    if (n == 0) return 0;
    if (n % 2 == 0)
        return (data[n / 2 - 1] + data[n / 2]) / 2.0;
    else
        return data[n / 2];
}

int main() {
    string filename = "data.csv";
    vector<double> data = read_csv(filename);

    if (data.empty()) {
        cout << "No data found!" << endl;
        return 0;
    }

    sort(data.begin(), data.end());
    int n = data.size();

    double Q2 = median(data);

    vector<double> lower_half, upper_half;
    int mid = n / 2;

    if (n % 2 == 0) {
        lower_half = vector<double>(data.begin(), data.begin() + mid);
        upper_half = vector<double>(data.begin() + mid, data.end());
    } else {
        lower_half = vector<double>(data.begin(), data.begin() + mid);
        upper_half = vector<double>(data.begin() + mid + 1, data.end());
    }

    double Q1 = median(lower_half);
    double Q3 = median(upper_half);
    double IQR = Q3 - Q1;

    double lower_bound = Q1 - 1.5 * IQR;
    double upper_bound = Q3 + 1.5 * IQR;

    double minVal = data[0], maxVal = data[n - 1];

    cout << "\n--- Outliers ---\n";
    for (double val : data) {
        if (val < lower_bound || val > upper_bound)
            cout << val << " ";
    }
    cout << "\n";

    // Update min/max excluding outliers
    for (double val : data) {
        if (val >= lower_bound) {
            minVal = val;
            break;
        }
    }
    for (int i = n - 1; i >= 0; i--) {
        if (data[i] <= upper_bound) {
            maxVal = data[i];
            break;
        }
    }

    cout << fixed << setprecision(2);
    cout << "\nFive Number Summary:\n";
    cout << "Min: " << minVal << endl;
    cout << "Q1 : " << Q1 << endl;
    cout << "Q2 : " << Q2 << endl;
    cout << "Q3 : " << Q3 << endl;
    cout << "Max: " << maxVal << endl;

    return 0;
}
