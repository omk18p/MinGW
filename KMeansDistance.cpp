#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

// Function to calculate Euclidean distance between two points
double distanceCalc(const vector<double> &a, const vector<double> &b) {
    double sum = 0;
    for (int i = 0; i < a.size(); i++)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

// Function to read CSV file (works for both numeric and labeled data)
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
            // Try converting to number
            try {
                double num = stod(value);
                row.push_back(num);
            } catch (...) {
                if (firstValue) {
                    name = value; // store row name (like A, B, C)
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

// Function to perform K-Means clustering
void kMeans(vector<vector<double>> &data, int k, const vector<string> &names, int max_iter = 100) {
    int n = data.size();
    int m = data[0].size();
    vector<vector<double>> centroids(k, vector<double>(m, 0));
    vector<int> cluster(n, -1);

    // Initialize centroids as first k points
    for (int i = 0; i < k; i++)
        centroids[i] = data[i];

    cout << "\nInitial Centroids:\n";
    for (int i = 0; i < k; i++) {
        cout << "Centroid " << i + 1 << ": ";
        for (double val : centroids[i]) cout << fixed << setprecision(2) << val << " ";
        cout << endl;
    }

    for (int iter = 1; iter <= max_iter; iter++) {
        bool changed = false;

        cout << "\n--- Iteration " << iter << " ---\n";

        // Step 1: Assign clusters
        for (int i = 0; i < n; i++) {
            double minDist = 1e9;
            int best = 0;
            for (int j = 0; j < k; j++) {
                double d = distanceCalc(data[i], centroids[j]);
                if (d < minDist) {
                    minDist = d;
                    best = j;
                }
            }
            if (cluster[i] != best) {
                cluster[i] = best;
                changed = true;
            }
            cout << "Point " << (names[i].empty() ? to_string(i + 1) : names[i])
                 << " -> Cluster " << best + 1 << " (Distance: " << fixed << setprecision(2) << minDist << ")\n";
        }

        // Step 2: Update centroids
        vector<vector<double>> newCentroids(k, vector<double>(m, 0));
        vector<int> count(k, 0);
        for (int i = 0; i < n; i++) {
            int c = cluster[i];
            for (int j = 0; j < m; j++)
                newCentroids[c][j] += data[i][j];
            count[c]++;
        }

        for (int j = 0; j < k; j++) {
            if (count[j] > 0)
                for (int p = 0; p < m; p++)
                    newCentroids[j][p] /= count[j];
        }

        centroids = newCentroids;

        cout << "\nUpdated Centroids:\n";
        for (int i = 0; i < k; i++) {
            cout << "Centroid " << i + 1 << ": ";
            for (double val : centroids[i]) cout << fixed << setprecision(2) << val << " ";
            cout << endl;
        }

        if (!changed) {
            cout << "\nCentroids stabilized - stopping iterations.\n";
            break;
        }
    }

    // Final Clusters
    cout << "\nFinal Cluster Assignments:\n";
    for (int i = 0; i < n; i++) {
        cout << (names[i].empty() ? "Data Point " + to_string(i + 1) : names[i])
             << " - Cluster " << cluster[i] + 1 << endl;
    }
}

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

    int k;
    cout << "Enter number of clusters: ";
    cin >> k;

    kMeans(data, k, names);

    return 0;
}
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF K-MEANS CLUSTERING PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **K-Means Clustering Algorithm** — a fundamental **unsupervised learning**
// technique used in data mining to group data points into *k clusters* based on similarity.
//
// The algorithm minimizes the distance between data points and their assigned cluster centroids,
// effectively finding “natural groupings” in data without predefined labels.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ distanceCalc()
//     - Computes the **Euclidean distance** between two data points in n-dimensional space.
//     - Formula: 
//           d(A, B) = √[(a₁−b₁)² + (a₂−b₂)² + ... + (an−bn)²]
//     - Used to measure similarity — smaller distance → higher similarity.
//
// ➤ readCSV()
//     - Reads data from a CSV file.
//     - Each row is converted into a vector of numeric attributes (features).
//     - Also stores optional row labels (like A, B, C) for readability.
//     - Skips header line automatically.
//
// ➤ kMeans()
//     - Implements the full **K-Means algorithm**.
//     - Steps:
//         1️⃣ Initialize centroids.
//         2️⃣ Assign each data point to the nearest centroid.
//         3️⃣ Recalculate centroids as mean of assigned points.
//         4️⃣ Repeat until centroids do not change (convergence).
//
// ➤ main()
//     - Accepts filename and number of clusters (k) from user.
//     - Reads data, calls `kMeans()`, and displays cluster results.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ K-MEANS ALGORITHM LOGIC (STEP-BY-STEP)
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → Initialization
//     - Choose number of clusters (k) from user.
//     - Select first k data points as initial centroids.
//
// STEP 2️⃣ → Assignment Step
//     - For each data point, compute distance to all centroids.
//     - Assign the point to the cluster whose centroid is closest (minimum distance).
//
// STEP 3️⃣ → Update Step
//     - For each cluster, compute the new centroid as the **mean** of all points assigned to that cluster.
//     - Formula:
//           Centroid_j = (Σ points in cluster_j) / (number of points in cluster_j)
//
// STEP 4️⃣ → Convergence Check
//     - If no points change their assigned clusters → centroids have stabilized → stop.
//     - Otherwise, repeat assignment and update steps.
//
// STEP 5️⃣ → Output
//     - Print iteration details, centroids, and final cluster memberships.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ EXAMPLE RUN (ILLUSTRATION)
// --------------------------------------------------------------------------------------------------
//
// Input CSV (Example):
//     Name,X,Y
//     A,2,3
//     B,3,3
//     C,6,8
//     D,7,9
//
// User Input:
//     Enter number of clusters: 2
//
// Output (Example):
//     Initial Centroids:
//     Centroid 1: 2.00 3.00
//     Centroid 2: 3.00 3.00
//
//     --- Iteration 1 ---
//     Point A -> Cluster 1 (Distance: 0.00)
//     Point B -> Cluster 1 (Distance: 1.00)
//     Point C -> Cluster 2 (Distance: 6.40)
//     Point D -> Cluster 2 (Distance: 7.07)
//
//     Updated Centroids:
//     Centroid 1: 2.50 3.00
//     Centroid 2: 6.50 8.50
//
//     Centroids stabilized - stopping iterations.
//
//     Final Cluster Assignments:
//     A - Cluster 1
//     B - Cluster 1
//     C - Cluster 2
//     D - Cluster 2
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ INTERNAL VARIABLE DESCRIPTION
// --------------------------------------------------------------------------------------------------
//
// data[][]       → Numeric data points (each row = 1 point).
// names[]        → Optional point labels (for display).
// k              → Number of clusters (user input).
// centroids[][]  → Current cluster centers.
// cluster[]      → Stores which cluster each data point belongs to.
// changed        → Boolean flag that tracks if any point changed its cluster assignment.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ CHARACTERISTICS OF K-MEANS CLUSTERING
// --------------------------------------------------------------------------------------------------
//
// ✅ **Type:** Unsupervised Learning (no class labels required).
// ✅ **Goal:** Minimize within-cluster variance (points inside a cluster are close to each other).
// ✅ **Input:** Dataset with numeric features + number of clusters (k).
// ✅ **Output:** Cluster assignments for each data point + centroid positions.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ ADVANTAGES OF K-MEANS
// --------------------------------------------------------------------------------------------------
//
// • Simple and efficient algorithm for large datasets.
// • Works well for spherical (convex-shaped) clusters.
// • Fast convergence using Euclidean distance.
// • Easy to interpret and visualize.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ LIMITATIONS OF K-MEANS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Must specify the number of clusters (k) in advance.
// ⚠️ Sensitive to initial centroid positions (can lead to different results).
// ⚠️ Struggles with non-spherical clusters or varying densities.
// ⚠️ Outliers can distort centroids significantly.
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ WHY K-MEANS WAS CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Nature:
//     - Data consists of **continuous numerical attributes** (e.g., salary, marks, coordinates, etc.).
//     - No predefined class labels (unsupervised learning problem).
//
// 🔹 Objective:
//     - Group similar data points based on Euclidean distance.
//     - Discover underlying structure or natural grouping in data.
//
// 🔹 Why K-Means Fits Perfectly:
//     1️⃣ Automatically divides data into k groups.
//     2️⃣ Efficient for moderate to large numeric datasets.
//     3️⃣ Provides clear numeric centroids for each cluster.
//     4️⃣ One of the most widely used and interpretable clustering algorithms.
//
// 🔹 Comparison with Other Methods:
//     - **DBSCAN:** Detects arbitrary-shaped clusters and noise, but needs eps and minPts tuning.
//     - **Hierarchical Clustering:** Computationally heavier for large data.
//     - ✅ **K-Means:** Simpler, faster, and easy to visualize cluster separation.
//
// --------------------------------------------------------------------------------------------------
// 🔸 9️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The K-Means algorithm successfully groups similar data points into k clusters.
// ➤ Each cluster is represented by its centroid — the mean position of all points within it.
// ➤ The process iteratively refines centroids until stabilization (no more reassignments).
// ➤ Output provides final cluster memberships and centroids.
//
// ➤ Example Conclusion Statement:
//     “K-Means clustered the dataset into 2 groups based on Euclidean distance.
//      The algorithm converged after 3 iterations with stable centroids.”
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Unsupervised Learning via K-Means Clustering** —
// a foundational data mining technique for pattern discovery.  
// It effectively groups data points with similar characteristics without prior class information.
//
// ==================================================================================================
