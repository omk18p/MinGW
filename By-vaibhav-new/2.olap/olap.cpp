#include <bits/stdc++.h>
using namespace std;

// Read CSV file
vector<map<string, string>> readCSV(const string& fileName, vector<string>& header) {
    vector<map<string, string>> records;
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return records;
    }
    string line;
    if (!getline(file, line)) return records;
    stringstream ss(line);
    string col;
    while (getline(ss, col, ',')) header.push_back(col);

    while (getline(file, line)) {
        stringstream row(line);
        string value;
        map<string, string> record;
        for (int i = 0; i < header.size(); i++) {
            getline(row, value, ',');
            record[header[i]] = value;
        }
        records.push_back(record);
    }
    return records;
}

// Print record
void printRecord(const map<string, string>& r, const vector<string>& header) {
    for (const string& h : header)
        cout << r.at(h) << " | ";
    cout << endl;
}

// ✅ Renamed from 'slice' to 'sliceData'
void sliceData(const vector<map<string, string>>& data, const string& field,
               const string& value, const vector<string>& header) {
    cout << "\nSlice: " << field << " = " << value << endl;
    for (const auto& r : data)
        if (r.at(field) == value)
            printRecord(r, header);
}

// Dice
void dice(const vector<map<string, string>>& data, const map<string, string>& filters,
          const vector<string>& header) {
    cout << "\nDice result:\n";
    for (const auto& r : data) {
        bool match = true;
        for (const auto& f : filters)
            if (r.at(f.first) != f.second) { match = false; break; }
        if (match) printRecord(r, header);
    }
}

// Roll-up
void rollup(const vector<map<string, string>>& data, const string& groupField, const string& numericField) {
    cout << "\nRoll-Up by " << groupField << " (sum of " << numericField << ")\n";
    map<string, double> totals;
    for (const auto& r : data) {
        try {
            double val = stod(r.at(numericField));
            totals[r.at(groupField)] += val;
        } catch (...) {
            cout << "Column not numeric!\n";
            return;
        }
    }
    for (auto& t : totals)
        cout << t.first << " → " << t.second << endl;
}

// Drill-down
void drilldown(const vector<map<string, string>>& data, const vector<string>& header) {
    cout << "\nDrill-down view:\n";
    for (const auto& r : data)
        printRecord(r, header);
}

// Pivot
void pivot(const vector<map<string, string>>& data, const string& rowField,
           const string& colField, const string& numericField) {
    cout << "\nPivot (" << rowField << " vs " << colField << ")\n";
    map<string, map<string, double>> pivot;
    set<string> colValues;

    for (const auto& r : data) {
        try {
            double val = stod(r.at(numericField));
            string row = r.at(rowField);
            string col = r.at(colField);
            colValues.insert(col);
            pivot[row][col] += val;
        } catch (...) {
            cout << "Column not numeric!\n";
            return;
        }
    }

    cout << rowField << "\t";
    for (const string& col : colValues) cout << col << "\t";
    cout << endl;

    for (const auto& rowPair : pivot) {
        cout << rowPair.first << "\t";
        for (const string& col : colValues)
            cout << rowPair.second.at(col) << "\t";
        cout << endl;
    }
}

int main() {
    string fileName;
    cout << "Enter CSV file name: ";
    getline(cin, fileName);

    vector<string> header;
    vector<map<string, string>> data = readCSV(fileName, header);

    if (data.empty()) {
        cout << "No data found!\n";
        return 0;
    }

    cout << "\nColumns: ";
    for (auto& h : header) cout << h << " ";
    cout << endl;

    while (true) {
        cout << "\n--- OLAP MENU ---\n";
        cout << "1. Show Data\n2. Slice\n3. Dice\n4. Roll-Up\n5. Drill-Down\n6. Pivot\n0. Exit\nChoice: ";
        int ch; cin >> ch; cin.ignore();

        if (ch == 1) {
            for (auto& r : data) printRecord(r, header);

        } else if (ch == 2) {
            string field, value;
            cout << "Field: "; getline(cin, field);
            cout << "Value: "; getline(cin, value);
            sliceData(data, field, value, header);  // ✅ renamed

        } else if (ch == 3) {
            int n;
            cout << "How many filters? "; cin >> n; cin.ignore();
            map<string, string> filters;
            for (int i = 0; i < n; i++) {
                string f, v;
                cout << "Field: "; getline(cin, f);
                cout << "Value: "; getline(cin, v);
                filters[f] = v;
            }
            dice(data, filters, header);

        } else if (ch == 4) {
            string group, num;
            cout << "Group by field: "; getline(cin, group);
            cout << "Numeric field: "; getline(cin, num);
            rollup(data, group, num);

        } else if (ch == 5) {
            drilldown(data, header);

        } else if (ch == 6) {
            string row, col, num;
            cout << "Row field: "; getline(cin, row);
            cout << "Column field: "; getline(cin, col);
            cout << "Numeric field: "; getline(cin, num);
            pivot(data, row, col, num);

        } else if (ch == 0) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice!\n";
        }
    }
    return 0;
}