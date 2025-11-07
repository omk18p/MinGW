// #include <bits/stdc++.h>
// using namespace std;


// vector<string> splitCSV(const string &line) {
//     vector<string> tokens;
//     stringstream ss(line);
//     string token;
//     while (getline(ss, token, ',')) {
//         token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
//         tokens.push_back(token);
//     }
//     return tokens;
// }


// double distance(const vector<double> &a, const vector<double> &b) {
//     double sum = 0;
//     for (size_t i = 0; i < a.size(); ++i)
//         sum += (a[i] - b[i]) * (a[i] - b[i]);
//     return sqrt(sum);
// }


// void kMeans(const vector<vector<double>> &data, int k, int maxIter,
//             vector<vector<double>> &centroids, vector<int> &labels) {
//     int n = data.size();
//     labels.assign(n, -1);

//     for (int iter = 0; iter < maxIter; ++iter) {
//         bool changed = false;

       
//         for (int i = 0; i < n; ++i) {
//             double minDist = 1e18;
//             int bestCluster = -1;
//             for (int j = 0; j < k; ++j) {
//                 double dist = distance(data[i], centroids[j]);
//                 if (dist < minDist) {
//                     minDist = dist;
//                     bestCluster = j;
//                 }
//             }
//             if (labels[i] != bestCluster) {
//                 labels[i] = bestCluster;
//                 changed = true;
//             }
//         }

      
//         vector<vector<double>> newCentroids(k, vector<double>(data[0].size(), 0.0));
//         vector<int> counts(k, 0);
//         for (int i = 0; i < n; ++i) {
//             int c = labels[i];
//             counts[c]++;
//             for (size_t d = 0; d < data[0].size(); ++d)
//                 newCentroids[c][d] += data[i][d];
//         }
//         for (int j = 0; j < k; ++j)
//             if (counts[j] > 0)
//                 for (size_t d = 0; d < data[0].size(); ++d)
//                     newCentroids[j][d] /= counts[j];

//         centroids = newCentroids;
//         if (!changed) break;
//     }
// }

// int main() {
//     string file_nm;
//     cout << "Enter CSV file_nm: ";
//     cin >> file_nm;

//     ifstream file(file_nm);
    
//     string line;
//     getline(file, line);
//     vector<vector<double>> data;
//     vector<string> pts_nm;

//     while (getline(file, line)) {
//         vector<string> tokens = splitCSV(line);
//         if (tokens.empty()) continue;
//         pts_nm.push_back(tokens[0]);
//         vector<double> point;
//         for (size_t i = 1; i < tokens.size(); ++i) {
//             try {
//                 point.push_back(stod(tokens[i]));
//             } catch (...) { } 
//         }
//         if (!point.empty()) data.push_back(point);
//     }
//     file.close();


//     int k;
//     cout << "Enter k  clusters: ";
//     cin >> k;

//     vector<vector<double>> centroids(k, vector<double>(data[0].size()));
//     for (int i = 0; i < k; ++i) {
//         cout << "Enter initial centroid(x,y) "  << ":\n";
//         for (size_t d = 0; d < data[0].size(); ++d)
//             cin >> centroids[i][d];
//     }

//     vector<int> labels;
//     kMeans(data, k, 100, centroids, labels);

  
//     cout << "\nFinal Centroids:\n";
//     for (int i = 0; i < k; ++i) {
//         cout << "Cluster " << i + 1 << ": ";
//         for (size_t d = 0; d < data[0].size(); ++d)
//             cout << centroids[i][d] << (d + 1 < data[0].size() ? ", " : "");
//         cout << endl;
//     }

    
//     cout << "\ndistance_matrix:\n";
//     for (size_t i = 0; i < data.size(); ++i) {
//         cout << pts_nm[i] << ": ";
//         for (int j = 0; j < k; ++j)
//             cout << distance(data[i], centroids[j]) << (j + 1 < k ? ", " : "");
//         cout << "\n";
//     }

   
//     map<int, vector<string>> clusters;
//     for (size_t i = 0; i < labels.size(); ++i)
//         clusters[labels[i]].push_back(pts_nm[i]);

//     cout << "\nClusters:\n";
//     for (auto &c : clusters) {
//         cout << "Cluster " << c.first + 1 << ": ";
//         for (auto &name : c.second)
//             cout << name << " ";
//         cout << "\n";
//     }

