#include<bits/stdc++.h>
using namespace std;

int main() {
    // --- Step 1: Read CSV file ---
    ifstream file("data.csv");
    if (!file.is_open()) {
        cout << "Error: Unable to open data.csv!" << endl;
        return 0;
    }

    string line;
    getline(file, line); // header
    stringstream header(line);
    vector<string> headers;
    string col;
    while (getline(header, col, ',')) headers.push_back(col);

    int nAttr = headers.size() - 1;    // last column is class
    string classCol = headers.back();

    vector<vector<string>> data;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        string value;
        while (getline(ss, value, ',')) row.push_back(value);
        if (!row.empty()) data.push_back(row);
    }
    file.close();

    int totalRecords = data.size();

    // --- Step 2: Identify all possible classes ---
    vector<string> classes;
    for (auto &row : data) {
        string cls = row[nAttr];
        if (find(classes.begin(), classes.end(), cls) == classes.end())
            classes.push_back(cls);
    }

    // --- Step 3: Input new record ---
    vector<string> newRecord(nAttr);
    cout << "\nEnter new record values:\n";
    for (int i = 0; i < nAttr; i++) {
        cout << headers[i] << ": ";
        cin >> newRecord[i];
    }

    // --- Step 4: Compute posterior probabilities ---
    vector<double> posterior(classes.size(), 0.0);
    cout << fixed << setprecision(4);

    cout << "\n--- Step-by-step Posterior Probabilities ---\n";
    for (int c = 0; c < classes.size(); c++) {
        string classVal = classes[c];
        int classCount = 0;

        // Count how many records belong to this class
        for (auto &row : data)
            if (row[nAttr] == classVal)
                classCount++;

        double prior = (double)classCount / totalRecords;
        double prob = prior;

        cout << "\nClass '" << classVal << "':\n";
        cout << "Prior Probability P(" << classVal << ") = " << prior << endl;

        // Multiply conditional probabilities for each attribute
        for (int j = 0; j < nAttr; j++) {
            int match = 0;
            for (auto &row : data) {
                if (row[nAttr] == classVal && row[j] == newRecord[j])
                    match++;
            }

            double condProb = 0.0;
            if (classCount > 0)
                condProb = (double)match / classCount;

            cout << "P(" << headers[j] << "=" << newRecord[j] 
                 << " | " << classVal << ") = " << condProb << endl;

            prob *= condProb;
        }

        posterior[c] = prob;
        cout << "Posterior Probability for class " << classVal 
             << " = " << prob << endl;
    }

    // --- Step 5: Choose class with maximum posterior ---
    double maxProb = -1;
    string predicted;
    for (int c = 0; c < classes.size(); c++) {
        if (posterior[c] > maxProb) {
            maxProb = posterior[c];
            predicted = classes[c];
        }
    }

    // --- Step 6: Display final prediction ---
    cout << "\nPredicted Class: " << predicted << endl;
    cout << "\n--- Conclusion ---\n";
    cout << "The new record is most likely to belong to class '" 
         << predicted << "' based on Naive Bayes classification.\n";

    return 0;
}
