// #include <bits/stdc++.h>
// using namespace std;

// struct Cluster {
//     vector<string> labels;
//     vector<double> values;
// };

// // Trim leading/trailing whitespace
// string trim(const string &s) {
//     auto start = s.find_first_not_of(" \t\r\n");
//     auto end = s.find_last_not_of(" \t\r\n");
//     if (start == string::npos) return "";
//     return s.substr(start, end - start + 1);
// }

// // Compute centroid of a cluster
// double centroid(const Cluster &c) {
//     double sum = 0;
//     for (double v : c.values) sum += v;
//     return sum / c.values.size();
// }

// // Distance between two clusters based on linkage type
// double clusterDistance(const Cluster &c1, const Cluster &c2, int linkageType) {
//     if (linkageType == 1) { // Single
//         double minD = DBL_MAX;
//         for (double a : c1.values)
//             for (double b : c2.values)
//                 minD = min(minD, abs(a - b));
//         return minD;
//     } else if (linkageType == 2) { // Complete
//         double maxD = 0;
//         for (double a : c1.values)
//             for (double b : c2.values)
//                 maxD = max(maxD, abs(a - b));
//         return maxD;
//     } else { // Average
//         double sum = 0;
//         int count = 0;
//         for (double a : c1.values)
//             for (double b : c2.values) { sum += abs(a - b); count++; }
//         return sum / count;
//     }
// }

// // Print current clusters with centroids
// void printClusters(const vector<Cluster> &clusters) {
//     int idx = 1;
//     cout << "\nCurrent Clusters and Centroids:\n";
//     for (auto &c : clusters) {
//         cout << "Cluster " << idx++ << ": " << c.labels.size() << " points [";
//         for (size_t i = 0; i < c.labels.size(); i++) {
//             cout << c.labels[i];
//             if (i != c.labels.size() - 1) cout << ", ";
//         }
//         cout << "], Centroid = [" << fixed << setprecision(2) << centroid(c) << "]\n";
//     }
// }

// // Print distance matrix between clusters
// void printDistanceMatrix(const vector<Cluster> &clusters, int linkageType) {
//     int n = clusters.size();
//     vector<vector<double>> mat(n, vector<double>(n, 0));
//     for (int i = 0; i < n; i++)
//         for (int j = i + 1; j < n; j++)
//             mat[i][j] = mat[j][i] = clusterDistance(clusters[i], clusters[j], linkageType);

//     cout << "\nDistance Matrix:\n            ";
//     for (auto &c : clusters) {
//         for (auto &lbl : c.labels) cout << lbl;
//         cout << "       ";
//     }
//     cout << "\n";
//     for (int i = 0; i < n; i++) {
//         for (auto &lbl : clusters[i].labels) cout << lbl;
//         cout << "      ";
//         for (int j = 0; j < n; j++)
//             cout << fixed << setprecision(2) << mat[i][j] << "       ";
//         cout << "\n";
//     }
// }

// int main() {
//     string filename;
//     cout << "Enter CSV filename: ";
//     cin >> filename;

//     ifstream file(filename);
//     if (!file.is_open()) { cerr << "Cannot open file.\n"; return 1; }

//     string line;
//     getline(file, line); // read header
//     stringstream ss(line);
//     vector<string> headers;
//     string col;
//     while (getline(ss, col, ',')) headers.push_back(trim(col));

//     cout << "Columns in CSV:\n";
//     for (int i = 0; i < headers.size(); i++)
//         cout << i << "] " << headers[i] << "\n";

//     int colIndex;
//     cout << "Enter column index to cluster on: ";
//     cin >> colIndex;

//     vector<Cluster> clusters;
//     int rowNum = 0;
//     while (getline(file, line)) {
//         rowNum++;
//         stringstream ss(line);
//         string label, valueStr;
//         if (!getline(ss, label, ',')) { cerr << "Row " << rowNum << " invalid, skipping.\n"; continue; }
//         label = trim(label);

//         // Read the desired column
//         bool valid = false;
//         for (int i = 0; i <= colIndex; i++) {
//             if (!getline(ss, valueStr, ',')) break;
//         }

//         valueStr = trim(valueStr);
//         try {
//             double val = stod(valueStr);
//             clusters.push_back({{label}, {val}});
//         } catch (...) {
//             cerr << "Row " << rowNum << " invalid numeric value, skipping.\n";
//         }
//     }
//     file.close();

//     cout << "Choose linkage type:\n1] Single\n2] Complete\n3] Average\n";
//     int linkageType; cin >> linkageType;

//     int step = 1;
//     while (clusters.size() > 1) {
//         printClusters(clusters);
//         printDistanceMatrix(clusters, linkageType);

