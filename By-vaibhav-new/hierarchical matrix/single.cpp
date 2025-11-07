#include <bits/stdc++.h>
using namespace std;

int main() {
    ifstream file("data.csv");
    if (!file.is_open()) {
        cout << "Error: Cannot open data.csv\n";
        return 0;
    }

    string line;
    getline(file, line);
    stringstream ss(line);
    string val;
    vector<string> labels;
    getline(ss, val, ','); // skip top-left "0" cell
    while (getline(ss, val, ',')) labels.push_back(val);

    int n = labels.size();
    vector<vector<double>> dist(n, vector<double>(n, 0));

    // Read distance matrix
    for (int i = 0; i < n; i++) {
        getline(file, line);
        stringstream row(line);
        string rowLabel;
        getline(row, rowLabel, ','); // read row label (a,b,c,...)
        for (int j = 0; j < n; j++) {
            getline(row, val, ',');
            dist[i][j] = stod(val);
        }
    }
    file.close();

    // Initial clusters
    vector<vector<int>> clusters;
    for (int i = 0; i < n; i++) clusters.push_back({i});

    cout << "Initial clusters: ";
    for (auto &l : labels) cout << l << " ";
    cout << "\n\n";

    while (clusters.size() > 1) {
        double minDist = 1e9;
        int idxA = -1, idxB = -1;

        // Find closest pair of clusters (Single linkage)
        for (int i = 0; i < clusters.size(); i++) {
            for (int j = i + 1; j < clusters.size(); j++) {
                double d = 1e9;
                for (int p1 : clusters[i])
                    for (int p2 : clusters[j])
                        d = min(d, dist[p1][p2]);
                if (d < minDist) {
                    minDist = d;
                    idxA = i;
                    idxB = j;
                }
            }
        }

        // Print merge info
        cout << "Merging cluster { ";
        for (int p : clusters[idxA]) cout << labels[p] << " ";
        cout << "} and { ";
        for (int p : clusters[idxB]) cout << labels[p] << " ";
        cout << "} at distance = " << minDist << "\n";

        // Merge clusters
        vector<int> merged = clusters[idxA];
        merged.insert(merged.end(), clusters[idxB].begin(), clusters[idxB].end());

        if (idxA < idxB) {
            clusters.erase(clusters.begin() + idxB);
            clusters.erase(clusters.begin() + idxA);
        } else {
            clusters.erase(clusters.begin() + idxA);
            clusters.erase(clusters.begin() + idxB);
        }
        clusters.push_back(merged);
    }

    cout << "\nFinal Cluster: { ";
    for (int p : clusters[0]) cout << labels[p] << " ";
    cout << "}\n";

    return 0;
}
