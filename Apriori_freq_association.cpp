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

// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF THE APRIORI CODE (STEP-BY-STEP WITH INTERNAL LOGIC)
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **Apriori Algorithm**, one of the most important techniques
// in Data Mining for **Association Rule Mining (ARM)**.
// It’s used to find frequent item combinations and strong relationships between items
// — typically applied in **Market Basket Analysis** (e.g., analyzing which products are often bought together).
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 1: HEADER FILES & SETUP
// --------------------------------------------------------------------------------------------------
//
// → #include directives bring standard C++ libraries for file handling, string manipulation, and data structures.
// → STL containers like `vector`, `set`, and `map` are heavily used for storing transactions and counting support.
//
//    - vector<vector<string>> → 2D structure to hold CSV rows
//    - set<string> → used for each transaction (automatically removes duplicates)
//    - map<set<string>, int> → tracks how many times each itemset appears (support count)
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 2: writeCSV()
// --------------------------------------------------------------------------------------------------
//
// • Purpose: Save results (frequent itemsets and rules) into separate .csv files for later inspection.
// • It loops through each “row” (vector<string>) and writes values separated by commas.
// • Example Output Files:
//     - frequent_itemsets.csv → Itemset + Support value
//     - association_rules.csv → Antecedent + Consequent + Support + Confidence
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 3: generateCandidates()
// --------------------------------------------------------------------------------------------------
//
// • Core idea: Combine frequent itemsets of size (k−1) to generate candidates of size k.
// • Uses `set_union()` to merge two sets if their combined size == k.
// • Example: If {milk, bread} and {milk, butter} are frequent (2-itemsets),
//   they generate candidate {milk, bread, butter} (3-itemset).
// • After generating all candidates, duplicates are removed using sort() and unique().
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 4: countSupport()
// --------------------------------------------------------------------------------------------------
//
// • Purpose: Count how often each candidate itemset appears in all transactions.
// • For each candidate set, it checks whether the transaction contains all candidate items using `includes()`.
// • Support count is stored in a map: `map<set<string>, int> supportCount`.
// • Example: if {milk, bread} appears in 3 of 5 transactions → supportCount[{milk,bread}] = 3.
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 5: apriori()
// --------------------------------------------------------------------------------------------------
//
// This function implements the **entire Apriori process**:
//
//   Step 1️⃣ — Extract all unique items from dataset.
//       - A set of all single items is created → initial 1-itemsets.
//
//   Step 2️⃣ — Count support for 1-itemsets.
//       - Using countSupport(), support = occurrences / total_transactions.
//       - Only items with support ≥ minSupport are kept as “frequent”.
//
//   Step 3️⃣ — Generate next-level itemsets.
//       - Using generateCandidates(), create 2-item, 3-item, … sets.
//       - Repeat support counting and pruning until no new frequent itemsets appear.
//
//   Step 4️⃣ — Print and store frequent itemsets.
//       - Each frequent itemset and its support are printed on screen
//         and also written into `frequent_itemsets.csv`.
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 6: GENERATING ASSOCIATION RULES
// --------------------------------------------------------------------------------------------------
//
// • Once all frequent itemsets are known, the algorithm creates rules of the form:
//       Antecedent → Consequent
//
//   Example: {Milk, Bread} → {Butter}
//
// • For every frequent itemset with at least 2 items, it generates all possible
//   combinations of antecedent and consequent by binary masking (`mask` loop).
//
// • Then it calculates:
//     - support(Itemset) = count(Itemset) / totalTransactions
//     - support(Antecedent) = count(Antecedent) / totalTransactions
//     - confidence = support(Itemset) / support(Antecedent)
//
// • Rules that satisfy:
//       support ≥ minSupport  AND  confidence ≥ minConfidence
//   are considered **strong rules** and displayed.
//
// • Example rule in output:
//       { Milk } => { Bread } (support=0.6, confidence=0.8)
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 7: OUTPUT & CSV STORAGE
// --------------------------------------------------------------------------------------------------
//
// • The algorithm prints all frequent itemsets and rules on the console for verification.
// • Then it writes them into two CSV files:
//     1️⃣ frequent_itemsets.csv
//     2️⃣ association_rules.csv
//
// • These files can later be visualized in Excel or any data visualization tool.
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 8: readTransactions()
// --------------------------------------------------------------------------------------------------
//
// • Reads the CSV input file line by line.
// • Each line represents a transaction (like a shopping basket).
// • The function splits the line by commas, trims spaces, and inserts each item into a set.
// • Example input line:  “milk,bread,butter”  →  transaction = {milk, bread, butter}
// • Finally returns a vector of all transactions.
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 9: main()
// --------------------------------------------------------------------------------------------------
//
// • Takes user inputs:
//       - File name (e.g., “groceries.csv”)
//       - Minimum support (e.g., 0.3 → 30%)
//       - Minimum confidence (e.g., 0.6 → 60%)
//
// • Calls readTransactions() → loads data
// • Calls apriori() → runs algorithm
// • If no data is found, exits gracefully.
//
// --------------------------------------------------------------------------------------------------
// 🔸 PART 10: SAMPLE RUN
// --------------------------------------------------------------------------------------------------
//
// Input:
//     transactions.csv
//     minSupport = 0.3
//     minConfidence = 0.7
//
// Output:
//     === Frequent Itemsets (minSup=0.3) ===
//     milk
//     bread
//     milk bread
//
//     === Association Rules (minConf=0.7) ===
//     { milk } => { bread } (support=0.6, confidence=0.8)
//
// Generated Files:
//     frequent_itemsets.csv
//     association_rules.csv
//
// --------------------------------------------------------------------------------------------------
// 🔸 INTERNAL LOGIC SUMMARY
// --------------------------------------------------------------------------------------------------
//
// The Apriori algorithm follows the **“bottom-up”** approach:
//
//     - Start with 1-itemsets
//     - Use them to build 2-itemsets
//     - Continue until no larger frequent itemsets exist
//
// It uses the **Apriori property**:
//     → If an itemset is frequent, all its subsets are also frequent.
//     → If an itemset is not frequent, none of its supersets will be frequent.
//
// This property helps in **pruning** unnecessary candidate itemsets,
// making the algorithm efficient.
//
// --------------------------------------------------------------------------------------------------
// 🔹 WHY APRIORI METHOD IS CHOSEN FOR THIS CODE
// --------------------------------------------------------------------------------------------------
//
// 1️⃣ Data Type Suitability:
//     - The dataset (like Groceries) is transactional: each record is a set of items.
//     - No class label or numeric target → classification/regression not applicable.
//     - We only need to find relationships between co-occurring items.
//
// 2️⃣ Purpose of Analysis:
//     - Identify patterns such as:
//           “If a customer buys milk and butter, they are likely to buy bread.”
//     - This is **Market Basket Analysis**, and Apriori is the standard technique for it.
//
// 3️⃣ Advantages of Apriori:
//     - Simple and interpretable algorithm.
//     - Efficiently eliminates infrequent itemsets using its pruning property.
//     - Generates meaningful rules that can be used for recommendations.
//
// 4️⃣ Why Not Other Methods:
//     - **Decision Trees / Naive Bayes:** Need a predefined target (not available here).
//     - **Clustering:** Groups data, but doesn’t show item dependencies.
//     - **Regression:** Predicts numeric values, not relationships.
//
// ✅ Hence, Apriori perfectly fits for transactional pattern discovery.
//
// --------------------------------------------------------------------------------------------------
// 🔹 CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// • The Apriori algorithm successfully finds all frequent item combinations
//   that meet the minimum support threshold.
//
// • It then derives strong association rules that satisfy both support and confidence constraints.
//
// • These rules can be interpreted as *if–then* statements useful in business intelligence,
//   customer behavior analysis, or recommendation systems.
//
// • Example Insight:
//       "If customers buy butter and milk, 80% of them also buy bread."
//
// • The generated CSV files can be used for visualization or integration with other analytics tools.
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Association Rule Mining using Apriori** —
// one of the most fundamental and interpretable techniques in Data Mining.
// It effectively uncovers hidden patterns from transactional data without requiring labeled outputs.
//
// ==================================================================================================
