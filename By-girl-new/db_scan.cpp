#include <bits/stdc++.h>
using namespace std;

struct Point {
    string id;
    double x, y;
    int cluster = 0;
    bool core = false;
};

double dist(Point &a, Point &b) {
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

vector<int> neighbors(int i, vector<Point> &points, double eps) {
    vector<int> neighbors_keeper;
    for (int j = 0; j < points.size(); j++)
        if (dist(points[i], points[j]) <= eps)
            neighbors_keeper.push_back(j);
    return neighbors_keeper;
}

void dbscan(vector<Point> &points, double eps, int minPoints) {
    int cluster_id = 0;

    for (int i = 0; i < points.size(); i++) {
        if (points[i].cluster != 0) continue;

        vector<int> neigh = neighbors(i, points, eps);
        if (neigh.size() < minPoints) continue;

        cluster_id++;
        points[i].cluster = cluster_id;
        points[i].core = true;

        queue<int> q;
        for (int idx : neigh) q.push(idx);

        while (!q.empty()) {
            int current = q.front(); q.pop();

            if (points[current].cluster == 0) {
                points[current].cluster = cluster_id;
                vector<int> neigh2 = neighbors(current, points, eps);
                if (neigh2.size() >= minPoints) {
                    points[current].core = true;
                    for (int nb : neigh2)
                        if (points[nb].cluster == 0)
                            q.push(nb);
                }
            }
        }
    }
}

int main() {
    string filename;
    cout << "Enter CSV file name: ";
    cin >> filename;

    ifstream file(filename);
    if(!file.is_open()) {
        cout << "File not found!" << endl;
        return 0;
    }

    string line;
    getline(file, line); // skip header
    vector<Point> points;

    while (getline(file, line)) {
        stringstream ss(line);
        string id, xs, ys;
        getline(ss, id, ',');
        getline(ss, xs, ',');
        getline(ss, ys, ',');
        points.push_back({id, stod(xs), stod(ys)});
    }
    file.close();

    // Print distance matrix
    cout << "\nDistance Matrix:\n\t";
    for (auto &p : points) cout << p.id << "\t";
    cout << endl;

    for (int i = 0; i < points.size(); i++) {
        cout << points[i].id << "\t";
        for (int j = 0; j < points.size(); j++) {
            cout << fixed << setprecision(2) << dist(points[i], points[j]) << "\t";
        }
        cout << endl;
    }

    double eps;
    int minPoints;
    cout << "\nEnter eps: ";
    cin >> eps;
    cout << "Enter min_pts: ";
    cin >> minPoints;

    dbscan(points, eps, minPoints);

    vector<string> core, border, noise;
    for (auto &p : points) {
        if (p.core) core.push_back(p.id);
        else if (p.cluster != 0) border.push_back(p.id);
        else noise.push_back(p.id);
    }

    cout << "\nResults-*DBSCAN*\n";
    cout << "Core Points: ";
    for (auto &s : core) cout << s << " ";
    cout << "\nBorder Points: ";
    for (auto &s : border) cout << s << " ";
    cout << "\nNoise Points: ";
    for (auto &s : noise) cout << s << " ";
    cout << "\n\n";

    map<int, vector<string>> clusters;
    for (auto &p : points)
        if (p.cluster != 0)
            clusters[p.cluster].push_back(p.id);

    for (auto &c : clusters) {
        cout << "Cluster " << c.first << ": ";
        for (auto &id : c.second) cout << id << " ";
        cout << endl;
    }

    return 0;
}
