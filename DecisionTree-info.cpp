#include <bits/stdc++.h>
using namespace std;

// ---------- Utility: Calculate Entropy ----------
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

// ---------- CSV Reader ----------
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

// ---------- Decision Tree Node ----------
struct Node {
    string attribute;
    map<string, Node*> children;
    string label;  // class label for leaf
};

// ---------- Recursive Tree Builder ----------
Node* buildTree(vector<vector<string>> data, vector<string> headers, string indent = "") {
    // Step 1: Count target classes
    map<string, double> classCounts;
    for (int i = 1; i < data.size(); i++)
        classCounts[data[i].back()]++;

    double currentEntropy = entropy(classCounts);

    cout << "\n" << indent << "---------------------------------------------\n";
    cout << indent << "Current Subset (" << data.size() - 1 << " records)\n";
    cout << indent << "Class Distribution: ";
    for (auto &p : classCounts) cout << p.first << "=" << p.second << " ";
    cout << "\n" << indent << "Parent Entropy = " << currentEntropy << endl;

    // Step 2: Pure node (all same class)
    if (currentEntropy == 0.0) {
        Node* leaf = new Node();
        leaf->label = data[1].back();
        cout << indent << "--> Leaf Node created with label: " << leaf->label << endl;
        return leaf;
    }

    // Step 3: If only one attribute left
    if (headers.size() <= 1) {
        string majorityClass;
        double maxCount = -1;
        for (auto &c : classCounts)
            if (c.second > maxCount)
                majorityClass = c.first, maxCount = c.second;
        Node* leaf = new Node();
        leaf->label = majorityClass;
        cout << indent << "--> Leaf (no attributes left): " << majorityClass << endl;
        return leaf;
    }

    // Step 4: Calculate info gain for each attribute
    int totalRecords = data.size() - 1;
    double totalEntropy = currentEntropy;

    string bestAttr;
    double bestInfoGain = -1;
    int bestCol = -1;

    for (int col = 0; col < headers.size() - 1; col++) {
        string attr = headers[col];
        map<string, map<string, double>> valueClassCount;

        // Count occurrences
        for (int i = 1; i < data.size(); i++)
            valueClassCount[data[i][col]][data[i].back()]++;

        double weightedEntropy = 0.0;

        cout << "\n" << indent << "Attribute: " << attr << endl;
        for (auto &kv : valueClassCount) {
            double subsetTotal = 0.0;
            for (auto &cls : kv.second) subsetTotal += cls.second;
            double e = entropy(kv.second);
            weightedEntropy += (subsetTotal / totalRecords) * e;

            cout << indent << "  " << attr << "=" << kv.first << " -> ";
            for (auto &cls : kv.second)
                cout << cls.first << "=" << cls.second << " ";
            cout << "| Entropy=" << e << endl;
        }

        double infoGain = totalEntropy - weightedEntropy;
        cout << indent << "  Information Gain (" << attr << ") = " << infoGain << endl;

        if (infoGain > bestInfoGain) {
            bestInfoGain = infoGain;
            bestAttr = attr;
            bestCol = col;
        }
    }

    cout << indent << "---------------------------------------------\n";
    cout << indent << "Best Attribute Chosen: " << bestAttr << " (Gain=" << bestInfoGain << ")\n";

    // Step 5: Split dataset by best attribute
    Node* node = new Node();
    node->attribute = bestAttr;

    map<string, vector<vector<string>>> subsets;
    for (int i = 1; i < data.size(); i++) {
        string val = data[i][bestCol];
        subsets[val].push_back(data[i]);
    }

    // Prepare reduced headers
    vector<string> newHeaders;
    for (int i = 0; i < headers.size(); i++)
        if (i != bestCol) newHeaders.push_back(headers[i]);

    // Recursive step
    for (auto &sub : subsets) {
        cout << "\n" << indent << "|-- Splitting on " << bestAttr << " = " << sub.first << endl;

        vector<vector<string>> newData;
        newData.push_back(newHeaders);
        for (auto &row : sub.second) {
            vector<string> newRow;
            for (int i = 0; i < row.size(); i++)
                if (i != bestCol) newRow.push_back(row[i]);
            newData.push_back(newRow);
        }

        node->children[sub.first] = buildTree(newData, newHeaders, indent + "   ");
    }

    return node;
}