//     return 0;
// }   





// code will peint each iteration result
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

double distance(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

void printDistanceMatrix(const vector<vector<double>> &data,
                         const vector<string> &pts_nm,
                         const vector<vector<double>> &centroids,
                         int iter) {
    cout << "\n=== Distance Matrix at Iteration " << iter << " ===\n";
    for (size_t i = 0; i < data.size(); ++i) {
        cout << pts_nm[i] << ": ";
        for (size_t j = 0; j < centroids.size(); ++j)
            cout << distance(data[i], centroids[j]) 
                 << (j + 1 < centroids.size() ? ", " : "");
        cout << "\n";
    }
}

void printCentroids(const vector<vector<double>> &centroids, int iter) {
    cout << "\n=== Centroids at Iteration " << iter << " ===\n";
    for (size_t i = 0; i < centroids.size(); ++i) {
        cout << "Cluster " << i + 1 << ": ";
        for (size_t d = 0; d < centroids[i].size(); ++d)
            cout << centroids[i][d] << (d + 1 < centroids[i].size() ? ", " : "");
        cout << endl;
    }
}

void printClusterAssignments(const vector<string> &pts_nm, const vector<int> &labels, int iter) {
    cout << "\n=== Cluster Assignments at Iteration " << iter << " ===\n";
    for (size_t i = 0; i < labels.size(); ++i)
        cout << pts_nm[i] << " -> Cluster " << labels[i] + 1 << "\n";
}

void kMeans(const vector<vector<double>> &data, int k, int maxIter,
            vector<vector<double>> &centroids, vector<int> &labels,
            const vector<string> &pts_nm) {
    
    int n = data.size();
    labels.assign(n, -1);

    for (int iter = 1; iter <= maxIter; ++iter) {
        bool changed = false;

        // Assign clusters
        for (int i = 0; i < n; ++i) {
            double minDist = 1e18;
            int bestCluster = -1;
            for (int j = 0; j < k; ++j) {
                double dist = distance(data[i], centroids[j]);
                if (dist < minDist) {
                    minDist = dist;
                    bestCluster = j;
                }
            }
            if (labels[i] != bestCluster) {
                labels[i] = bestCluster;
                changed = true;
            }
        }

        // Print iteration details
        printDistanceMatrix(data, pts_nm, centroids, iter);
        printClusterAssignments(pts_nm, labels, iter);
        printCentroids(centroids, iter);

        // Recompute centroids
        vector<vector<double>> newCentroids(k, vector<double>(data[0].size(), 0.0));
        vector<int> counts(k, 0);

        for (int i = 0; i < n; ++i) {
            int c = labels[i];
            counts[c]++;
            for (size_t d = 0; d < data[0].size(); ++d)
                newCentroids[c][d] += data[i][d];
        }

        for (int j = 0; j < k; ++j)
            if (counts[j] > 0)
                for (size_t d = 0; d < data[0].size(); ++d)
                    newCentroids[j][d] /= counts[j];

        centroids = newCentroids;

        // If no change → stop
        if (!changed) {
            cout << "\nK-Means Converged at iteration " << iter << " ✅\n";
            break;
        }
    }
}

int main() {
    string file_nm;
    cout << "Enter CSV filename: ";
    cin >> file_nm;

    ifstream file(file_nm);
    string line;
    getline(file, line);

    vector<vector<double>> data;
    vector<string> pts_nm;

    while (getline(file, line)) {
        vector<string> tokens = splitCSV(line);
        if (tokens.empty()) continue;
        pts_nm.push_back(tokens[0]);
        vector<double> point;
        for (size_t i = 1; i < tokens.size(); ++i)
            point.push_back(stod(tokens[i]));
        data.push_back(point);
    }
    file.close();

    int k;
    cout << "Enter number of clusters k: ";
    cin >> k;

    vector<vector<double>> centroids(k, vector<double>(data[0].size()));
    cout << "\nEnter initial centroids:\n";
    for (int i = 0; i < k; ++i) {
        cout << "Centroid " << i + 1 << ":\n";
        for (size_t d = 0; d < data[0].size(); ++d)
            cin >> centroids[i][d];
    }

    vector<int> labels;
    kMeans(data, k, 100, centroids, labels, pts_nm);

    return 0;
}
