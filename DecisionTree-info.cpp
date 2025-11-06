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