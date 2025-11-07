#include <bits/stdc++.h>
using namespace std;

// Read CSV and return transactions
vector<set<string>> readCSV(const string &filename) {
    vector<set<string>> transactions;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file\n";
        return transactions;
    }

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        string itemsStr = line.substr(comma + 1);

        // remove quotes if present
        if (!itemsStr.empty() && itemsStr.front() == '"' && itemsStr.back() == '"')
            itemsStr = itemsStr.substr(1, itemsStr.size() - 2);

        vector<string> itemsVec;
        stringstream ss(itemsStr);
        string item;
        while (getline(ss, item, ',')) {
            // trim spaces
            item.erase(item.begin(), find_if(item.begin(), item.end(), [](int ch){ return !isspace(ch); }));
            item.erase(find_if(item.rbegin(), item.rend(), [](int ch){ return !isspace(ch); }).base(), item.end());
            itemsVec.push_back(item);
        }

        transactions.push_back(set<string>(itemsVec.begin(), itemsVec.end()));
    }
    file.close();
    return transactions;
}

// Generate candidate 1-itemsets
set<set<string>> genCandidate1(const vector<set<string>> &transactions) {
    set<set<string>> candidates;
    for (const auto &t : transactions) {
        for (const auto &item : t) {
            candidates.insert({item});
        }
    }
    return candidates;
}

// Count support for candidates
map<set<string>, int> countSupport(const vector<set<string>> &transactions, const set<set<string>> &candidates) {
    map<set<string>, int> supportCounts;
    for (const auto &t : transactions) {
        for (const auto &cand : candidates) {
            if (includes(t.begin(), t.end(), cand.begin(), cand.end()))
                supportCounts[cand]++;
        }
    }
    return supportCounts;
}

// Prune itemsets based on minSupport
set<set<string>> pruneItemsets(const map<set<string>, int> &supportCounts, double minSupport, int totalTransactions) {
    set<set<string>> frequent;
    for (const auto &p : supportCounts) {
        double supPercent = (double)p.second / totalTransactions * 100;
        if (supPercent >= minSupport)
            frequent.insert(p.first);
    }
    return frequent;
}

// Generate candidate k-itemsets
set<set<string>> generateCandidates(const set<set<string>> &frequentItemsets, int k) {
    set<set<string>> candidates;
    vector<set<string>> freqVec(frequentItemsets.begin(), frequentItemsets.end());
    int n = freqVec.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            set<string> merged = freqVec[i];
            merged.insert(freqVec[j].begin(), freqVec[j].end());
            if ((int)merged.size() == k)
                candidates.insert(merged);
        }
    }
    return candidates;
}

// Print frequent itemsets
void printFrequentItemsets(const set<set<string>> &frequentItemsets, const map<set<string>, int> &supportCounts, int totalTransactions) {
    for (const auto &itemset : frequentItemsets) {
        double supPercent = (double)supportCounts.at(itemset) / totalTransactions * 100;
        cout << "{ ";
        for (const auto &item : itemset) cout << item << " ";
        cout << "} - Support: " << fixed << setprecision(2) << supPercent << "%\n";
    }
}

// Generate association rules
void generateAssociationRules(const set<set<string>> &allFrequentItemsets, const map<set<string>, int> &supportCounts, int totalTransactions, double minConfidence) {
    for (const auto &itemset : allFrequentItemsets) {
        if (itemset.size() < 2) continue;
        vector<string> items(itemset.begin(), itemset.end());
        int n = items.size();
        int supItemset = supportCounts.at(itemset);

        for (int mask = 1; mask < (1 << n) - 1; mask++) {
            set<string> antecedent, consequent;
            for (int i = 0; i < n; i++) {
                if (mask & (1 << i)) antecedent.insert(items[i]);
                else consequent.insert(items[i]);
            }

            if (supportCounts.find(antecedent) == supportCounts.end()) continue;
            int supAntecedent = supportCounts.at(antecedent);
            double confidence = (double)supItemset / supAntecedent * 100;
            if (confidence >= minConfidence) {
                double supPercent = (double)supItemset / totalTransactions * 100;
                cout << "{ ";
                for (auto &a : antecedent) cout << a << " ";
                cout << "} => { ";
                for (auto &c : consequent) cout << c << " ";
                cout << "} (Support: " << supPercent << "%, Confidence: " << confidence << "%)\n";
            }
        }
    }
}

int main() {
    string filename;
    double minSupport, minConfidence;

    cout << "Enter CSV file name: ";
    cin >> filename;
    cout << "Enter minimum support (%): ";
    cin >> minSupport;
    cout << "Enter minimum confidence (%): ";
    cin >> minConfidence;

    vector<set<string>> transactions = readCSV(filename);
    int totalTransactions = transactions.size();

    set<set<string>> candidate1 = genCandidate1(transactions);
    map<set<string>, int> supportCounts = countSupport(transactions, candidate1);
    set<set<string>> frequentItemsets = pruneItemsets(supportCounts, minSupport, totalTransactions);
    set<set<string>> allFrequentItemsets = frequentItemsets;

    cout << "\nFrequent Itemsets (size 1):\n";
    printFrequentItemsets(frequentItemsets, supportCounts, totalTransactions);

    int k = 2;
    while (!frequentItemsets.empty()) {
        set<set<string>> candidateK = generateCandidates(frequentItemsets, k);
        if (candidateK.empty()) break;
        map<set<string>, int> supportK = countSupport(transactions, candidateK);
        frequentItemsets = pruneItemsets(supportK, minSupport, totalTransactions);

        if (!frequentItemsets.empty()) {
            cout << "\nFrequent Itemsets (size " << k << "):\n";
            printFrequentItemsets(frequentItemsets, supportK, totalTransactions);

            supportCounts.insert(supportK.begin(), supportK.end());
            allFrequentItemsets.insert(frequentItemsets.begin(), frequentItemsets.end());
        }
        k++;
    }

    cout << "\nAssociation Rules:\n";
    generateAssociationRules(allFrequentItemsets, supportCounts, totalTransactions, minConfidence);

    return 0;
}
