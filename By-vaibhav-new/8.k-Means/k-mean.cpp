#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

double euclideanDistance(const vector<double> &a, const vector<double> &b) {
    double sum = 0.0;
    for (int i = 0; i < a.size(); i++)
        sum += pow(a[i] - b[i], 2);
    return sqrt(sum);
}

int main() {
    ifstream file("data.csv");
    string header;
    getline(file, header); 

    vector<vector<double>> data;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<double> row;
        while (getline(ss, value, ',')) {
            if (!value.empty())
                row.push_back(stod(value));
        }
        if (!row.empty())
            data.push_back(row);
    }
    file.close();

    int n = data.size();
    int dim = data[0].size();
    cout << "Data loaded: " << n << " points, " << dim << " dimensions.\n";

    int k;
    cout << "Enter number of clusters: ";
    cin >> k;

    vector<vector<double>> centroids(k, vector<double>(dim));
    vector<vector<double>> centroidSum(k, vector<double>(dim));
    vector<int> count(k, 0);
    vector<int> label(n, -1);

    for (int i = 0; i < k; i++)
        centroids[i] = data[i];

    bool changed = true;
    int step = 0;
    while (changed && step < 100) {
        changed = false;
        step++;
        cout << "\n--- Step " << step << " ---\n";

        // Reset sums and counts
        for (int i = 0; i < k; i++) {
            count[i] = 0;
            fill(centroidSum[i].begin(), centroidSum[i].end(), 0.0);
        }

        // Assign points to nearest centroid
        for (int i = 0; i < n; i++) {
            double minDist = INT_MAX;
            int best = -1;

            for (int c = 0; c < k; c++) {
                double dist = euclideanDistance(data[i], centroids[c]);
                if (dist < minDist) {
                    minDist = dist;
                    best = c;
                }
            }

            if (label[i] != best)
                changed = true;

            label[i] = best;

            // Add point to centroid sum
            for (int d = 0; d < dim; d++)
                centroidSum[best][d] += data[i][d];

            count[best]++;
        }

        // Update centroids
        for (int c = 0; c < k; c++) {
            if (count[c] > 0) {
                for (int d = 0; d < dim; d++)
                    centroids[c][d] = centroidSum[c][d] / count[c];
            }
        }

        // --- Print current centroids and cluster points ---
        for (int c = 0; c < k; c++) {
            cout << "Cluster " << c + 1 << " centroid: ";
            for (int d = 0; d < dim; d++)
                cout << centroids[c][d] << " ";
            cout << "\nPoints: ";
            for (int i = 0; i < n; i++)
                if (label[i] == c)
                    cout << "P" << i + 1 << " ";
            cout << "\n";
        }
    }

    cout << "\nK-means clustering completed in " << step << " steps!\n";

    return 0;
}
