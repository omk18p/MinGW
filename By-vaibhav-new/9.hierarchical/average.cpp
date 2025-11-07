#include <bits/stdc++.h>
using namespace std;

// Euclidean distance between two points
double euclideanDistance(pair<double,double> a, pair<double,double> b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

int main() {
    ifstream file("data.csv");
    vector<pair<double,double>> points;
    double x, y;
    char comma;
    while(file >> x >> comma >> y)
        points.push_back({x,y});
    file.close();

    int n = points.size();

    // Each cluster stores the indices of points
    vector<vector<int>> clusters;
    vector<string> clusterNames;
    for(int i = 0; i < n; i++) {
        clusters.push_back({i});
        clusterNames.push_back("P" + to_string(i+1));
    }

    cout << "Initial clusters:\n";
    for(auto &name : clusterNames) cout << name << " ";
    cout << "\n\n";

    while(clusters.size() > 1) {
        double minDist = 1e9;
        int idxA=-1, idxB=-1;

        // Find closest pair of clusters (average linkage)
        for(int i = 0; i < clusters.size(); i++) {
            for(int j = i+1; j < clusters.size(); j++) {
                double sumDist = 0;
                int count = 0;
                for(int p1 : clusters[i]) {
                    for(int p2 : clusters[j]) {
                        sumDist += euclideanDistance(points[p1], points[p2]);
                        count++;
                    }
                }
                double avgDist = sumDist / count; // average distance
                if(avgDist < minDist) {
                    minDist = avgDist;
                    idxA = i;
                    idxB = j;
                }
            }
        }

        // Merge clusters
        vector<int> merged = clusters[idxA];
        merged.insert(merged.end(), clusters[idxB].begin(), clusters[idxB].end());
        string mergedName = "(" + clusterNames[idxA] + "," + clusterNames[idxB] + ")";

        // Print merge info
        cout << "Merging " << clusterNames[idxA] << " and " << clusterNames[idxB]
             << " => " << mergedName << " (distance = " << minDist << ")\n";

        // Remove old clusters and add merged
        if(idxA < idxB) {
            clusters.erase(clusters.begin() + idxB);
            clusters.erase(clusters.begin() + idxA);
            clusterNames.erase(clusterNames.begin() + idxB);
            clusterNames.erase(clusterNames.begin() + idxA);
        } else {
            clusters.erase(clusters.begin() + idxA);
            clusters.erase(clusters.begin() + idxB);
            clusterNames.erase(clusterNames.begin() + idxA);
            clusterNames.erase(clusterNames.begin() + idxB);
        }

        clusters.push_back(merged);
        clusterNames.push_back(mergedName);
    }

    cout << "\nFinal Cluster: " << clusterNames[0] << "\n";
    return 0;
}