//         // Find closest pair
//         double minDist = DBL_MAX;
//         int idx1 = -1, idx2 = -1;
//         for (size_t i = 0; i < clusters.size(); i++)
//             for (size_t j = i + 1; j < clusters.size(); j++) {
//                 double d = clusterDistance(clusters[i], clusters[j], linkageType);
//                 if (d < minDist) { minDist = d; idx1 = i; idx2 = j; }
//             }

//         // Merge clusters
//         cout << "\nStep " << step++ << ": Merging clusters " 
//              << idx1+1 << " and " << idx2+1 
//              << " (distance=" << fixed << setprecision(2) << minDist << ")\n";

//         clusters[idx1].labels.insert(clusters[idx1].labels.end(),
//                                      clusters[idx2].labels.begin(), clusters[idx2].labels.end());
//         clusters[idx1].values.insert(clusters[idx1].values.end(),
//                                      clusters[idx2].values.begin(), clusters[idx2].values.end());
//         clusters.erase(clusters.begin() + idx2);
//     }

//     cout << "\n--- Final Cluster ---\n";
//     printClusters(clusters);
// }


//simple
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;

string trim(const string &s) {
    string result = s;
    result.erase(result.begin(), find_if(result.begin(), result.end(),
        [](unsigned char ch){ return !isspace(ch); }));
    result.erase(find_if(result.rbegin(), result.rend(),
        [](unsigned char ch){ return !isspace(ch); }).base(), result.end());
    return result;
}

vector<double> read_csv_file(const string &filename, int colIndex) {
    vector<double> values;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return values;
    }

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string field;
        int idx = 0;
        while (getline(ss, field, ',')) {
            if (idx == colIndex) {
                field = trim(field);
                if (!field.empty()) {
                    try {
                        values.push_back(stod(field));
                    } catch (...) {
                        cout << "Skipping invalid value: " << field << endl;
                    }
                }
                break;
            }
            idx++;
        }
    }
    return values;
}

double single_linkage(const vector<double> &c1, const vector<double> &c2) {
    double mini = numeric_limits<double>::max();
    for (double v1 : c1)
        for (double v2 : c2)
            mini = min(mini, abs(v1 - v2));
    return mini;
}

double complete_linkage(const vector<double> &c1, const vector<double> &c2) {
    double maxi = numeric_limits<double>::lowest();
    for (double v1 : c1)
        for (double v2 : c2)
            maxi = max(maxi, abs(v1 - v2));
    return maxi;
}

double average_linkage(const vector<double> &c1, const vector<double> &c2) {
    double sum = 0;
    int count = 0;
    for (double v1 : c1)
        for (double v2 : c2) {
            sum += abs(v1 - v2);
            count++;
        }
    return sum / count;
}

void printClusters(const vector<vector<double>> &clusters) {
    cout << "[";
    for (size_t i = 0; i < clusters.size(); i++) {
        cout << "[";
        for (size_t j = 0; j < clusters[i].size(); j++) {
            cout << clusters[i][j];
            if (j != clusters[i].size() - 1) cout << ", ";
        }
        cout << "]";
        if (i != clusters.size() - 1) cout << ", ";
    }
    cout << "]" << endl;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    int col_idx;
    cout << "Enter column index for clustering: ";
    cin >> col_idx;

    vector<double> values = read_csv_file(filename, col_idx);
    if (values.empty()) {
        cout << "No valid data found. Exiting.\n";
        return 0;
    }

    cout << "Choose linkage type:\n1] Single\n2] Complete\n3] Average\n";
    int your_choice;
    cin >> your_choice;
    if (your_choice < 1 || your_choice > 3) {
        cout << "Invalid choice. Exiting.\n";
        return 0;
    }

    vector<vector<double>> clusters;
    for (double v : values) clusters.push_back({v});

    int step = 1;
    while (clusters.size() > 1) {
        double minDist = numeric_limits<double>::max();
        int idx1 = -1, idx2 = -1;

        for (size_t i = 0; i < clusters.size(); i++) {
            for (size_t j = i + 1; j < clusters.size(); j++) {
                double dist = 0;
                if (your_choice == 1) dist = single_linkage(clusters[i], clusters[j]);
                else if (your_choice == 2) dist = complete_linkage(clusters[i], clusters[j]);
                else dist = average_linkage(clusters[i], clusters[j]);

                if (dist < minDist) {
                    minDist = dist;
                    idx1 = i;
                    idx2 = j;
                }
            }
        }

        cout << "\nStep " << step++ << ": Merging clusters with distance = "
             << fixed << setprecision(4) << minDist << endl;

        clusters[idx1].insert(clusters[idx1].end(), clusters[idx2].begin(), clusters[idx2].end());
        clusters.erase(clusters.begin() + idx2);

        printClusters(clusters);
    }

    cout << "\nFinal Cluster: ";
    printClusters(clusters);

    return 0;
}




