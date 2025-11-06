#include <bits/stdc++.h>
using namespace std;

vector<string> splitCSV(const string &line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

double distCalc(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += pow(a[i] - b[i], 2);
    return sqrt(sum);
}

void kMeans(const vector<vector<double>> &data, int k, int maxIter,
            vector<vector<double>> &centroids, vector<int> &labels) {
    int n = data.size();
    int dim = data[0].size();
    labels.assign(n, -1);

    for (int iter = 1; iter <= maxIter; ++iter) {
        bool changed = false;
        cout << "\nIteration " << iter << ":\n";

        // Step 1: Assign each data point to nearest centroid
        for (int i = 0; i < n; ++i) {
            double minDist = DBL_MAX;
            int clusterIdx = -1;
            for (int j = 0; j < k; ++j) {
                double d = distCalc(data[i], centroids[j]);
                if (d < minDist) {
                    minDist = d;
                    clusterIdx = j;
                }
            }
            if (labels[i] != clusterIdx) {
                labels[i] = clusterIdx;
                changed = true;
            }
        }

        // Step 2: Show which points belong to which cluster
        map<int, vector<int>> clusters;
        for (int i = 0; i < n; ++i)
            clusters[labels[i]].push_back(i);

        for (int j = 0; j < k; ++j) {
            cout << "Cluster " << j + 1 << ": ";
            for (int idx : clusters[j]) cout << idx + 1 << " ";
            cout << endl;
        }

        // Step 3: Compute new centroids
        vector<vector<double>> newCentroids(k, vector<double>(dim, 0.0));
        vector<int> count(k, 0);
        for (int i = 0; i < n; ++i) {
            int c = labels[i];
            count[c]++;
            for (int d = 0; d < dim; ++d)
                newCentroids[c][d] += data[i][d];
        }
        for (int j = 0; j < k; ++j)
            if (count[j] > 0)
                for (int d = 0; d < dim; ++d)
                    newCentroids[j][d] /= count[j];

        cout << "Updated Centroids:\n";
        for (int j = 0; j < k; ++j) {
            cout << "Centroid " << j + 1 << ": ";
            for (double val : newCentroids[j]) cout << val << " ";
            cout << endl;
        }

        // Check convergence
        if (!changed) {
            cout << "\nCentroids stabilized — stopping iterations.\n";
            break;
        }
        centroids = newCentroids;
    }
}

int main() {
    string fileName;
    cout << "Enter CSV file name: ";
    cin >> fileName;

    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: Could not open file.\n";
        return 1;
    }

    string line;
    getline(file, line); // skip header

    vector<string> pointNames;
    vector<vector<double>> data;

    while (getline(file, line)) {
        vector<string> tokens = splitCSV(line);
        if (tokens.empty()) continue;

        pointNames.push_back(tokens[0]);
        vector<double> row;
        for (size_t i = 1; i < tokens.size(); ++i) {
            try { row.push_back(stod(tokens[i])); } catch (...) {}
        }
        if (!row.empty()) data.push_back(row);
    }
    file.close();

    if (data.empty()) {
        cout << "Error: No data found.\n";
        return 1;
    }

    int k;
    cout << "Enter number of clusters (k): ";
    cin >> k;

    // Random centroid initialization
    srand(time(0));
    vector<vector<double>> centroids;
    set<int> chosen;
    while ((int)centroids.size() < k) {
        int idx = rand() % data.size();
        if (!chosen.count(idx)) {
            centroids.push_back(data[idx]);
            chosen.insert(idx);
        }
    }

    cout << "\nInitial Random Centroids:\n";
    for (int i = 0; i < k; ++i) {
        cout << "Centroid " << i + 1 << ": ";
        for (double val : centroids[i]) cout << val << " ";
        cout << endl;
    }

    vector<int> labels;
    kMeans(data, k, 10, centroids, labels);

    cout << "\nFinal Centroids:\n";
    for (int i = 0; i < k; ++i) {
        cout << "Cluster " << i + 1 << ": ";
        for (double val : centroids[i]) cout << val << " ";
        cout << endl;
    }

    cout << "\nCluster Assignments:\n";
    for (size_t i = 0; i < labels.size(); ++i)
        cout << pointNames[i] << "- Cluster " << labels[i] + 1 << endl;

    return 0;
}
