#include<bits/stdc++.h>
using namespace std;

int main() {
    ifstream file("data.csv");
    string line;
    vector<vector<double>> data;

    // Read CSV data
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        vector<double> row;
        while (getline(ss, val, ',')) {
            row.push_back(stod(val));
        }
        if (!row.empty()) data.push_back(row);
    }
    file.close();

    int n = data.size();
    int m = data[0].size();

    // Compute correlation for each pair of columns
    for (int i = 0; i < m; i++) {
        for (int j = i + 1; j < m; j++) {
            double sumx = 0, sumy = 0, sumxy = 0, sumx2 = 0, sumy2 = 0;

            for (int k = 0; k < n; k++) {
                double x = data[k][i];
                double y = data[k][j];
                sumx += x;
                sumy += y;
                sumxy += x * y;
                sumx2 += x * x;
                sumy2 += y * y;
            }

            double num = (n * sumxy) - (sumx * sumy);
            double den = sqrt((n * sumx2 - sumx * sumx) * (n * sumy2 - sumy * sumy));

            double corr = (den != 0) ? num / den : 0;

            cout << "Correlation between column " << i + 1
                 << " and column " << j + 1 << " = " << corr << endl;
        }
    }

    return 0;
}
