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
