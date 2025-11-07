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
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF DBSCAN CLUSTERING PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **DBSCAN (Density-Based Spatial Clustering of Applications with Noise)** algorithm.
// It is used to group data points into clusters based on density (proximity) and mark outliers as *noise*.
// Unlike K-Means, DBSCAN does not require specifying the number of clusters beforehand.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ OVERVIEW OF FUNCTIONS
// --------------------------------------------------------------------------------------------------
//
// ➤ distanceCalc()
//     - Calculates the **Euclidean distance** between two points in n-dimensional space.
//     - Formula: √((x₁−x₂)² + (y₁−y₂)² + ... )
//     - Used to check if points are close enough (within epsilon distance) to belong to the same cluster.
//
// ➤ readCSV()
//     - Reads the input CSV file and extracts numeric data points.
//     - Each line corresponds to one data record.
//     - The first column (if non-numeric) is stored in “names[]” (like a label or ID).
//     - Returns a vector of vectors (data matrix).
//
// ➤ regionQuery()
//     - Finds all points within **epsilon (eps)** distance from a given point.
//     - Returns a list (vector<int>) of neighboring points (their indices).
//     - This is the core of density checking — points within eps radius are considered *neighbors*.
//
// ➤ expandCluster()
//     - Expands the cluster from a “core point” (a point with enough neighbors).
//     - It recursively adds all reachable points (directly or indirectly within eps).
//     - If new neighbors have enough nearby points, they’re also expanded.
//     - This forms a *dense region* = one cluster.
//
// ➤ dbscan()
//     - The main algorithm.
//     - It scans through all points, checks their neighborhoods, and assigns them as:
//          ▪ Part of a cluster (if dense region found)
//          ▪ Noise (if not enough nearby points)
//     - Prints all intermediate steps (neighbors found, cluster formation, noise points).
//     - Displays final clusters clearly.
//
// ➤ main()
//     - Takes input filename, epsilon (eps), and minimum points (minPts) from user.
//     - Calls the DBSCAN algorithm and prints cluster results.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ WORKING OF DBSCAN (ALGORITHM LOGIC)
// --------------------------------------------------------------------------------------------------
//
// STEP 1: For each unvisited point
//     → Find all neighboring points within eps distance (regionQuery).
//     → If number of neighbors < minPts → mark as NOISE.
//     → Else → start a new cluster.
//
// STEP 2: Expand the cluster
//     → For each neighbor of the core point:
//         - If unvisited → label it as part of the cluster.
//         - If neighbor has ≥ minPts points → recursively expand it (grow the cluster).
//
// STEP 3: Repeat for all points
//     → Continue until all points are visited or marked.
//
// STEP 4: Output results
//     → Cluster IDs and their member points printed on screen.
//     → Points labeled -1 are considered noise (not part of any cluster).
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ EXAMPLE OF OUTPUT INTERPRETATION
// --------------------------------------------------------------------------------------------------
//
// Input Parameters:
//     Epsilon (eps) = 2.0
//     MinPts = 3
//
// Sample Output:
//     --- Starting DBSCAN ---
//     Epsilon (eps): 2.0, MinPts: 3
//
//     Point A has 4 neighbors.
//     Forming Cluster 1
//
//     Point D has 2 neighbors.
//     Marked as Noise
//
//     --- Final Clusters ---
//     Cluster 1: A B C
//     Noise: D E
//
// ➤ Interpretation:
//     - Points A, B, and C are close and form one cluster.
//     - D and E are far apart and treated as noise (outliers).
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ INTERNAL VARIABLE MEANINGS
// --------------------------------------------------------------------------------------------------
//
//     eps       → Radius within which points are considered neighbors.
//     minPts    → Minimum number of neighbors required to form a dense cluster.
//     labels[]  → Keeps cluster assignments for each point.
//                  ▪  0  → Unvisited
//                  ▪ -1  → Noise
//                  ▪ >0  → Cluster ID
//     clusterId → Tracks current cluster number.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ STRENGTHS OF DBSCAN
// --------------------------------------------------------------------------------------------------
//
// ✅ Automatically determines the number of clusters based on data density.
// ✅ Can detect arbitrary-shaped clusters (not just circular or spherical like K-Means).
// ✅ Robust to noise and outliers.
// ✅ Works well even when cluster sizes differ.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Sensitive to choice of eps and minPts — poor parameters may merge or split clusters incorrectly.
// ⚠️ Struggles when data has varying densities (some dense, some sparse regions).
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ WHY DBSCAN METHOD WAS CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Nature:
//     - The data contains continuous numeric attributes (e.g., age, salary, marks, etc.).
//     - There are no class labels (so classification algorithms like Decision Tree or Naive Bayes aren’t suitable).
//
// 🔹 Objective:
//     - To group similar data points automatically based on their proximity.
//     - Also to detect outliers or noise points that don’t fit in any cluster.
//
// 🔹 Why DBSCAN is Ideal Here:
//     1. It does not require knowing the number of clusters beforehand (unlike K-Means).
//     2. Can identify noise/outlier records explicitly.
//     3. Works well for irregular cluster shapes (non-linear boundaries).
//     4. Purely distance-based, so suitable for numerical datasets.
//
// 🔹 Comparison with Other Methods:
//     - **K-Means:** Requires number of clusters (k) → not always known.
//     - **Hierarchical Clustering:** Computationally expensive for large datasets.
//     - **Classification:** Needs labeled data, not available here.
//     - **DBSCAN:** Works on density — automatically discovers clusters and detects noise.
//
// ✅ Hence, DBSCAN was chosen as the most appropriate method for this dataset,
//    since it can cluster data without prior labels or fixed number of clusters.
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ FINAL CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The DBSCAN algorithm successfully groups data points based on density similarity.
// ➤ Points lying in high-density regions form clusters; isolated points are labeled as noise.
// ➤ It is a robust unsupervised learning method for exploratory data analysis.
// ➤ Results depend on tuning eps (distance threshold) and minPts (density threshold).
//
// Example Summary Output:
//
//     Cluster 1: x1 x2 x3
//     Cluster 2: y1 y2
//     Noise: z1 z2
//
// ➤ Conclusion:
//     - DBSCAN effectively identifies natural groupings in data without supervision.
//     - It highlights both patterns (clusters) and anomalies (noise).
//     - The technique is ideal for real-world data where the number of groups is unknown.
//
// ==================================================================================================
// ✅ FINAL REMARK:
// This experiment demonstrates **Density-Based Clustering (DBSCAN)** —
// a powerful unsupervised learning technique that discovers arbitrary-shaped clusters
// and isolates noise points effectively.
//
// ==================================================================================================
