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

// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF RANDOMIZED K-MEANS CLUSTERING PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program performs **K-Means Clustering** using **random centroid initialization**.
// It groups unlabeled data into ‘k’ clusters based on Euclidean distance between data points.
//
// This implementation improves upon the basic version by:
//  - Randomly initializing centroids (to avoid bias).
//  - Tracking convergence through iterations.
//  - Printing intermediate clusters and centroid updates.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ splitCSV()
//     - Reads a single line from the CSV and splits it by commas (`,`).
//     - Removes extra spaces and returns a vector of string tokens.
//     - Used for parsing the dataset properly.
//
// ➤ distCalc()
//     - Calculates the **Euclidean distance** between two multi-dimensional points.
//     - Formula:
//           d(A, B) = √[ (a₁−b₁)² + (a₂−b₂)² + ... + (an−bn)² ]
//     - Lower distance = higher similarity (used to assign clusters).
//
// ➤ kMeans()
//     - Core function that executes the **K-Means algorithm** with iterative refinement.
//     - Steps involved:
//         1️⃣ Assign points to the nearest centroid.
//         2️⃣ Update centroids as mean of all points in each cluster.
//         3️⃣ Repeat until centroids stabilize or max iterations reached.
//
// ➤ main()
//     - Reads data from CSV file.
//     - Randomly initializes centroids.
//     - Calls kMeans() for clustering.
//     - Displays intermediate and final results.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ STEP-BY-STEP WORKING OF THE ALGORITHM
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → READ DATA
//     - CSV file is opened and the header line is skipped.
//     - Each record is read as:
//           [Name, Feature1, Feature2, ...]
//     - Names are stored in pointNames[] for easy display.
//     - Features (numeric values) are stored in data[][] for calculations.
//
// STEP 2️⃣ → RANDOM INITIALIZATION
//     - Randomly choose ‘k’ data points as initial centroids.
//     - This avoids bias (unlike the first-k initialization).
//     - Ensures different runs can produce different, sometimes better, clustering results.
//
// STEP 3️⃣ → ASSIGNMENT STEP
//     - For each data point, calculate its distance to all centroids using Euclidean distance.
//     - Assign the point to the cluster with the minimum distance (closest centroid).
//     - Print which points belong to each cluster.
//
// STEP 4️⃣ → UPDATE STEP
//     - Compute new centroids as the mean of all points in each cluster.
//     - Formula (for each cluster j):
//           Cj = (Σ points in cluster j) / (count of cluster j)
//     - Updated centroids represent new cluster centers.
//
// STEP 5️⃣ → CHECK FOR CONVERGENCE
//     - If no points change clusters (i.e., assignments remain the same):
//           → centroids have stabilized → stop iterations.
//     - Else, repeat Assignment + Update steps.
//
// STEP 6️⃣ → OUTPUT FINAL RESULTS
//     - Print final centroid coordinates.
//     - Display which cluster each point belongs to.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ SAMPLE ILLUSTRATION (EXAMPLE DATA)
// --------------------------------------------------------------------------------------------------
//
// CSV Input (Example: 2D points):
//     Name,X,Y
//     A,2,3
//     B,3,3
//     C,6,8
//     D,7,9
//     E,8,10
//
// User Input:
//     Enter number of clusters (k): 2
//
// Sample Output:
//
//     Initial Random Centroids:
//     Centroid 1: 3 3
//     Centroid 2: 8 10
//
//     Iteration 1:
//     Cluster 1: 1 2 
//     Cluster 2: 3 4 5
//     Updated Centroids:
//     Centroid 1: 2.50 3.00
//     Centroid 2: 7.00 9.00
//
//     Iteration 2:
//     Centroids stabilized — stopping iterations.
//
//     Final Centroids:
//     Cluster 1: 2.50 3.00
//     Cluster 2: 7.00 9.00
//
//     Cluster Assignments:
//     A - Cluster 1
//     B - Cluster 1
//     C - Cluster 2
//     D - Cluster 2
//     E - Cluster 2
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ VARIABLES USED
// --------------------------------------------------------------------------------------------------
//
// fileName         → Input CSV filename from user.
// pointNames[]     → Stores names/labels of points for easier identification.
// data[][]         → Matrix of feature values (each row = data point).
// centroids[][]    → Coordinates of current centroids (cluster centers).
// labels[]         → Cluster label assigned to each point.
// k                → Number of clusters entered by user.
// changed          → Boolean flag to detect if cluster assignment changes (used for stopping condition).
// clusters{}       → Temporary map to store points grouped by cluster number during each iteration.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ CHARACTERISTICS OF K-MEANS ALGORITHM
// --------------------------------------------------------------------------------------------------
//
// ✅ Type: **Unsupervised Learning** (no target labels).
// ✅ Goal: Partition data into k clusters minimizing intra-cluster distance.
// ✅ Input: Numerical data points + number of clusters (k).
// ✅ Output: Final centroids and cluster memberships.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ ADVANTAGES
// --------------------------------------------------------------------------------------------------
//
// • Simple, efficient, and easy to implement.
// • Works well on large, continuous-valued datasets.
// • Fast convergence with low computational complexity O(n*k*iterations).
// • Produces distinct, compact, and spherical clusters.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Requires predefining number of clusters (k).
// ⚠️ Sensitive to random initialization — different runs can yield different clusters.
// ⚠️ Struggles with non-spherical clusters or datasets with noise/outliers.
// ⚠️ May converge to a local minimum (not always globally optimal).
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ WHY K-MEANS WAS CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Type:
//     - The dataset contains **numerical data** (e.g., salary, marks, coordinates, etc.).
//     - No pre-labeled class information is available (unsupervised problem).
//
// 🔹 Objective:
//     - To discover natural groupings or patterns within the data using distance-based clustering.
//
// 🔹 Why K-Means is Ideal:
//     1️⃣ Well-suited for numerical, continuous features.
//     2️⃣ Automatically partitions data into k groups with similar characteristics.
//     3️⃣ Provides clear centroid outputs for each cluster.
//     4️⃣ Simple, fast, and widely used in industry (market segmentation, image compression, etc.).
//
// 🔹 Comparison with Other Methods:
//     - **Hierarchical Clustering:** Computationally expensive for large datasets.
//     - **DBSCAN:** Better for non-spherical clusters, but requires tuning eps/minPts.
//     - ✅ **K-Means:** Efficient, interpretable, and best suited for compact, spherical clusters.
//
// --------------------------------------------------------------------------------------------------
// 🔸 9️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The program successfully groups data into k clusters based on similarity (Euclidean distance).
// ➤ It iteratively refines centroids until stability (no change in assignments).
// ➤ Each cluster is represented by its centroid (mean position of points).
//
// ➤ Example Conclusion Statement:
//     “K-Means algorithm partitioned data into 2 clusters in 3 iterations,
//      achieving stable centroids at (2.5, 3.0) and (7.0, 9.0).”
//
// ➤ Practical Applications:
//     - Customer segmentation
//     - Pattern recognition
//     - Market analysis
//     - Image compression
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Unsupervised Learning using the K-Means Algorithm**.
// The algorithm efficiently discovers underlying structures in unlabeled data
// by minimizing intra-cluster variance and producing interpretable cluster groups.
//
// ==================================================================================================
