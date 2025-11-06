#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

// Function to calculate Euclidean distance between two points
double distanceCalc(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// Function to read CSV file (works for both numeric and labeled data)
vector<vector<double>> readCSV(string filename, vector<string> &names) {
    vector<vector<double>> data;
    ifstream file(filename);
    string line;
    bool firstLine = true;

    while (getline(file, line)) {
        vector<double> row;
        string value;
        stringstream ss(line);
        bool firstValue = true;
        string name = "";

        // Skip header row
        if (firstLine) {
            firstLine = false;
            continue;
        }

        while (getline(ss, value, ',')) {
            // Try converting to number
            try {
                double num = stod(value);
                row.push_back(num);
            } catch (...) {
                if (firstValue) {
                    name = value; // store row name (like A, B, C)
                    firstValue = false;
                }
            }
        }

        if (!row.empty()) {
            data.push_back(row);
            names.push_back(name);
        }
    }
    return data;
}

// Function to perform K-Means clustering
void kMeans(vector<vector<double>> &data, int k, const vector<string> &names, int max_iter = 100) {
    int n = data.size();
    int m = data[0].size();
    vector<vector<double>> centroids(k, vector<double>(m, 0));
    vector<int> cluster(n, -1);

    // Initialize centroids as first k points
    for (int i = 0; i < k; i++)
        centroids[i] = data[i];

    cout << "\nInitial Centroids:\n";
    for (int i = 0; i < k; i++) {
        cout << "Centroid " << i + 1 << ": ";
        for (double val : centroids[i]) cout << fixed << setprecision(2) << val << " ";
        cout << endl;
    }

    for (int iter = 1; iter <= max_iter; iter++) {
        bool changed = false;

        cout << "\n--- Iteration " << iter << " ---\n";

        // Step 1: Assign clusters
        for (int i = 0; i < n; i++) {
            double minDist = 1e9;
            int best = 0;
            for (int j = 0; j < k; j++) {
                double d = distanceCalc(data[i], centroids[j]);
                if (d < minDist) {
                    minDist = d;
                    best = j;
                }
            }
            if (cluster[i] != best) {
                cluster[i] = best;
                changed = true;
            }
            cout << "Point " << (names[i].empty() ? to_string(i + 1) : names[i])
                 << " -> Cluster " << best + 1 << " (Distance: " << fixed << setprecision(2) << minDist << ")\n";
        }

        // Step 2: Update centroids
        vector<vector<double>> newCentroids(k, vector<double>(m, 0));
        vector<int> count(k, 0);
        for (int i = 0; i < n; i++) {
            int c = cluster[i];
            for (int j = 0; j < m; j++)
                newCentroids[c][j] += data[i][j];
            count[c]++;
        }

        for (int j = 0; j < k; j++) {
            if (count[j] > 0)
                for (int p = 0; p < m; p++)
                    newCentroids[j][p] /= count[j];
        }

        centroids = newCentroids;

        cout << "\nUpdated Centroids:\n";
        for (int i = 0; i < k; i++) {
            cout << "Centroid " << i + 1 << ": ";
            for (double val : centroids[i]) cout << fixed << setprecision(2) << val << " ";
            cout << endl;
        }

        if (!changed) {
            cout << "\nCentroids stabilized - stopping iterations.\n";
            break;
        }
    }

    // Final Clusters
    cout << "\nFinal Cluster Assignments:\n";
    for (int i = 0; i < n; i++) {
        cout << (names[i].empty() ? "Data Point " + to_string(i + 1) : names[i])
             << " - Cluster " << cluster[i] + 1 << endl;
    }
}

int main() {
    string filename;
    cout << "Enter CSV filename (with .csv): ";
    cin >> filename;

    vector<string> names;
    vector<vector<double>> data = readCSV(filename, names);

    if (data.empty()) {
        cout << "Error: CSV file empty or invalid!\n";
        return 0;
    }

    int k;
    cout << "Enter number of clusters: ";
    cin >> k;

    kMeans(data, k, names);

    return 0;
}