///Clustering if distace matrix is given 
#include <bits/stdc++.h>
using namespace std;

// Print distance matrix with cluster labels
void printMatrix(const vector<vector<double>> &dist, const vector<string> &labels) {
    int n = dist.size();
    cout << "\nDistance Matrix:\n";
    cout << setw(10) << "";
    for (auto &l : labels) cout << setw(10) << l;
    cout << endl;
    for (int i = 0; i < n; i++) {
        cout << setw(10) << labels[i];
        for (int j = 0; j < n; j++)
            cout << setw(10) << fixed << setprecision(3) << dist[i][j];
        cout << endl;
    }
}

// Single Linkage
void singleLinkage(vector<vector<double>> dist, vector<string> labels) {
    int n = dist.size();
    vector<bool> merged(n, false);

    cout << "--- Single Linkage Clustering ---\n";
    for (int step = 1; step < n; step++) {
        double minDist = 1e18;
        int x=-1, y=-1;

        for (int i=0;i<n;i++) {
            if(merged[i]) continue;
            for(int j=i+1;j<n;j++){
                if(merged[j]) continue;
                if(dist[i][j]<minDist){ minDist=dist[i][j]; x=i; y=j; }
            }
        }

        cout << "\nStep " << step << ": Merge (" << labels[x] << "," << labels[y] << ") distance=" << minDist << endl;

        for(int i=0;i<n;i++){
            if(i!=x && i!=y && !merged[i])
                dist[x][i] = dist[i][x] = min(dist[x][i], dist[y][i]);
        }

        merged[y]=true;
        labels[x]=labels[x]+"+"+labels[y];
        printMatrix(dist, labels);
    }
    cout << "Final Cluster: " << labels[0] << endl;
}

// Complete Linkage
void completeLinkage(vector<vector<double>> dist, vector<string> labels) {
    int n = dist.size();
    vector<bool> merged(n,false);
    cout << "--- Complete Linkage Clustering ---\n";
    for(int step=1; step<n; step++){
        double minDist=1e18;
        int x=-1,y=-1;
        for(int i=0;i<n;i++){
            if(merged[i]) continue;
            for(int j=i+1;j<n;j++){
                if(merged[j]) continue;
                if(dist[i][j]<minDist){ minDist=dist[i][j]; x=i; y=j; }
            }
        }

        cout << "\nStep "<<step<<": Merge ("<<labels[x]<<","<<labels[y]<<") distance="<<minDist<<endl;

        for(int i=0;i<n;i++){
            if(i!=x && i!=y && !merged[i])
                dist[x][i] = dist[i][x] = max(dist[x][i], dist[y][i]);
        }

        merged[y]=true;
        labels[x]=labels[x]+"+"+labels[y];
        printMatrix(dist, labels);
    }
    cout<<"Final Cluster: "<<labels[0]<<endl;
}

// Average Linkage
void averageLinkage(vector<vector<double>> dist, vector<string> labels) {
    int n=dist.size();
    vector<bool> merged(n,false);
    vector<int> size(n,1); // cluster sizes

    cout<<"--- Average Linkage Clustering ---\n";
    for(int step=1; step<n; step++){
        double minDist=1e18;
        int x=-1,y=-1;
        for(int i=0;i<n;i++){
            if(merged[i]) continue;
            for(int j=i+1;j<n;j++){
                if(merged[j]) continue;
                if(dist[i][j]<minDist){ minDist=dist[i][j]; x=i; y=j; }
            }
        }

        cout<<"\nStep "<<step<<": Merge ("<<labels[x]<<","<<labels[y]<<") distance="<<minDist<<endl;

        for(int i=0;i<n;i++){
            if(i!=x && i!=y && !merged[i]){
                dist[x][i] = dist[i][x] = (dist[x][i]*size[x] + dist[y][i]*size[y])/(size[x]+size[y]);
            }
        }

        merged[y]=true;
        size[x]+=size[y];
        labels[x]=labels[x]+"+"+labels[y];
        printMatrix(dist, labels);
    }
    cout<<"Final Cluster: "<<labels[0]<<endl;
}

int main() {
    // Hardcoded example
    vector<string> labels = {"A","B","C","D"};
    vector<vector<double>> dist = {
        {0,2,6,10},
        {2,0,5,9},
        {6,5,0,4},
        {10,9,4,0}
    };

    cout << "Original Labels: ";
    for(auto &l: labels) cout << l << " ";
    cout << endl;

    // Choose linkage
    singleLinkage(dist, labels);
    

    return 0;
}
