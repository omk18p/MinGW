#include <bits/stdc++.h>
using namespace std;

double entropy(const map<string, double> &classCounts) {
    double total = 0.0;
    for (auto &p : classCounts) total += p.second;
    if (total == 0) return 0.0;

    double e = 0.0;
    for (auto &p : classCounts) {
        double ratio = p.second / total;
        if (ratio > 0)
            e += -ratio * log2(ratio);
    }
    return e;
}

vector<vector<string>> readCSV(const string &filename) {
    vector<vector<string>> data;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            if (!cell.empty() && cell.front() == ' ') cell.erase(cell.begin());
            if (!cell.empty() && cell.back() == ' ') cell.pop_back();
            row.push_back(cell);
        }
        data.push_back(row);
    }
    return data;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    vector<vector<string>> data = readCSV(filename);
    if (data.empty()) {
        cout << "Error: File empty or invalid.\n";
        return 0;
    }

    vector<string> headers = data[0];
    int totalRecords = data.size() - 1;
    string target = headers.back();

    cout << fixed << setprecision(4);

    // Step 1: Calculate Parent Entropy
    map<string, double> totalTargetCount;
    for (int i = 1; i < data.size(); i++)
        totalTargetCount[data[i].back()]++;

    double totalEntropy = entropy(totalTargetCount);
    cout << "\nEntropy (Parent) = " << totalEntropy << endl;

    string bestAttr;
    double bestInfoGain = -1;

    // Step 2: For each attribute, calculate Information Gain
    for (int col = 0; col < headers.size() - 1; col++) {
        string attr = headers[col];
        cout << "\nFor Attribute: " << attr << endl;

        // Count how target classes distribute under each attribute value
        map<string, map<string, double>> valueClassCount;
        for (int i = 1; i < data.size(); i++)
            valueClassCount[data[i][col]][data[i].back()]++;

        double weightedEntropy = 0.0;

        for (auto &kv : valueClassCount) {
            string value = kv.first;
            double subsetTotal = 0.0;
            for (auto &cls : kv.second) subsetTotal += cls.second;

            double e = entropy(kv.second);
            weightedEntropy += (subsetTotal / totalRecords) * e;

            cout << "  Entropy(" << value << ") = " << e << " -> ";
            for (auto &cls : kv.second)
                cout << cls.first << "=" << cls.second << " ";
            cout << endl;
        }

        double infoGain = totalEntropy - weightedEntropy;
        cout << "  Information Gain (" << attr << ") = " << infoGain << endl;
        cout << "----------------------------------------" << endl;

        if (infoGain > bestInfoGain) {
            bestInfoGain = infoGain;
            bestAttr = attr;
        }
    }

    cout << "\nBest Attribute = " << bestAttr << "  (Gain = " << bestInfoGain << ")\n";
    return 0;
}
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF ENTROPY AND INFORMATION GAIN CALCULATION PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program demonstrates **Entropy** and **Information Gain** computation, which are the
// core mathematical concepts behind the **ID3 Decision Tree algorithm**.
// 
// The goal is to determine **which attribute best splits the dataset** based on information gain —
// that is, how much "uncertainty" or "disorder" in the target class is reduced by using that attribute.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ entropy()
//     - Calculates the **entropy** of a given class distribution.
//     - Entropy measures impurity or randomness in the data.
//     - Formula:  Entropy(S) = − ∑ (pi * log₂(pi))
//         where pi = proportion of class i in the dataset.
//     - Example:
//           If 4 samples are “Yes” and 4 are “No”
//           → pi(Yes) = 0.5, pi(No) = 0.5
//           → Entropy = −[0.5*log₂(0.5) + 0.5*log₂(0.5)] = 1.0
//       (Maximum uncertainty)
//
// ➤ readCSV()
//     - Reads the dataset from a CSV file.
//     - Stores each row as a vector<string>.
//     - The first row (headers) contains attribute names.
//     - The last column is the **target attribute** (class label).
//
// ➤ main()
//     - Reads the dataset.
//     - Calculates total (parent) entropy of the target class.
//     - For each attribute, calculates the **weighted child entropies** and **Information Gain**.
//     - Selects and displays the attribute with the **highest information gain** —
//       the best attribute for splitting in a decision tree.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ STEP-BY-STEP LOGICAL FLOW
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → Input and Initialization
//     - User enters CSV filename (e.g., “play_tennis.csv”).
//     - Data is read into a 2D vector “data”.
//     - Headers are extracted from the first row.
//     - The target class is assumed to be the **last column**.
//
// STEP 2️⃣ → Calculate Parent Entropy
//     - Count how many records belong to each target class (e.g., Yes/No).
//     - Pass these counts to entropy() to compute parent entropy (total disorder).
//     - Example Output:
//           Entropy (Parent) = 0.9403
//
// STEP 3️⃣ → Attribute-wise Entropy Calculation
//     - For each attribute (except target):
//         ▪ Group records based on attribute value (e.g., Outlook = Sunny, Overcast, Rainy).
//         ▪ For each group, calculate entropy of the target class distribution.
//         ▪ Multiply each subset’s entropy by its proportion in the total dataset.
//     - Example:
//           For Outlook:
//               Sunny → Entropy = 0.971
//               Overcast → Entropy = 0.000
//               Rainy → Entropy = 0.971
//
// STEP 4️⃣ → Compute Weighted Entropy & Information Gain
//     - Weighted Entropy = ∑ (subset_size / total_size) * Entropy(subset)
//     - Information Gain = Parent Entropy − Weighted Entropy
//     - The attribute with the **highest Information Gain** gives the purest split.
//
// STEP 5️⃣ → Display Results
//     - Prints each attribute’s entropy and gain.
//     - Displays the **best attribute** for decision tree root selection.
//
// Example Output:
//
//     Entropy (Parent) = 0.9403
//
//     For Attribute: Outlook
//       Entropy(Sunny) = 0.9710 -> Yes=2 No=3
//       Entropy(Overcast) = 0.0000 -> Yes=4
//       Entropy(Rainy) = 0.9710 -> Yes=3 No=2
//       Information Gain (Outlook) = 0.246
//     ----------------------------------------
//
//     For Attribute: Humidity
//       Entropy(High) = 1.0000 -> Yes=3 No=3
//       Entropy(Normal) = 0.0000 -> Yes=6
//       Information Gain (Humidity) = 0.151
//
//     Best Attribute = Outlook (Gain = 0.246)
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ INTERNAL VARIABLE DESCRIPTION
// --------------------------------------------------------------------------------------------------
//
// data[][]          → Stores dataset read from CSV.
// headers[]         → Stores attribute names from first row.
// totalRecords      → Number of data records (excluding header).
// target            → Name of target class column.
// totalTargetCount  → Map storing frequency of each class label (e.g., Yes=9, No=5).
// valueClassCount   → Nested map to count occurrences of class labels for each attribute value.
// totalEntropy      → Entropy of the parent dataset (before splitting).
// weightedEntropy   → Average entropy across all attribute values, weighted by subset size.
// infoGain          → Difference between totalEntropy and weightedEntropy (gain in purity).
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ WHY ENTROPY AND INFORMATION GAIN ARE USED
// --------------------------------------------------------------------------------------------------
//
// 🔹 Entropy measures **disorder** in the dataset.
//     - High entropy → data is mixed and impure.
//     - Low entropy → data is more pure (closer to one class).
//
// 🔹 Information Gain quantifies **how much entropy is reduced** by splitting on an attribute.
//     - High information gain → attribute provides more useful information.
//     - The attribute with maximum gain is chosen for the **root** or **internal split** in the tree.
//
// 🔹 Example Intuition:
//     - If “Outlook = Overcast” always results in “Play = Yes”, entropy = 0 → perfectly pure branch.
//     - If “Outlook = Sunny” results in a 50–50 Yes/No split → high entropy → less useful.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ WHY THIS METHOD (ID3-STYLE ENTROPY) WAS CHOSEN
// --------------------------------------------------------------------------------------------------
//
// ✅ Dataset Type:
//     - The dataset (e.g., “Play_Tennis1”) has **categorical attributes** and a **categorical class label**.
//     - Perfect for **classification** tasks.
//
// ✅ Reason for Choosing ID3/Entropy-Based Method:
//     1️⃣ Handles categorical attributes naturally.
//     2️⃣ Identifies the most significant attribute for classification.
//     3️⃣ Produces interpretable results (which attribute influences decision most).
//     4️⃣ Foundation for building a full **Decision Tree Classifier (ID3)**.
//
// ✅ Comparison with Other Techniques:
//     - **Gini Index (CART):** Similar purpose but uses a different impurity measure (Gini instead of Entropy).
//     - **Naive Bayes:** Based on probability, not attribute-based splitting.
//     - **K-Means / DBSCAN:** Unsupervised clustering methods (no class labels).
//     - ✅ **Entropy (ID3)** is ideal for categorical classification and feature selection.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ This program demonstrates how **Entropy** and **Information Gain** guide the selection of
//     the best attribute for classification.
//
// ➤ The attribute with the **highest Information Gain** reduces uncertainty the most and forms
//     the root node in a Decision Tree.
//
// ➤ Example Conclusion:
//     “Outlook” was chosen as the root attribute since it provided the highest gain (0.246).
//
// ➤ Summary of Advantages:
//     - Quantifies uncertainty mathematically.
//     - Provides objective attribute ranking.
//     - Forms the foundation for **ID3 Decision Tree construction**.
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates the **Information Gain and Entropy Method** — the core principle
// behind the ID3 Decision Tree algorithm.  
// It effectively identifies the most informative attributes for classification, enabling the
// construction of accurate and interpretable decision models.
//
// ==================================================================================================
