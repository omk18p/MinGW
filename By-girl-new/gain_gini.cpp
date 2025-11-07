#include <bits/stdc++.h>
using namespace std;

// ---------- Function to calculate Entropy ----------
double entropy(const vector<string>& y) {
    unordered_map<string, int> freq;
    for (size_t i = 0; i < y.size(); ++i)
        freq[y[i]]++;

    double ent = 0.0;
    int total = y.size();
    for (unordered_map<string, int>::iterator it = freq.begin(); it != freq.end(); ++it) {
        double p = (double)it->second / total;
        ent -= p * log2(p);
    }
    return ent;
}

// ---------- Function to calculate Gini Index ----------
double gini(const vector<string>& y) {
    unordered_map<string, int> freq;
    for (size_t i = 0; i < y.size(); ++i)
        freq[y[i]]++;

    double g = 1.0;
    int total = y.size();
    for (unordered_map<string, int>::iterator it = freq.begin(); it != freq.end(); ++it) {
        double p = (double)it->second / total;
        g -= p * p;
    }
    return g;
}

// ---------- Split dataset by attribute value ----------
vector<vector<string>> splitDataset(const vector<vector<string>>& dataset, int attrIndex, const string& value) {
    vector<vector<string>> subset;
    for (size_t i = 0; i < dataset.size(); ++i) {
        if (dataset[i][attrIndex] == value)
            subset.push_back(dataset[i]);
    }
    return subset;
}

// ---------- Calculate weighted entropy and gini for feature ----------
pair<double, double> calculateFeatureScores(const vector<vector<string>>& dataset, int attrIndex, int classIndex) {
    unordered_set<string> values;
    for (size_t i = 0; i < dataset.size(); ++i)
        values.insert(dataset[i][attrIndex]);

    double total = dataset.size();
    double weightedEntropy = 0.0;
    double weightedGini = 0.0;

    for (unordered_set<string>::iterator it = values.begin(); it != values.end(); ++it) {
        vector<vector<string>> subset = splitDataset(dataset, attrIndex, *it);
        vector<string> subsetClass;
        for (size_t i = 0; i < subset.size(); ++i)
            subsetClass.push_back(subset[i][classIndex]);

        double subsetProb = subset.size() / total;
        weightedEntropy += subsetProb * entropy(subsetClass);
        weightedGini += subsetProb * gini(subsetClass);
    }

    return make_pair(weightedEntropy, weightedGini);
}

// ---------- MAIN ----------
int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cerr << "Error: Cannot open file!" << endl;
        return 1;
    }

    string line;
    vector<vector<string>> dataset;
    vector<string> header;

    // Read header
    if (getline(file, line)) {
        stringstream ss(line);
        string val;
        while (getline(ss, val, ',')) header.push_back(val);
    }

    // Read data rows
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        vector<string> row;
        while (getline(ss, val, ',')) row.push_back(val);
        if (!row.empty()) dataset.push_back(row);
    }

    file.close();

    int classIndex = header.size() - 1; // Last column is class
    vector<string> classCol;
    for (size_t i = 0; i < dataset.size(); ++i)
        classCol.push_back(dataset[i][classIndex]);

    double baseEntropy = entropy(classCol);
    double baseGini = gini(classCol);

    cout << fixed << setprecision(3);
    cout << "\nBase Entropy = " << baseEntropy << endl;
    cout << "Base Gini Index = " << baseGini << endl;
    cout << "\n=== Attribute-wise Info Gain & Gini ===\n";

    for (int i = 0; i < classIndex; ++i) {
        pair<double, double> featureScores = calculateFeatureScores(dataset, i, classIndex);
        double featureEntropy = featureScores.first;
        double featureGini = featureScores.second;
        double infoGain = baseEntropy - featureEntropy;

        cout << "\nAttribute: " << header[i] << endl;
        cout << "  Info Gain = " << infoGain << endl;
        cout << "  Gini Index = " << featureGini << endl;
    }

    return 0;
}
