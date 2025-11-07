#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <iomanip>
#include <set>
using namespace std;

// ---------- Utility: Calculate Gini Impurity ----------
double gini(const map<string, double> &classCounts) {
    double total = 0.0;
    for (auto &p : classCounts) total += p.second;
    if (total == 0) return 0.0;

    double g = 1.0;
    for (auto &p : classCounts) {
        double ratio = p.second / total;
        g -= ratio * ratio;
    }
    return g;
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

// ---------- Recursive Gini Tree Builder ----------
Node* buildTree(vector<vector<string>> data, vector<string> headers, string indent = "") {
    // Step 1: Count target classes
    map<string, double> classCounts;
    for (int i = 1; i < data.size(); i++)
        classCounts[data[i].back()]++;

    double currentGini = gini(classCounts);

    cout << "\n" << indent << "---------------------------------------------\n";
    cout << indent << "Current Subset (" << data.size() - 1 << " records)\n";
    cout << indent << "Class Distribution: ";
    for (auto &p : classCounts) cout << p.first << "=" << p.second << " ";
    cout << "\n" << indent << "Parent Gini = " << currentGini << endl;

    // Step 2: Pure node (Gini = 0)
    if (currentGini == 0.0) {
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

    // Step 4: Compute weighted Gini for each attribute
    int totalRecords = data.size() - 1;
    double bestWeightedGini = 999.0;
    string bestAttr;
    int bestCol = -1;

    for (int col = 0; col < headers.size() - 1; col++) {
        string attr = headers[col];
        map<string, map<string, double>> valueClassCount;

        // Count occurrences of target classes for each value
        for (int i = 1; i < data.size(); i++)
            valueClassCount[data[i][col]][data[i].back()]++;

        double weightedGini = 0.0;

        cout << "\n" << indent << "Attribute: " << attr << endl;
        for (auto &kv : valueClassCount) {
            double subsetTotal = 0.0;
            for (auto &cls : kv.second) subsetTotal += cls.second;
            double g = gini(kv.second);
            weightedGini += (subsetTotal / totalRecords) * g;

            cout << indent << "  " << attr << "=" << kv.first << " -> ";
            for (auto &cls : kv.second)
                cout << cls.first << "=" << cls.second << " ";
            cout << "| Gini=" << g << endl;
        }

        cout << indent << "  Weighted Gini(" << attr << ") = " << weightedGini << endl;

        if (weightedGini < bestWeightedGini) {
            bestWeightedGini = weightedGini;
            bestAttr = attr;
            bestCol = col;
        }
    }

    cout << indent << "---------------------------------------------\n";
    cout << indent << "Best Attribute Chosen: " << bestAttr << " (Lowest Weighted Gini = " << bestWeightedGini << ")\n";

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
    cout << "\n=========== GINI-BASED DECISION TREE ===========" << endl;

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
// 🔹 DETAILED EXPLANATION OF GINI-BASED DECISION TREE PROGRAM (CART METHOD)
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements a **Decision Tree Classifier** based on the **Gini Impurity** measure,
// which is the foundation of the **CART (Classification and Regression Tree)** algorithm.
//
// It classifies categorical data by selecting attributes that best split the dataset into pure subsets,
// minimizing the Gini impurity at each node.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ gini()
//     - Computes the **Gini Impurity** for a given class distribution.
//     - Gini measures how “pure” a dataset is in terms of class labels.
//     - Formula:  Gini(S) = 1 − ∑(pi²)
//         where pi = proportion of class i in the dataset
//     - Lower Gini value → purer node.
//
// ➤ readCSV()
//     - Reads the CSV file and converts it into a 2D vector of strings.
//     - Each row = one record.
//     - The last column in the dataset is assumed to be the target class label.
//
// ➤ struct Node
//     - Represents a node in the decision tree.
//     - Fields:
//         ▪ attribute → attribute name used for splitting.
//         ▪ children  → map of attribute values → pointers to child nodes.
//         ▪ label     → class label if node is a leaf.
//
// ➤ buildTree()
//     - Core recursive function that builds the decision tree using **Gini impurity**.
//     - Steps:
//         1️⃣ Compute Gini impurity for the current dataset.
//         2️⃣ If dataset is pure (Gini = 0) → create a leaf node with class label.
//         3️⃣ If no attributes left → create a leaf node with the majority class.
//         4️⃣ For each attribute:
//               - Compute weighted Gini impurity across its values.
//               - Choose the attribute with the **lowest weighted Gini** (best split).
//         5️⃣ Split dataset by this attribute and recursively build child nodes.
//
// ➤ printTree()
//     - Displays the final decision tree in a hierarchical structure.
//
// ➤ predict()
//     - Predicts the class label for an unseen test record.
//     - Traverses the tree according to input attribute values until a leaf node is reached.
//
// ➤ main()
//     - Reads dataset, builds the Gini-based decision tree, prints it, and predicts a class for user input.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ ALGORITHM LOGIC (CART USING GINI INDEX)
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → Calculate Gini Impurity for current dataset.
//
// STEP 2️⃣ → For each attribute:
//              - Split data into subsets based on unique attribute values.
//              - Compute Gini for each subset.
//              - Calculate weighted Gini = ∑(subset_size / total_size) * Gini(subset).
//
// STEP 3️⃣ → Choose attribute with **lowest weighted Gini** (best purity).
//
// STEP 4️⃣ → Split dataset using the chosen attribute, and recursively build subtrees.
//
// STEP 5️⃣ → Repeat until:
//              - All records in a subset belong to the same class (pure node), OR
//              - No attributes remain.
//
// STEP 6️⃣ → Output final decision tree and test prediction.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ INTERMEDIATE CALCULATIONS AND OUTPUT DETAILS
// --------------------------------------------------------------------------------------------------
//
// For each node, the program prints:
//     - Class distribution (e.g., Yes=5, No=3)
//     - Parent Gini value
//     - Attribute-wise subset Gini calculations
//     - Weighted Gini for each attribute
//     - Best attribute chosen for splitting
//
// Example Partial Output:
//
//     ---------------------------------------------
//     Current Subset (14 records)
//     Class Distribution: Yes=9 No=5
//     Parent Gini = 0.4592
//
//     Attribute: Outlook
//       Outlook=Sunny -> Yes=2 No=3 | Gini=0.4800
//       Outlook=Overcast -> Yes=4 | Gini=0.0000
//       Outlook=Rainy -> Yes=3 No=2 | Gini=0.4800
//       Weighted Gini(Outlook) = 0.3429
//
//     ---------------------------------------------
//     Best Attribute Chosen: Outlook (Lowest Weighted Gini = 0.3429)
//
// This indicates “Outlook” is the best attribute for splitting this dataset.
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ GINI IMPURITY EXAMPLE
// --------------------------------------------------------------------------------------------------
//
// Suppose a dataset has 10 samples:
//     Yes = 6, No = 4
//     Gini = 1 − [(6/10)² + (4/10)²]
//           = 1 − (0.36 + 0.16)
//           = 0.48
//
// A lower Gini score (closer to 0) means purer class separation.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ ADVANTAGES OF USING GINI INDEX OVER ENTROPY
// --------------------------------------------------------------------------------------------------
//
// ✅ Gini is computationally faster (no logarithms involved).
// ✅ It provides similar or better performance for classification tasks.
// ✅ The splitting criterion is straightforward — choose the attribute that minimizes impurity.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ WHY GINI-BASED DECISION TREE WAS CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Nature:
//     - The dataset contains **categorical attributes** (e.g., Outlook, Windy, etc.)
//       and a **categorical target class** (e.g., Play = Yes/No).
//
// 🔹 Objective:
//     - To classify records into one of multiple classes (e.g., Pass/Fail, Yes/No).
//
// 🔹 Why Gini Index is Ideal:
//     1️⃣ Gini works effectively for **classification problems** with discrete outcomes.
//     2️⃣ It measures impurity directly and chooses the attribute that yields **purest child nodes**.
//     3️⃣ It forms clear decision boundaries and interpretable rules.
//     4️⃣ It’s the splitting criterion used in **CART (Classification and Regression Trees)**.
//
// 🔹 Comparison with Other Methods:
//     - **ID3 (Entropy):** Uses Information Gain, but involves logarithmic computation.
//     - **Naive Bayes:** Probabilistic, but doesn’t visualize decision rules.
//     - **K-Means / DBSCAN:** Unsupervised — unsuitable for labeled classification.
//     - ✅ **Gini (CART):** Ideal for supervised classification with categorical data.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The Gini-based Decision Tree algorithm successfully builds a classification model
//     by recursively selecting attributes that minimize impurity.
//
// ➤ Each internal node represents a decision condition,
//     while each leaf node represents a predicted class.
//
// ➤ Advantages Observed:
//     - Simple, interpretable, and efficient.
//     - Automatically identifies the most discriminative attributes.
//     - Provides a clear hierarchy of decisions.
//
// ➤ Example Final Output Summary:
//
//     Attribute: Outlook
//     |-- Outlook = Sunny
//        Attribute: Humidity
//        |-- Humidity = High --> Leaf: No
//        |-- Humidity = Normal --> Leaf: Yes
//     |-- Outlook = Overcast --> Leaf: Yes
//     |-- Outlook = Rainy
//        Attribute: Windy
//        |-- Windy = True --> Leaf: No
//        |-- Windy = False --> Leaf: Yes
//
// ➤ Test Prediction:
//     Predicted Class = Yes
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Classification using a Gini-based Decision Tree (CART)**.
// The method effectively separates data into pure classes, providing a clear, interpretable
// decision-making model that can predict unseen cases accurately.
//
// ==================================================================================================
