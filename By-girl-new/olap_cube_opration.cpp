#include <bits/stdc++.h>
using namespace std;

// Read CSV
vector<map<string,string>> readCSV(string fileName, vector<string> &header){
    vector<map<string,string>> data;
    ifstream file(fileName);
    if(!file.is_open()){
        cout << "Error opening file!" << endl;
        return data;
    }

    string line;
    if(getline(file, line)){
        stringstream ss(line);
        string token;
        while(getline(ss, token, ',')) header.push_back(token);
    }

    while(getline(file, line)){
        stringstream ss(line);
        string token;
        map<string,string> row;
        int i = 0;
        while(getline(ss, token, ',')){
            if(i < header.size()) row[header[i]] = token;
            i++;
        }
        data.push_back(row);
    }
    return data;
}

// Print a row
void printRow(map<string,string> &row, vector<string> &header){
    for(auto &h: header)
        cout << row[h] << " | ";
    cout << endl;
}

// Slice1
void slice1(vector<map<string,string>> &data, string field, string value, vector<string> &header){
    cout << "\nResult:\n";
    for(auto &row : data)
        if(row[field] == value) printRow(row, header);
}

// Dice
void dice(vector<map<string,string>> &data, map<string,string> &filters, vector<string> &header){
    cout << "\nResult:\n";
    for(auto &row : data){
        bool ok = true;
        for(auto &f : filters)
            if(row[f.first] != f.second){ ok = false; break; }
        if(ok) printRow(row, header);
    }
}

// Roll-Up
void rollup(vector<map<string,string>> &data, string groupField, string numField){
    cout << "\nResult:\n";
    map<string,double> sum;
    for(auto &row : data){
        sum[row[groupField]] += stod(row[numField]);
    }
    for(auto &p : sum)
        cout << p.first << " -> " << p.second << endl;
}

// Drill-Down
void drilldown(vector<map<string,string>> &data, vector<string> &header){
    cout << "\nFull Data:\n";
    for(auto &row : data) printRow(row, header);
}

// Pivot
void pivot(vector<map<string,string>> &data, string rowField, string colField, string numField){
    cout << "\nPivot Result:\n";

    set<string> colVals;
    map<string,map<string,double>> table;

    for(auto &row : data){
        string r = row[rowField];
        string c = row[colField];
        double val = stod(row[numField]);
        colVals.insert(c);
        table[r][c] += val;
    }

    cout << rowField << "\t";
    for(auto &c: colVals) cout << c << "\t";
    cout << "\n";

    for(auto &r : table){
        cout << r.first << "\t";
        for(auto &c : colVals)
            cout << r.second[c] << "\t";
        cout << "\n";
    }
}

int main(){
    string file;
    cout << "Enter CSV file name (example: sales.csv): ";
    cin >> file;

    vector<string> header;
    vector<map<string,string>> data = readCSV(file, header);

    if(data.empty()){
        cout << "No Data Found!\n";
        return 0;
    }

    int ch;
    while(true){
        cout << "\n--- OLAP MENU ---\n"
             << "1. Show Data\n"
             << "2. Slice1 (Example: Field = Region, Value = North)\n"
             << "3. Dice (Example: Region = North AND Product = TV)\n"
             << "4. Roll-Up (Example: Group by = Region, Numeric = Sales)\n"
             << "5. Drill-Down (Show full detailed data)\n"
             << "6. Pivot (Example: Row=Region, Column=Month, Numeric=Sales)\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> ch;

        if(ch == 1){
            drilldown(data, header);

        } else if(ch == 2){
            cout << "\nExample Input:\nField: Region\nValue: North\n";
            string f,v;
            cout << "Field: "; cin >> f;
            cout << "Value: "; cin >> v;
            slice1(data, f, v, header);

        } else if(ch == 3){
            cout << "\nExample: Region = North AND Product = TV\n";
            int n; cout << "How many conditions?: "; cin >> n;
            map<string,string> filters;
            while(n--){
                string f,v;
                cout << "Field: "; cin >> f;
                cout << "Value: "; cin >> v;
                filters[f] = v;
            }
            dice(data, filters, header);

        } else if(ch == 4){
            cout << "\nExample: Group by = Region, Numeric = Sales\n";
            string g,nf;
            cout << "Group by field: "; cin >> g;
            cout << "Numeric field: "; cin >> nf;
            rollup(data, g, nf);

        } else if(ch == 5){
            drilldown(data, header);

        } else if(ch == 6){
            cout << "\nExample: Row=Region, Column=Month, Numeric=Sales\n";
            string rf,cf,nf;
            cout << "Row field: "; cin >> rf;
            cout << "Column field: "; cin >> cf;
            cout << "Numeric field: "; cin >> nf;
            pivot(data, rf, cf, nf);

        } else if(ch == 0) {
            cout << "Goodbye!\n";
            break;

        } else cout << "Invalid choice!";
    }
}