// ---------- Tree Printing ----------
void printTree(Node* node, string indent = "") {
    if (!node) return;
    if (!node->attribute.empty()) {
        cout << indent << "Attribute: " << node->attribute << endl;
        for (auto &child : node->children) {
            cout << indent << "|-- " << node->attribute << " = " << child.first << endl;
            printTree(child.second, indent + "   ");
        }
    } else {
        cout << indent << "--> Leaf: " << node->label << endl;
    }
}

// ---------- Prediction ----------
string predict(Node* root, const vector<string> &headers, const vector<string> &record) {
    if (root->attribute.empty()) return root->label;

    string attr = root->attribute;
    int idx = -1;
    for (int i = 0; i < headers.size(); i++)
        if (headers[i] == attr) idx = i;

    if (idx == -1) return "Unknown";

    string val = record[idx];
    if (root->children.find(val) == root->children.end())
        return "Unknown";
    return predict(root->children[val], headers, record);
}

// ---------- Main ----------
int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    vector<vector<string>> data = readCSV(filename);
    if (data.empty()) {
        cout << "Error: Empty or invalid file.\n";
        return 0;
    }

    vector<string> headers = data[0];
    cout << fixed << setprecision(4);
    cout << "\n=========== ID3 Decision Tree Generation ===========" << endl;

    Node* root = buildTree(data, headers);

    cout << "\n=========== Final Decision Tree ===========" << endl;
    printTree(root);
    cout << "===============================================" << endl;

    // ---------- Test Prediction ----------
    cout << "\nEnter values for a test case:\n";
    vector<string> test(headers.size() - 1);
    for (int i = 0; i < headers.size() - 1; i++) {
        cout << headers[i] << ": ";
        cin >> test[i];
    }

    string result = predict(root, headers, test);
    cout << "\nPredicted Class = " << result << endl;

    return 0;
}
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF ID3 DECISION TREE PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **ID3 (Iterative Dichotomiser 3)** algorithm for classification.
// It builds a **Decision Tree** based on **Information Gain** calculated from **Entropy**.
// The goal is to classify data into predefined classes (e.g., Play = Yes/No) using attribute values.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ entropy()
//     - Calculates the **Entropy** of a dataset based on class distribution.
//     - Entropy measures impurity or disorder in data.
//     - Formula:  E(S) = −∑ (pi * log₂(pi))
//     - Example: if 4 samples are "Yes" and 4 are "No", entropy = 1 (maximum uncertainty).
//
// ➤ readCSV()
//     - Reads data from a CSV file and stores it in a 2D vector (vector<vector<string>>).
//     - Each row represents one record.
//     - The last column is treated as the **target attribute (class)**.
//     - The first row is stored as headers (attribute names).
//
// ➤ struct Node
//     - Defines a structure for a decision tree node.
//     - Each node has:
//         ▪ attribute – the attribute name used for splitting.
//         ▪ children  – map of attribute values → subtree pointers.
//         ▪ label     – class label (used only for leaf nodes).
//
// ➤ buildTree()
//     - Core recursive function that constructs the decision tree using the ID3 algorithm.
//     - Steps:
//         1️⃣ Compute class counts and entropy for the current subset.
//         2️⃣ If all records belong to one class → create a **Leaf Node**.
//         3️⃣ Otherwise, calculate **Information Gain** for each attribute.
//         4️⃣ Select the attribute with the **highest Information Gain** as the split attribute.
//         5️⃣ Partition data into subsets for each attribute value.
//         6️⃣ Recursively call buildTree() on each subset.
//
// ➤ printTree()
//     - Traverses and prints the tree in a readable, hierarchical format.
//     - Displays attributes and leaf labels for each branch.
//
// ➤ predict()
//     - Classifies a new, unseen record by traversing the decision tree.
//     - Follows branches based on the record’s attribute values.
//     - Returns the predicted class label (e.g., “Yes” or “No”).
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ ALGORITHM FLOW OF ID3
// --------------------------------------------------------------------------------------------------
//
// STEP 1: Start with the full dataset as the root node.
//
// STEP 2: Calculate entropy for the target variable (class).
//
// STEP 3: For each attribute:
//     - Split the data based on each possible attribute value.
//     - Calculate the weighted entropy for each split.
//     - Compute **Information Gain**:
//           Gain(Attribute) = Entropy(parent) − ∑ ( |subset| / |total| ) * Entropy(subset)
//
// STEP 4: Choose the attribute with the **highest Information Gain** as the best split.
//
// STEP 5: Create a node for this attribute and branches for each attribute value.
//
// STEP 6: Repeat recursively for each branch until:
//     - All examples belong to the same class (pure node), OR
//     - No attributes remain → assign majority class.
//
// STEP 7: The result is a complete Decision Tree.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ EXAMPLE WALKTHROUGH (Play_Tennis DATASET)
// --------------------------------------------------------------------------------------------------
//
// Example (simplified):
//   Outlook | Temperature | Humidity | Windy | Play
//   -----------------------------------------------
//   Sunny   | Hot         | High     | False | No
//   Sunny   | Hot         | Normal   | True  | Yes
//   Overcast| Cool        | High     | False | Yes
//   Rainy   | Mild        | High     | False | No
//
// Entropy(Play) = 0.97
//
// For each attribute:
//   - Compute subset entropies.
//   - Calculate weighted entropy.
//   - Information Gain = Parent Entropy − Weighted Entropy.
//
// Attribute with highest gain (e.g., Outlook) becomes root node.
//
// Resulting Tree (simplified):
//       Outlook
//       /   |    \
//    Sunny  Rainy  Overcast
//     /       \       \
//  Windy?    Play=No  Play=Yes
//     |
//   True→No
//   False→Yes
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ OUTPUT DESCRIPTION
// --------------------------------------------------------------------------------------------------
//
// The program prints intermediate results for clarity:
//     - Current subset and its class distribution
//     - Entropy of parent node
//     - Attribute-wise entropy and Information Gain
//     - Attribute chosen for splitting
//
// Example Output (Partial):
//
//     Current Subset (14 records)
//     Class Distribution: Yes=9 No=5
//     Parent Entropy = 0.9403
//
//     Attribute: Outlook
//       Outlook=Sunny -> Yes=2 No=3 | Entropy=0.9710
//       Outlook=Overcast -> Yes=4 | Entropy=0.0000
//       Outlook=Rainy -> Yes=3 No=2 | Entropy=0.9710
//       Information Gain (Outlook) = 0.246
//
//     ---------------------------------------------
//     Best Attribute Chosen: Outlook (Gain=0.246)
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ VARIABLES SUMMARY
// --------------------------------------------------------------------------------------------------
//
// data[][]     → Contains dataset (rows × attributes).
// headers[]    → Attribute names from first row.
// classCounts  → Counts of each class label in subset.
// infoGain     → Difference between parent and child entropies.
// Node*        → Pointer to each decision tree node (stores split attribute and children).
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ WHY DECISION TREE (ID3) METHOD WAS CHOSEN
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Type:
//     - The dataset (like “Play_Tennis1”) contains **categorical attributes** (sunny, rainy, windy, etc.)
//       and a **categorical target variable** (“Play”: yes/no).
//
// 🔹 Objective:
//     - To build a model that can classify or predict the target class for unseen data
//       based on attribute values.
//
// 🔹 Why ID3 is Ideal:
//     1️⃣ ID3 handles **categorical data** effectively.
//     2️⃣ It produces **interpretable decision rules** (if–then style).
//     3️⃣ It automatically selects the most informative attributes (using Information Gain).
//     4️⃣ It provides insight into which factors most influence the decision.
//
// 🔹 Comparison with Other Methods:
//     - **Naive Bayes:** Works on probabilities, but doesn’t visualize decision logic.
//     - **K-Means / DBSCAN:** Used for clustering, not classification.
//     - **Regression:** For numeric predictions, not categorical outcomes.
//     - **Apriori:** For association rules, not supervised learning.
//
// ✅ Therefore, the **ID3 Decision Tree** method is best suited for **classification tasks**
//    involving categorical attributes and discrete outcomes.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The ID3 algorithm successfully constructs a decision tree using entropy and information gain.
// ➤ Each internal node represents a decision (attribute test),
//    and each leaf node represents a class label.
//
// ➤ Advantages demonstrated:
//     - Produces a human-readable model.
//     - Automatically identifies key attributes.
//     - Accurately predicts unseen test cases.
//
// ➤ Example Output Summary:
//     Predicted Class = Yes
//
// ➤ Overall:
//     The ID3 Decision Tree is an effective classification method for small to medium-sized
//     categorical datasets. It is a foundational algorithm in data mining and machine learning.
//
// ==================================================================================================
//
// ✅ FINAL REMARK:
// This experiment demonstrates **Classification using the ID3 Decision Tree Algorithm**.
// It classifies categorical data based on Information Gain and Entropy,
// providing clear, interpretable decision rules and accurate predictions.
//
// ==================================================================================================
