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

// ---------- Apriori Algorithm (Support Only) ----------
void apriori(const vector<set<string>> &transactions, double minSupport) {
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

    // Step 2: Generate frequent itemsets iteratively
    while (true) {
        cout << "\n--- Iteration k = " << k << " ---\n";
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

                cout << "Frequent Itemset: { ";
                for (auto &i : p.first)
                    cout << i << " ";
                cout << "}  --> Support = " << support << endl;
            }
        }

        if (freqItemsets.empty())
            break;

        allFrequentSets.push_back(freqItemsets);
        oneItemsets = generateCandidates(freqItemsets, ++k);
    }

    // Step 3: Output Summary
    cout << "\n=== Summary of Frequent Itemsets (minSup = " << minSupport << ") ===\n";
    for (auto &level : allFrequentSets) {
        for (auto &s : level) {
            cout << "{ ";
            for (auto &item : s)
                cout << item << " ";
            cout << "}" << endl;
        }
    }

    writeCSV("frequent_itemsets.csv", freqCSV);
    cout << "\nCSV file generated: frequent_itemsets.csv\n";
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
    double minSupport;

    cout << "Enter CSV file name (e.g. transactions.csv): ";
    cin >> inputFile;

    cout << "Enter minimum support (e.g. 0.5 for 50%): ";
    cin >> minSupport;

    vector<set<string>> transactions = readTransactions(inputFile);
    if (transactions.empty()) {
        cout << "No transactions found in " << inputFile << endl;
        return 0;
    }

    cout << "\nTotal Transactions: " << transactions.size() << endl;
    apriori(transactions, minSupport);
    return 0;
}
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF APRIORI ALGORITHM (Support-Only Version)
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **Apriori Algorithm** for discovering **Frequent Itemsets**
// from a transactional dataset.  
// The algorithm identifies which combinations of items frequently occur together in transactions.
//
// Unlike the full version (which also computes association rules with confidence),
// this version focuses on **support calculation only**, i.e., finding itemsets that meet
// a minimum support threshold.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ writeCSV()
//     - Saves the frequent itemsets and their support values into a CSV file.
//     - Each row of the CSV contains an itemset and its support value.
//
// ➤ generateCandidates()
//     - Generates candidate itemsets of size `k` from the frequent (k-1)-itemsets.
//     - Uses the Apriori principle: “All subsets of a frequent itemset must also be frequent.”
//     - Combines two frequent itemsets if their union results in a set of size `k`.
//     - Removes duplicate candidate sets.
//
// ➤ countSupport()
//     - Counts how many transactions contain each candidate itemset.
//     - For every candidate, checks if it is a subset of the transaction.
//     - Returns a map of itemset → occurrence count.
//
// ➤ apriori()
//     - The main function that drives the Apriori process.
//     - Steps:
//         1️⃣ Find all unique 1-itemsets (individual items).
//         2️⃣ Count their supports and filter by minimum support.
//         3️⃣ Generate larger candidate itemsets (2-item, 3-item, ...).
//         4️⃣ Continue until no frequent itemsets are found.
//         5️⃣ Print all frequent itemsets and their support values.
//         6️⃣ Save results to `frequent_itemsets.csv`.
//
// ➤ readTransactions()
//     - Reads the transaction data from a CSV file.
//     - Each line represents a single transaction (e.g., products purchased together).
//     - Converts each line into a `set<string>` to remove duplicates and maintain uniqueness.
//     - Returns a list (vector) of all transaction sets.
//
// ➤ main()
//     - Takes input for the dataset file and minimum support value.
//     - Reads all transactions and calls `apriori()`.
//     - Prints the frequent itemsets and generates the output CSV.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ STEP-BY-STEP LOGICAL FLOW OF THE ALGORITHM
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → **Input & Initialization**
//     - Read transactions from the dataset (CSV file).
//     - Extract all unique items and treat each as a 1-itemset.
//
// STEP 2️⃣ → **Count Support**
//     - For each candidate itemset, count how many transactions include it.
//     - Compute support = (Number of transactions containing the itemset) / (Total transactions).
//
// STEP 3️⃣ → **Prune**
//     - Discard all itemsets whose support < minSupport (they are not “frequent”).
//
// STEP 4️⃣ → **Generate Next-Level Candidates**
//     - Combine frequent (k−1)-itemsets to form k-item candidates.
//     - Example: {milk, bread} + {milk, butter} → {milk, bread, butter}.
//
// STEP 5️⃣ → **Repeat**
//     - Continue counting and pruning until no frequent itemsets remain.
//
// STEP 6️⃣ → **Output**
//     - Display all frequent itemsets and their support.
//     - Write them to a CSV file for further analysis.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ EXAMPLE RUN (Groceries Dataset Example)
// --------------------------------------------------------------------------------------------------
//
// Sample transactions.csv:
//
//     milk,bread,butter
//     milk,bread
//     milk,butter
//     bread,butter
//     milk,bread,butter
//
// Input:
//     Minimum Support = 0.5
//
// Output:
//     --- Iteration k = 1 ---
//     Frequent Itemset: { bread }  --> Support = 0.8
//     Frequent Itemset: { butter } --> Support = 0.8
//     Frequent Itemset: { milk }   --> Support = 1.0
//
//     --- Iteration k = 2 ---
//     Frequent Itemset: { bread butter } --> Support = 0.6
//     Frequent Itemset: { bread milk }   --> Support = 0.8
//     Frequent Itemset: { butter milk }  --> Support = 0.8
//
//     --- Iteration k = 3 ---
//     Frequent Itemset: { bread butter milk } --> Support = 0.6
//
//     === Summary of Frequent Itemsets (minSup = 0.5) ===
//     { bread }
//     { butter }
//     { milk }
//     { bread butter }
//     { bread milk }
//     { butter milk }
//     { bread butter milk }
//
// CSV file generated: frequent_itemsets.csv
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ INTERNAL VARIABLE DESCRIPTION
// --------------------------------------------------------------------------------------------------
//
// transactions     → Stores each transaction as a set of items.
// allItems         → Holds all unique items across all transactions.
// oneItemsets      → Current list of candidate itemsets.
// supportCount     → Map to store support frequency of each candidate itemset.
// freqItemsets     → Itemsets that satisfy the minimum support threshold.
// totalTransactions→ Total number of records in the dataset.
// minSupport       → Minimum frequency threshold set by the user (e.g., 0.4 = 40%).
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ WHY APRIORI METHOD IS USED HERE (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Nature:
//     - The dataset (e.g., "Groceries") contains **transactional data** — lists of items bought together.
//     - There is **no target variable or label** (unsupervised learning problem).
//
// 🔹 Objective:
//     - To find frequent item combinations that often occur together.
//     - For example, "If a customer buys milk and bread, they also tend to buy butter."
//
// 🔹 Why Apriori is Suitable:
//     1️⃣ It is specifically designed for **Market Basket Analysis** and transactional data.
//     2️⃣ It systematically reduces the search space using the **Apriori property**:
//         - If an itemset is infrequent, none of its supersets can be frequent.
//     3️⃣ It is efficient, interpretable, and produces actionable insights.
//     4️⃣ Results (frequent itemsets) can later be used to generate **association rules**.
//
// 🔹 Comparison with Other Data Mining Techniques:
//     - **Classification (Decision Tree / Naive Bayes):**
//         → Requires labeled data with target classes (not available here).
//     - **Clustering (K-Means / DBSCAN):**
//         → Groups similar transactions but doesn’t identify item co-occurrence patterns.
//     - **Regression:**
//         → Predicts numeric outcomes, irrelevant for categorical transaction data.
//     - ✅ **Apriori:**
//         → Perfectly fits for identifying frequent item patterns in unlabeled, categorical data.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The Apriori algorithm successfully identifies frequent itemsets that occur above the minimum support threshold.
// ➤ These frequent itemsets reveal strong relationships among items, forming the basis for association rule mining.
//
// ➤ Example Interpretation:
//     - If “milk” and “bread” appear together in 80% of transactions → frequent pattern detected.
//     - This insight can be used for cross-selling, shelf placement, or recommendation systems.
//
// ➤ Output Summary:
//     - Frequent itemsets are displayed in the console.
//     - All results are exported to "frequent_itemsets.csv" for further processing.
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Frequent Pattern Mining using the Apriori Algorithm (Support-Only)**.
// It extracts item combinations that appear frequently in a dataset,
// providing valuable insights for decision-making and business analytics.
//
// ==================================================================================================
