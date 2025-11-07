#include <bits/stdc++.h>
using namespace std;

// ---------- Gaussian Probability Function ----------
double gaussianPDF(double x, double mean, double sd) {
    if (sd == 0) return 1e-9;
    double exponent = exp(-pow(x - mean, 2) / (2 * pow(sd, 2)));
    return (1 / (sqrt(2 * M_PI) * sd)) * exponent;
}

// ---------- Utility: Split CSV ----------
vector<string> split(const string &s, char d) {
    vector<string> t; string x; stringstream ss(s);
    while (getline(ss, x, d)) {
        if (!x.empty() && x.front() == ' ') x.erase(x.begin());
        if (!x.empty() && x.back() == ' ') x.pop_back();
        t.push_back(x);
    }
    return t;
}

// ---------- Detect if feature is numeric ----------
bool isNumeric(const string &s) {
    if (s.empty()) return false;
    return all_of(s.begin(), s.end(), [](unsigned char c) {
        return isdigit(c) || c == '.' || c == '-';
    });
}

// ---------- Read CSV ----------
vector<vector<string>> readCSV(const string &filename) {
    ifstream file(filename);
    vector<vector<string>> data;
    string line;
    while (getline(file, line)) {
        if (!line.empty()) data.push_back(split(line, ','));
    }
    return data;
}

// ---------- Main ----------
int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    vector<vector<string>> data = readCSV(filename);
    if (data.empty()) {
        cout << "Error: File empty or invalid!\n";
        return 0;
    }

    vector<string> headers = data[0];
    int featureCount = headers.size() - 1;
    cout << "\n--- Dataset Loaded ---\n";
    cout << "Records: " << data.size() - 1 << "\nFeatures: " << featureCount
         << "\nTarget: " << headers.back() << "\n";

    // ---------- Determine which columns are numeric ----------
    vector<bool> isNum(featureCount, false);
    for (int j = 0; j < featureCount; j++) {
        for (int i = 1; i < data.size(); i++) {
            if (isNumeric(data[i][j])) {
                isNum[j] = true;
                break;
            }
        }
    }

    // ---------- Split data by class ----------
    map<string, vector<vector<string>>> classData;
    for (int i = 1; i < data.size(); i++)
        classData[data[i].back()].push_back(data[i]);

    int total = data.size() - 1;

    // ---------- Step 1: Priors ----------
    cout << "\n=== PRIOR PROBABILITIES ===\n";
    map<string, double> priors;
    for (auto &cls : classData) {
        priors[cls.first] = (double)cls.second.size() / total;
        cout << "P(" << cls.first << ") = " << cls.second.size()
             << "/" << total << " = " << priors[cls.first] << endl;
    }

    // ---------- Step 2: Compute Class Statistics ----------
    cout << "\n=== CLASS STATISTICS ===\n";
    map<string, vector<double>> meanVals, sdVals;
    map<string, map<string, map<string, double>>> catProb;

    for (auto &cls : classData) {
        cout << "\nClass: " << cls.first << endl;

        // Numeric stats
        vector<double> mean(featureCount, 0.0), sd(featureCount, 0.0);
        int n = cls.second.size();

        for (int j = 0; j < featureCount; j++) {
            if (isNum[j]) {
                // compute mean
                for (auto &row : cls.second) mean[j] += stod(row[j]);
                mean[j] /= n;

                // compute std dev
                for (auto &row : cls.second)
                    sd[j] += pow(stod(row[j]) - mean[j], 2);
                sd[j] = sqrt(sd[j] / n);

                cout << headers[j] << " -> Mean=" << mean[j]
                     << ", StdDev=" << sd[j] << endl;
            } else {
                // categorical
                map<string, int> freq;
                for (auto &row : cls.second)
                    freq[row[j]]++;
                for (auto &f : freq)
                    catProb[headers[j]][cls.first][f.first] =
                        (double)f.second / n;

                cout << headers[j] << " (Categorical): ";
                for (auto &f : freq)
                    cout << f.first << "="
                         << catProb[headers[j]][cls.first][f.first] << " ";
                cout << endl;
            }
        }
        meanVals[cls.first] = mean;
        sdVals[cls.first] = sd;
    }

    // ---------- Step 3: Get Test Input ----------
    cout << "\n=== ENTER TEST DATA ===\n";
    vector<string> test(featureCount);
    for (int j = 0; j < featureCount; j++) {
        cout << headers[j] << ": ";
        cin >> test[j];
    }

    // ---------- Step 4: Calculate Posterior ----------
    cout << "\n=== POSTERIOR PROBABILITIES ===\n";
    map<string, double> post;

    for (auto &cls : classData) {
        double prob = priors[cls.first];
        cout << "\nClass = " << cls.first << ":\n";

        for (int j = 0; j < featureCount; j++) {
            if (isNum[j]) {
                double val = stod(test[j]);
                double g = gaussianPDF(val, meanVals[cls.first][j], sdVals[cls.first][j]);
                cout << "  P(" << headers[j] << "=" << val << " | " << cls.first << ") = " << g << endl;
                prob *= g;
            } else {
                double cond = catProb[headers[j]][cls.first].count(test[j]) ?
                              catProb[headers[j]][cls.first][test[j]] : 1e-6;
                cout << "  P(" << headers[j] << "=" << test[j] << " | " << cls.first << ") = " << cond << endl;
                prob *= cond;
            }
        }
        post[cls.first] = prob;
        cout << "  => Combined Probability = " << prob << endl;
    }

    // ---------- Step 5: Choose Best Class ----------
    string bestClass = max_element(post.begin(), post.end(),
                                   [](auto &a, auto &b) { return a.second < b.second; })
                           ->first;

    cout << "\n=== FINAL RESULTS ===\n";
    for (auto &p : post)
        cout << "P(" << p.first << " | Test) = " << p.second << endl;

    cout << "\nPredicted Class = " << bestClass << endl;
    return 0;
}



