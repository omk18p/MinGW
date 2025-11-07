#include <bits/stdc++.h>
using namespace std;

// Split CSV line by delimiter
vector<string> split(const string &s, char d) {
    vector<string> tokens; string temp; stringstream ss(s);
    while (getline(ss, temp, d)) tokens.push_back(temp);
    return tokens;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file.\n";
        return 0;
    }

    string line;
    getline(file, line);
    vector<string> headers = split(line, ',');
    int featureCount = headers.size() - 1;  // Last column = Class

    vector<vector<string>> data;
    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (row.size() == headers.size())
            data.push_back(row);
    }
    file.close();

    cout << "\n--- Dataset Loaded Successfully ---\n";
    cout << "Total records: " << data.size() << endl;
    cout << "Features: " << featureCount << endl;
    cout << "Target (Class): " << headers.back() << "\n";

    // Count class occurrences
    map<string, int> classCount;
    for (auto &r : data) classCount[r.back()]++;

    int total = data.size();
    cout << "\n=== PRIOR PROBABILITIES ===\n";
    map<string, double> prior;
    for (auto &c : classCount) {
        prior[c.first] = (double)c.second / total;
        cout << "P(" << c.first << ") = " << c.second << "/" << total
             << " = " << fixed << setprecision(3) << prior[c.first] << endl;
    }

    // Calculate conditional probabilities P(feature=value | class)
    cout << "\n=== CONDITIONAL PROBABILITIES ===\n";
    map<string, map<string, map<string, double>>> condProb;

    for (int i = 0; i < featureCount; i++) {
        map<string, map<string, int>> freq;
        for (auto &r : data) {
            freq[r.back()][r[i]]++;
        }

        for (auto &cls : classCount) {
            cout << "\nFor Class = " << cls.first
                 << " (" << headers[i] << "):\n";
            for (auto &pair : freq[cls.first]) {
                condProb[headers[i]][cls.first][pair.first] =
                    (double)pair.second / classCount[cls.first];
                cout << "P(" << pair.first << " | " << cls.first << ") = "
                     << pair.second << "/" << classCount[cls.first]
                     << " = " << condProb[headers[i]][cls.first][pair.first] << endl;
            }
        }
    }

    // Get test case input
    cout << "\n=== ENTER TEST CASE ===\n";
    map<string, string> test;
    for (int i = 0; i < featureCount; i++) {
        cout << headers[i] << ": ";
        cin >> test[headers[i]];
    }

    cout << "\n=== POSTERIOR PROBABILITIES ===\n";
    map<string, double> posterior;

    for (auto &cls : classCount) {
        double prob = prior[cls.first];
        cout << "\nFor class = " << cls.first << ":\n";
        cout << "Start with prior P(" << cls.first << ") = " << prob << endl;

        for (int i = 0; i < featureCount; i++) {
            string feat = headers[i], val = test[feat];
            double cond = condProb[feat][cls.first].count(val)
                              ? condProb[feat][cls.first][val]
                              : 0.0;
            cout << "P(" << val << " | " << cls.first << ") = " << cond << endl;
            prob *= cond;
        }

        posterior[cls.first] = prob;
        cout << "Final P(" << cls.first << " | Case) = " << prob << endl;
    }

    cout << "\n=== COMPARISON ===\n";
    for (auto &p : posterior)
        cout << "P(" << p.first << " | Case) = " << p.second << endl;

    string prediction = max_element(posterior.begin(), posterior.end(),
                                    [](auto &a, auto &b) {
                                        return a.second < b.second;
                                    })
                            ->first;

    cout << "\nPredicted Class = " << prediction << endl;

    return 0;
}
// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF NAÏVE BAYES CLASSIFIER PROGRAM
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **Naïve Bayes Classifier**, a fundamental **supervised learning**
// technique used for **classification** in Data Mining and Machine Learning.
//
// The classifier uses **Bayes’ Theorem** and assumes that all features (attributes) are conditionally
// independent given the class label.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FORMULA AND THEORY BEHIND NAÏVE BAYES
// --------------------------------------------------------------------------------------------------
//
// Bayes’ Theorem:
//       P(C | X) = [ P(X | C) * P(C) ] / P(X)
//
// In practice, since P(X) is constant for all classes, we only compare the numerators:
//       P(C | X) ∝ P(C) * Π P(Xi | C)
//
// where:
//   ▪ P(C)      = Prior probability of class C
//   ▪ P(Xi | C) = Conditional probability of feature Xi given class C
//   ▪ P(C | X)  = Posterior probability of class C given all features X
//
// The **class with the highest posterior probability** is selected as the predicted class.
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ split()
//     - Splits a line of CSV data using the comma (`,`) delimiter.
//     - Returns a vector of string tokens.
//
// ➤ main()
//     - Reads dataset from CSV file.
//     - Calculates prior probabilities P(Class).
//     - Calculates conditional probabilities P(Attribute=value | Class).
//     - Reads a test case (unseen data record).
//     - Computes posterior probabilities for each class using Naïve Bayes formula.
//     - Predicts the class with the highest posterior probability.
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ STEP-BY-STEP LOGICAL FLOW
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → INPUT DATA
//     - The program reads the dataset (CSV format).
//     - The **last column** is assumed to be the target class label.
//     - The rest of the columns are input features.
//
// STEP 2️⃣ → CALCULATE PRIOR PROBABILITIES
//     - Count how many records belong to each class.
//     - Compute: P(Class) = (Count of Class) / (Total Records)
//
// Example Output:
//     P(Yes) = 9/14 = 0.643
//     P(No)  = 5/14 = 0.357
//
// STEP 3️⃣ → CALCULATE CONDITIONAL PROBABILITIES
//     - For each attribute and each possible value, count how often it appears
//       in combination with each class label.
//     - Compute: P(Attribute=value | Class) = (Count of value in class) / (Count of class)
//
// Example Output:
//     For Class = Yes (Outlook):
//         P(Sunny | Yes) = 2/9 = 0.222
//         P(Rainy | Yes) = 3/9 = 0.333
//
//     For Class = No (Outlook):
//         P(Sunny | No) = 3/5 = 0.6
//         P(Rainy | No) = 2/5 = 0.4
//
// STEP 4️⃣ → INPUT TEST CASE
//     - User provides a new record (test case) with feature values.
//     - Example:
//         Outlook: Sunny
//         Humidity: High
//         Windy: False
//
// STEP 5️⃣ → COMPUTE POSTERIOR PROBABILITIES FOR EACH CLASS
//     - For each class C:
//           Posterior(C) = P(C) × Π P(Attribute=value | C)
//
// Example Computation:
//
//     For Class = Yes:
//         P(Yes) × P(Sunny | Yes) × P(High | Yes) × P(False | Yes)
//         = 0.643 × 0.222 × 0.444 × 0.667 = 0.0426
//
//     For Class = No:
//         P(No) × P(Sunny | No) × P(High | No) × P(False | No)
//         = 0.357 × 0.6 × 0.4 × 0.5 = 0.0428
//
// STEP 6️⃣ → SELECT CLASS WITH MAXIMUM POSTERIOR
//     - Compare posterior probabilities and pick the higher one.
//
//     Example Output:
//         P(Yes | Case) = 0.0426
//         P(No | Case) = 0.0428
//         Predicted Class = No
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ SAMPLE OUTPUT FORMAT
// --------------------------------------------------------------------------------------------------
//
// --- Dataset Loaded Successfully ---
// Total records: 14
// Features: 4
// Target (Class): Play
//
// === PRIOR PROBABILITIES ===
// P(Yes) = 9/14 = 0.643
// P(No)  = 5/14 = 0.357
//
// === CONDITIONAL PROBABILITIES ===
// For Class = Yes (Outlook):
// P(Sunny | Yes) = 2/9 = 0.222
// P(Overcast | Yes) = 4/9 = 0.444
// P(Rainy | Yes) = 3/9 = 0.333
//
// For Class = No (Outlook):
// P(Sunny | No) = 3/5 = 0.6
// P(Overcast | No) = 0/5 = 0
// P(Rainy | No) = 2/5 = 0.4
//
// === ENTER TEST CASE ===
// Outlook: Sunny
// Temperature: Hot
// Humidity: High
// Windy: False
//
// === POSTERIOR PROBABILITIES ===
// P(Yes | Case) = 0.0426
// P(No | Case)  = 0.0428
//
// Predicted Class = No
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ INTERNAL VARIABLES USED
// --------------------------------------------------------------------------------------------------
//
// filename          → Input dataset file.
// headers[]         → Attribute names (first row of CSV).
// data[][]          → All dataset rows (vector of vectors).
// classCount{}      → Count of each target class (used for prior probabilities).
// condProb{}        → Nested map for storing conditional probabilities P(Attribute=value | Class).
// test{}            → Stores user input for test record.
// posterior{}       → Stores computed posterior probabilities for each class.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ WHY NAÏVE BAYES CLASSIFIER WAS USED (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Type:
//     - The dataset contains **categorical attributes** (e.g., Outlook, Windy, etc.)
//       and a **categorical target label** (e.g., Play = Yes/No).
//
// 🔹 Objective:
//     - To **classify unseen test cases** into one of the known classes based on probabilities.
//
// 🔹 Why Naïve Bayes is Suitable:
//     1️⃣ Works efficiently with categorical data.
//     2️⃣ Based on solid probabilistic foundations (Bayes’ Theorem).
//     3️⃣ Requires small amounts of training data.
//     4️⃣ Simple to implement and interpret.
//     5️⃣ Performs surprisingly well even with the “independence” assumption.
//
// 🔹 Comparison with Other Methods:
//     - **Decision Tree (ID3/CART):** More complex, requires entropy or Gini calculations.
//     - **K-Means / DBSCAN:** Unsupervised; do not perform classification.
//     - ✅ **Naïve Bayes:** Perfect for small, labeled, categorical datasets for classification.
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ ADVANTAGES
// --------------------------------------------------------------------------------------------------
//
// ✅ Fast, simple, and efficient — works well with large datasets.
// ✅ Performs well even with small amounts of training data.
// ✅ Handles multiple categorical features easily.
// ✅ Requires no parameter tuning (unlike KNN or SVM).
//
// --------------------------------------------------------------------------------------------------
// 🔸 8️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Assumes independence between features — often unrealistic in real-world data.
// ⚠️ Zero probability problem (if an unseen attribute value occurs) —
//     can be solved using **Laplace smoothing** (not implemented here).
// ⚠️ Works best with categorical data (numeric data requires discretization or Gaussian NB).
//
// --------------------------------------------------------------------------------------------------
// 🔸 9️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The Naïve Bayes Classifier successfully predicts the class of unseen instances
//     using probabilities derived from training data.
//
// ➤ It calculates:
//     - Prior probabilities of each class (P(C))
//     - Conditional probabilities of features given class (P(Xi | C))
//     - Posterior probabilities for each class (P(C | X))
//
// ➤ The class with the **maximum posterior probability** is selected as the prediction.
//
// Example Conclusion Statement:
//     “Based on Naïve Bayes computation, the test case was classified as ‘No’
//      since it had the highest posterior probability.”
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Classification using the Naïve Bayes Algorithm**,
// which applies Bayes’ Theorem with conditional independence assumptions.
// It is a simple yet powerful probabilistic model widely used in spam filtering,
// medical diagnosis, and text classification.
//
// ==================================================================================================
