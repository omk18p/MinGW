#include <bits/stdc++.h>
using namespace std;

// Euclidean distance between two points
double euclideanDistance(pair<double, double> a, pair<double, double> b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

int main() {
    ifstream file("data.csv");
    vector<pair<double, double>> points;
    double x, y;
    char comma;
    while(file >> x >> comma >> y)
        points.push_back({x, y});
    file.close();

    int n = points.size();

    // Initialize clusters: each cluster stores indices of points
    vector<vector<int>> clusters;
    for(int i = 0; i < n; i++)
        clusters.push_back({i});

    // Print initial clusters
    cout << "Initial clusters:\n";
    for(int i = 0; i < n; i++) 
        cout << "P" << i+1 << " ";
    cout << "\n\n";

    while(clusters.size() > 1) {
        double minDist = INT_MAX;
        int idxA = -1, idxB = -1;

        // Find closest pair of clusters (single linkage)
        for(int i = 0; i < clusters.size(); i++) {
            for(int j = i+1; j < clusters.size(); j++) {
                double d = 1e9;
                for(int p1 : clusters[i])
                    for(int p2 : clusters[j])
                        d = min(d, euclideanDistance(points[p1], points[p2]));
                if(d < minDist) {
                    minDist = d;
                    idxA = i;
                    idxB = j;
                }
            }
        }

        // Merge clusters
        vector<int> merged = clusters[idxA];
        merged.insert(merged.end(), clusters[idxB].begin(), clusters[idxB].end());

        // Print merge info
        cout << "Merging clusters: ";
        for(int p : clusters[idxA]) cout << "P" << p+1 << " ";
        cout << "and ";
        for(int p : clusters[idxB]) cout << "P" << p+1 << " ";
        cout << "(distance = " << minDist << ")\n";

        // Remove old clusters and add merged
        if(idxA < idxB) { 
            clusters.erase(clusters.begin() + idxB); 
            clusters.erase(clusters.begin() + idxA); 
        }
        else { clusters.erase(clusters.begin() + idxA); 
            clusters.erase(clusters.begin() + idxB); 
        }
        clusters.push_back(merged);
    }

    // Print final cluster
    cout << "\nFinal Cluster: ";
    for(int p : clusters[0]) cout << "P" << p+1 << " ";
    cout << "\n";

    return 0;
}
