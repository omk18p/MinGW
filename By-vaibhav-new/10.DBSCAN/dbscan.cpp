#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

struct Point {
    vector<double> coords; // store all dimensions
    int cluster;
};


double distance(const Point &a, const Point &b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.coords.size(); i++) {
        sum += pow(a.coords[i] - b.coords[i], 2);
    }
    return sqrt(sum);
}


vector<int> getNeighbors(const vector<Point> &points, int idx, double eps) {
    vector<int> neighbors;
    for (int i = 0; i < points.size(); i++) {
        if (distance(points[idx], points[i]) <= eps)
            neighbors.push_back(i);
    }
    return neighbors;
}

void expandCluster(vector<Point> &points, int idx, vector<int> neighbors,
                   int clusterId, double eps, int minPts) {
    points[idx].cluster = clusterId;

    for (int i = 0; i < neighbors.size(); i++) {
        int nIdx = neighbors[i];

        if (points[nIdx].cluster == 0) { // unvisited
            points[nIdx].cluster = clusterId;
            vector<int> newNeighbors = getNeighbors(points, nIdx, eps);

            if (newNeighbors.size() >= minPts)
                neighbors.insert(neighbors.end(), newNeighbors.begin(), newNeighbors.end());
        }

        if (points[nIdx].cluster == -1) // was noise
            points[nIdx].cluster = clusterId;
    }
}

int main() {
   ifstream file("data.csv");
vector<Point> points;
string line;

while (getline(file, line)) {
    stringstream ss(line);
    string value;
    Point p;
    while (getline(ss, value, ',')) {
        p.coords.push_back(stod(value)); // add each dimension
    }
    p.cluster = 0;
    points.push_back(p);
}
    double eps;
    int minPts;
    cout << "Enter eps and minPts: ";
    cin >> eps >> minPts;

    int clusterId = 0;

    for (int i = 0; i < points.size(); i++) {
        if (points[i].cluster != 0) continue; 
        
        vector<int> neighbors = getNeighbors(points, i, eps);

        if (neighbors.size() < minPts) {
            points[i].cluster = -1; // mark as noise
        } else {
            clusterId++;
            expandCluster(points, i, neighbors, clusterId, eps, minPts);
        }
    }

    cout << "\n=== DBSCAN Clustering Result ===\n";
for (int i = 0; i < points.size(); i++) {
    cout << "(";
    for (int j = 0; j < points[i].coords.size(); j++) {
        cout << points[i].coords[j];
        if (j != points[i].coords.size() - 1)
            cout << ", ";
    }
    cout << ") -> ";
    
    if (points[i].cluster == -1)
        cout << "Noise\n";
    else
        cout << "Cluster " << points[i].cluster << "\n";
}

    return 0;
}
