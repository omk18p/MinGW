#include <bits/stdc++.h>
using namespace std;

// Function to calculate support count of an itemset
int getSupportCount(const set<string> &itemset, const vector<set<string>> &transactions) {
    int count = 0;
    for (auto &transaction : transactions) {
        if (includes(transaction.begin(), transaction.end(), itemset.begin(), itemset.end()))
            count++;
    }
    return count;
}

// Helper function to print a set nicely
void printSet(const set<string> &s) {
    int idx = 0;
    cout << "{";
    for (auto &item : s) {
        if (idx++) cout << ",";
        cout << item;
    }
    cout << "}";
}

// Generate all non-empty proper subsets of an itemset
vector<set<string>> getSubsets(const set<string> &itemset) {
    vector<string> items(itemset.begin(), itemset.end());
    int n = items.size();
    vector<set<string>> subsets;

    for (int size = 1; size < n; ++size) {
        vector<bool> select(n, false);
        fill(select.begin(), select.begin() + size, true);

        do {
            set<string> subset;
            for (int i = 0; i < n; ++i) {
                if (select[i]) subset.insert(items[i]);
            }
            subsets.push_back(subset);
        } while (prev_permutation(select.begin(), select.end()));
    }

    return subsets;
}

int main() {
    // --- Read transactions from CSV ---
    ifstream file("transactions.csv");
    if (!file.is_open()) {
        cout << "Error: Could not open transactions.csv\n";
        return 0;
    }

    vector<set<string>> transactions;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        set<string> transaction;
        while (getline(ss, item, ',')) {
            if (!item.empty())
                transaction.insert(item);
        }
        if (!transaction.empty())
            transactions.push_back(transaction);
    }
    file.close();

    // --- Input minimum support and confidence ---
    double minSupportInput, minConfidence;
    cout << "Enter minimum support (0-1): ";
    cin >> minSupportInput;
    cout << "Enter minimum confidence (0-1): ";
    cin >> minConfidence;

    int minCount = ceil(minSupportInput * transactions.size());
    map<int, map<set<string>, int>> frequentItemsets;

    // --- Step 1: Count all 1-itemsets ---
    for (auto &transaction : transactions) {
        for (auto &item : transaction)
            frequentItemsets[1][{item}]++;
    }

    // Remove 1-itemsets below min support
    for (auto it = frequentItemsets[1].begin(); it != frequentItemsets[1].end();) {
        if (it->second < minCount)
            it = frequentItemsets[1].erase(it);
        else
            ++it;
    }

    // --- Step 2: Generate k-itemsets (k>=2) ---
    int k = 2;
    while (!frequentItemsets[k-1].empty()) {
        vector<set<string>> prevLevel;
        for (auto &p : frequentItemsets[k-1])
            prevLevel.push_back(p.first);

        for (int i = 0; i < prevLevel.size(); ++i) {
            for (int j = i + 1; j < prevLevel.size(); ++j) {
                set<string> candidate = prevLevel[i];
                candidate.insert(prevLevel[j].begin(), prevLevel[j].end());

                if (candidate.size() == k) {
                    int supportCount = getSupportCount(candidate, transactions);
                    if (supportCount >= minCount)
                        frequentItemsets[k][candidate] = supportCount;
                }
            }
        }
        k++;
    }

    // --- Step 3: Print all frequent itemsets (support >= minSupport) ---
    cout << "\n=== Frequent Itemsets (support >= " << minSupportInput << ") ===\n";
    for (auto &level : frequentItemsets) {
        if (level.second.empty()) continue;
        for (auto &p : level.second) {
            printSet(p.first);
            cout << " -> support=" << round((double)p.second / transactions.size()*100.0)/100.0 << endl;
        }
    }

    // --- Step 4: Generate Association Rules satisfying minConfidence ---
    cout << "\n=== Association Rules (minConfidence = " << minConfidence << ") ===\n";
    for (auto &level : frequentItemsets) {
        if (level.first < 2) continue;

        for (auto &entry : level.second) {
            const set<string> &itemset = entry.first;
            int itemsetSupport = entry.second;

            auto subsets = getSubsets(itemset);
            for (auto &A : subsets) {
                set<string> B;
                for (auto &item : itemset)
                    if (A.find(item) == A.end()) B.insert(item);

                double confidence = (double)itemsetSupport / getSupportCount(A, transactions);
                if (confidence >= minConfidence) {
                    printSet(A);
                    cout << " => ";
                    printSet(B);
                    cout << " (conf=" << round(confidence*100.0)/100.0
                         << ", support=" << round((double)itemsetSupport/transactions.size()*100.0)/100.0 << ")\n";
                }
            }
        }
    }

    return 0;
}
