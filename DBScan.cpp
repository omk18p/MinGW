#include <bits/stdc++.h>
using namespace std;

// -------- Function to calculate Euclidean distance between two points --------
double distanceCalc(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// -------- Read CSV file --------
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
            try {
                double num = stod(value);
                row.push_back(num);
            } catch (...) {
                if (firstValue) {
                    name = value;
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

// -------- Find all neighbors within epsilon distance --------
vector<int> regionQuery(const vector<vector<double>> &data, int pointIdx, double eps) {
    vector<int> neighbors;
    for (int i = 0; i < data.size(); i++) {
        if (distanceCalc(data[pointIdx], data[i]) <= eps)
            neighbors.push_back(i);
    }
    return neighbors;
}

// -------- Expand cluster recursively --------
void expandCluster(vector<vector<double>> &data, vector<int> &labels, int pointIdx,
                   int clusterId, double eps, int minPts) {
    vector<int> neighbors = regionQuery(data, pointIdx, eps);
    labels[pointIdx] = clusterId;

    for (int i = 0; i < neighbors.size(); i++) {
        int nIdx = neighbors[i];
        if (labels[nIdx] == -1)
            labels[nIdx] = clusterId;  // previously marked as noise
        else if (labels[nIdx] != 0)
            continue; // already assigned to a cluster

        labels[nIdx] = clusterId;
        vector<int> newNeighbors = regionQuery(data, nIdx, eps);
        if (newNeighbors.size() >= minPts) {
            neighbors.insert(neighbors.end(), newNeighbors.begin(), newNeighbors.end());
        }
    }
}

// -------- DBSCAN algorithm --------
void dbscan(vector<vector<double>> &data, vector<string> &names, double eps, int minPts) {
    int n = data.size();
    vector<int> labels(n, 0); // 0 = unvisited, -1 = noise, >0 = cluster ID
    int clusterId = 0;

    cout << "\n--- Starting DBSCAN ---\n";
    cout << "Epsilon (eps): " << eps << ", MinPts: " << minPts << endl;

    for (int i = 0; i < n; i++) {
        if (labels[i] != 0) continue; // already visited

        vector<int> neighbors = regionQuery(data, i, eps);

        cout << "\nPoint " << (names[i].empty() ? to_string(i + 1) : names[i])
             << " has " << neighbors.size() << " neighbors.\n";

        if (neighbors.size() < minPts) {
            labels[i] = -1; // mark as noise
            cout << "Marked as Noise\n";
        } else {
            clusterId++;
            cout << "Forming Cluster " << clusterId << endl;
            expandCluster(data, labels, i, clusterId, eps, minPts);
        }
    }

    // -------- Print Final Clusters --------
    cout << "\n--- Final Clusters ---\n";
    map<int, vector<string>> clusters;
    for (int i = 0; i < n; i++) {
        string pointName = names[i].empty() ? "Point " + to_string(i + 1) : names[i];
        clusters[labels[i]].push_back(pointName);
    }

    for (auto &c : clusters) {
        if (c.first == -1)
            cout << "Noise: ";
        else
            cout << "Cluster " << c.first << ": ";
        for (auto &p : c.second)
            cout << p << " ";
        cout << endl;
    }
}

// -------- Main Function --------
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

    double eps;
    int minPts;
    cout << "Enter Epsilon (eps): ";
    cin >> eps;
    cout << "Enter Minimum Points (MinPts): ";
    cin >> minPts;

    dbscan(data, names, eps, minPts);

    return 0;
}