// ==================================================================================================
// 🔹 DETAILED EXPLANATION OF GAUSSIAN NAÏVE BAYES CLASSIFIER (FOR MIXED NUMERIC & CATEGORICAL DATA)
// ==================================================================================================
//
// 🧩 PURPOSE:
// This program implements the **Gaussian Naïve Bayes Algorithm**, which extends the basic Naïve Bayes
// classifier to handle **both numerical and categorical attributes**.
//
// For categorical features → uses **frequency-based probabilities**.
// For numerical features → assumes data follows a **Gaussian (normal) distribution** and computes
// conditional probability using the **Gaussian Probability Density Function (PDF)**.
//
// --------------------------------------------------------------------------------------------------
// 🔸 1️⃣ FUNCTION OVERVIEW
// --------------------------------------------------------------------------------------------------
//
// ➤ gaussianPDF()
//     - Computes the **probability density** of a numeric value ‘x’ for a given mean and standard deviation.
//     - Formula:
//           P(x | class) = (1 / (√(2π) * σ)) * exp(−(x−μ)² / (2σ²))
//       where μ = mean, σ = standard deviation
//     - Represents how likely a numeric value ‘x’ is under the class’s distribution.
//
// ➤ split()
//     - Splits each CSV line into comma-separated tokens.
//     - Removes extra spaces and returns a vector of strings.
//
// ➤ isNumeric()
//     - Determines whether a string value is numeric (used to identify numeric columns).
//
// ➤ readCSV()
//     - Reads the dataset file (CSV format) and stores all rows as string vectors.
//
// ➤ main()
//     - Controls the workflow:
//         1️⃣ Loads dataset
//         2️⃣ Identifies numeric/categorical columns
//         3️⃣ Computes prior probabilities P(Class)
//         4️⃣ Computes feature statistics (mean, standard deviation) for numeric features
//         5️⃣ Computes frequency-based probabilities for categorical features
//         6️⃣ Accepts a test record from the user
//         7️⃣ Calculates posterior probabilities using Bayes’ theorem
//         8️⃣ Predicts the class with the highest posterior probability
//
// --------------------------------------------------------------------------------------------------
// 🔸 2️⃣ STEP-BY-STEP WORKING LOGIC
// --------------------------------------------------------------------------------------------------
//
// STEP 1️⃣ → DATA LOADING
//     - The program reads the dataset (CSV file).
//     - The **first row** is treated as headers (feature names).
//     - The **last column** is considered the target (class label).
//
// Example:
//     Age, Income, Student, Credit, Buys
//     25, High, No, Fair, No
//     35, Medium, Yes, Excellent, Yes
//
// STEP 2️⃣ → CLASS SEPARATION
//     - Records are divided based on their class labels (e.g., Yes, No).
//     - For each class, all rows belonging to it are grouped for statistics computation.
//
// STEP 3️⃣ → PRIOR PROBABILITIES
//     - Calculates probability of each class in the dataset.
//     - Formula:
//           P(Class) = (Count of records in Class) / (Total records)
//
// Example Output:
//     P(Yes) = 9/14 = 0.643
//     P(No)  = 5/14 = 0.357
//
// STEP 4️⃣ → FEATURE STATISTICS
//     - For each class, compute the following for every numeric attribute:
//         ▪ Mean (μ)
//         ▪ Standard Deviation (σ)
//     - For categorical attributes, calculate relative frequency:
//           P(Value | Class) = (Count of value in Class) / (Total in Class)
//
// Example Output:
//     Class: Yes
//         Age -> Mean=30.6, StdDev=4.7
//         Student (Categorical): Yes=0.7 No=0.3
//
// STEP 5️⃣ → TEST RECORD INPUT
//     - The user provides a test record with all feature values (numeric + categorical).
//     - Example:
//           Age: 28
//           Income: High
//           Student: Yes
//           Credit: Fair
//
// STEP 6️⃣ → POSTERIOR PROBABILITY CALCULATION
//     - For each class, compute:
//           P(Class | Test) ∝ P(Class) × Π P(FeatureValue | Class)
//
//     - For numeric features → use Gaussian PDF with mean and stddev of the class.
//     - For categorical features → use frequency-based conditional probabilities.
//     - Multiply all probabilities (small values) to get combined likelihood for the class.
//
// Example Computation:
//
//     For Class = Yes:
//         P(Yes) × P(Age=28|Yes) × P(Income=High|Yes) × P(Student=Yes|Yes) × P(Credit=Fair|Yes)
//
//     For Class = No:
//         P(No) × P(Age=28|No) × P(Income=High|No) × P(Student=Yes|No) × P(Credit=Fair|No)
//
// STEP 7️⃣ → CLASS SELECTION
//     - Compare posterior probabilities of all classes.
//     - The class with the **highest posterior probability** is chosen as the prediction.
//
// Example Output:
//     P(Yes | Test) = 0.000026
//     P(No | Test)  = 0.000011
//     Predicted Class = Yes
//
// --------------------------------------------------------------------------------------------------
// 🔸 3️⃣ INTERNAL VARIABLES USED
// --------------------------------------------------------------------------------------------------
//
// data[][]           → Complete dataset read from CSV file.
// headers[]          → Column names (first row of the dataset).
// featureCount       → Number of features (excluding class column).
// isNum[]            → Boolean vector indicating which columns are numeric.
// classData{}        → Map storing rows grouped by class label.
// priors{}           → Prior probabilities of each class P(Class).
// meanVals{}, sdVals{} → Numeric mean and standard deviation per class and feature.
// catProb{}          → Conditional probabilities for categorical features.
// post{}             → Final posterior probabilities for each class.
// test[]             → User-provided test record values.
//
// --------------------------------------------------------------------------------------------------
// 🔸 4️⃣ WHY GAUSSIAN NAÏVE BAYES WAS CHOSEN (JUSTIFICATION)
// --------------------------------------------------------------------------------------------------
//
// 🔹 Dataset Nature:
//     - The dataset contains **both numerical and categorical** attributes.
//     - Requires a classifier capable of handling both types simultaneously.
//
// 🔹 Objective:
//     - To classify unseen records based on training data using probabilistic inference.
//
// 🔹 Why Gaussian Naïve Bayes is Ideal:
//     1️⃣ Extends Naïve Bayes to handle **continuous numeric features** using the Gaussian distribution.
//     2️⃣ Efficient for real-valued data and mixed attribute types.
//     3️⃣ Works well with small datasets and simple probabilistic assumptions.
//     4️⃣ Provides interpretable results (via probabilities).
//
// 🔹 Comparison with Other Methods:
//     - **Standard Naïve Bayes:** Only for categorical data.
//     - **Decision Tree (ID3/CART):** Requires splitting and entropy/gini calculations.
//     - **K-Means / DBSCAN:** Unsupervised (no class labels).
//     - ✅ **Gaussian NB:** Handles numeric + categorical classification effectively.
//
// --------------------------------------------------------------------------------------------------
// 🔸 5️⃣ ADVANTAGES
// --------------------------------------------------------------------------------------------------
//
// ✅ Handles mixed data types (numeric & categorical).
// ✅ Computationally efficient and simple to implement.
// ✅ Works well even with small datasets.
// ✅ Provides probabilistic confidence values for predictions.
//
// --------------------------------------------------------------------------------------------------
// 🔸 6️⃣ LIMITATIONS
// --------------------------------------------------------------------------------------------------
//
// ⚠️ Assumes feature independence given the class (rarely true in practice).
// ⚠️ Sensitive to zero-frequency problems (solved here with smoothing = 1e−6).
// ⚠️ Assumes normal distribution for numeric features (may not always hold true).
//
// --------------------------------------------------------------------------------------------------
// 🔸 7️⃣ CONCLUSION
// --------------------------------------------------------------------------------------------------
//
// ➤ The Gaussian Naïve Bayes classifier calculates both **prior** and **conditional probabilities**
//     (via Gaussian PDFs for numeric data) and multiplies them to find posterior probabilities.
//
// ➤ The class with the **maximum posterior probability** is chosen as the final prediction.
//
// ➤ Example Conclusion:
//     “Based on the Gaussian Naïve Bayes model, the test case was classified as ‘Yes’
//      with the highest posterior probability among all classes.”
//
// ➤ Summary of Advantages Observed:
//     - Supports both numeric and categorical data.
//     - Probabilistic, interpretable, and mathematically sound.
//     - Provides fast and accurate classification.
//
// --------------------------------------------------------------------------------------------------
// ✅ FINAL REMARK:
// This experiment demonstrates **Classification using the Gaussian Naïve Bayes Algorithm**, 
// a hybrid probabilistic model capable of handling mixed-type datasets efficiently.
// It is widely used in spam detection, medical diagnosis, and text classification tasks.
//
// ==================================================================================================

