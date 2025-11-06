#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <string>
using namespace std;

// ---------- CSV Writer ----------
void writeCSV(const string &filename, const vector<vector<string>> &rows) {
    ofstream file(filename);
    for (auto &row : rows) {
        for (size_t i = 0; i < row.size(); i++) {
            file << row[i];
            if (i != row.size() - 1)
                file << ",";
        }
        file << "\n";
    }
    file.close();
}

// ---------- Candidate Generator ----------
vector<set<string>> generateCandidates(const vector<set<string>> &prevFreq, int k) {
    vector<set<string>> candidates;
    for (size_t i = 0; i < prevFreq.size(); i++) {
        for (size_t j = i + 1; j < prevFreq.size(); j++) {
            set<string> c;
            set_union(prevFreq[i].begin(), prevFreq[i].end(),
                      prevFreq[j].begin(), prevFreq[j].end(),
                      inserter(c, c.begin()));
            if ((int)c.size() == k)
                candidates.push_back(c);
        }
    }
    sort(candidates.begin(), candidates.end());
    candidates.erase(unique(candidates.begin(), candidates.end()), candidates.end());
    return candidates;
}

// ---------- Support Counter ----------
map<set<string>, int> countSupport(const vector<set<string>> &transactions,
                                   const vector<set<string>> &candidates) {
    map<set<string>, int> supportCount;
    for (auto &c : candidates) {
        for (auto &t : transactions) {
            if (includes(t.begin(), t.end(), c.begin(), c.end())) {
                supportCount[c]++;
            }
        }
    }
    return supportCount;
}

// ---------- Apriori Algorithm ----------
void apriori(const vector<set<string>> &transactions, double minSupport, double minConfidence) {
    int totalTransactions = transactions.size();
    vector<set<string>> oneItemsets;
    set<string> allItems;

    // Step 1: Collect all unique items
    for (auto &t : transactions)
        for (auto &item : t)
            allItems.insert(item);

    for (auto &item : allItems)
        oneItemsets.push_back({item});

    vector<vector<set<string>>> allFrequentSets;
    int k = 1;

    vector<vector<string>> freqCSV = {{"Itemset", "Support"}};
    vector<vector<string>> rulesCSV = {{"Antecedent", "Consequent", "Support", "Confidence"}};

    // Step 2: Generate frequent itemsets iteratively
    while (true) {
        map<set<string>, int> supportCount = countSupport(transactions, oneItemsets);

        vector<set<string>> freqItemsets;
        for (auto &p : supportCount) {
            double support = (double)p.second / totalTransactions;
            if (support >= minSupport) {
                freqItemsets.push_back(p.first);

                string items = "";
                for (auto &i : p.first)
                    items += i + " ";
                freqCSV.push_back({items, to_string(support)});
            }
        }

        if (freqItemsets.empty())
            break;

        allFrequentSets.push_back(freqItemsets);
        oneItemsets = generateCandidates(freqItemsets, ++k);
    }

    // Step 3: Print Frequent Itemsets
    cout << "\n=== Frequent Itemsets (minSup=" << minSupport << ") ===\n";
    for (auto &level : allFrequentSets) {
        for (auto &s : level) {
            for (auto &item : s)
                cout << item << " ";
            cout << endl;
        }
    }

    // Step 4: Generate Association Rules
    cout << "\n=== Association Rules (minConf=" << minConfidence << ") ===\n";
    for (auto &level : allFrequentSets) {
        for (auto &itemset : level) {
            if (itemset.size() < 2)
                continue;

            vector<string> items(itemset.begin(), itemset.end());
            int n = items.size();

            for (int mask = 1; mask < (1 << n) - 1; mask++) {
                set<string> antecedent, consequent;
                for (int i = 0; i < n; i++) {
                    if (mask & (1 << i))
                        antecedent.insert(items[i]);
                    else
                        consequent.insert(items[i]);
                }

                // Compute supports
                map<set<string>, int> supCount = countSupport(transactions, {itemset, antecedent});
                double supportItemset = (double)supCount[itemset] / totalTransactions;
                double supportAntecedent = (double)supCount[antecedent] / totalTransactions;

                if (supportAntecedent == 0)
                    continue;

                double confidence = supportItemset / supportAntecedent;

                if (supportItemset >= minSupport && confidence >= minConfidence) {
                    cout << "{ ";
                    for (auto &a : antecedent)
                        cout << a << " ";
                    cout << "} => { ";
                    for (auto &c : consequent)
                        cout << c << " ";
                    cout << "} (support=" << supportItemset
                         << ", confidence=" << confidence << ")\n";

                    string ant = "", con = "";
                    for (auto &a : antecedent)
                        ant += a + " ";
                    for (auto &c : consequent)
                        con += c + " ";
                    rulesCSV.push_back({ant, con,
                                        to_string(supportItemset),
                                        to_string(confidence)});
                }
            }
        }
    }

    // Step 5: Write results to CSV files
    writeCSV("frequent_itemsets.csv", freqCSV);
    writeCSV("association_rules.csv", rulesCSV);

    cout << "\nCSV files generated: frequent_itemsets.csv, association_rules.csv\n";
}

// ---------- Read Transactions from CSV ----------
vector<set<string>> readTransactions(const string &filename) {
    vector<set<string>> transactions;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        set<string> transaction;
        stringstream ss(line);
        string item;
        while (getline(ss, item, ',')) {
            if (!item.empty()) {
                // Remove leading/trailing spaces
                item.erase(remove_if(item.begin(), item.end(), ::isspace), item.end());
                transaction.insert(item);
            }
        }
        if (!transaction.empty())
            transactions.push_back(transaction);
    }
    return transactions;
}

// ---------- Main ----------
int main() {
    string inputFile;
    double minSupport, minConfidence;

    cout << "Enter CSV file name (e.g. transactions.csv): ";
    cin >> inputFile;

    cout << "Enter minimum support (e.g. 0.5 for 50%): ";
    cin >> minSupport;

    cout << "Enter minimum confidence (e.g. 0.7 for 70%): ";
    cin >> minConfidence;

    vector<set<string>> transactions = readTransactions(inputFile);
    if (transactions.empty()) {
        cout << "No transactions found in " << inputFile << endl;
        return 0;
    }

    apriori(transactions, minSupport, minConfidence);
    return 0;
}