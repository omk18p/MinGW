#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;


string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if(start == string::npos || end == string::npos) return "";
    return s.substr(start, end - start + 1);
}

int main() {
    string inputFile, outputFile;
    cout << "Enter input CSV filename: ";
    cin >> inputFile;

    ifstream file(inputFile);
    if(!file.is_open()) {
        cout << " Cannot open  file.\n";
        return 0;
    }

    cout << "Enter output CSV filename: ";
    cin >> outputFile;

    string line;
    vector<string> headers;
    map<string,map<string,double>> data;

    // Read header
    if(getline(file,line)) {
        stringstream ss(line);
        string col;
        while(getline(ss,col,',')) headers.push_back(trim(col));
    }

    // Read data rows
    while(getline(file,line)) {
        if(trim(line).empty()) continue;
        stringstream ss(line);
        vector<string> row;
        string cell;
        while(getline(ss,cell,',')) row.push_back(trim(cell));
        if(row.size() < 2) continue;

        string rowLabel = row[0];
        for(size_t i=1;i<headers.size();i++){
            double val = 0;
            if(i<row.size() && !row[i].empty()) val = stod(row[i]);
            data[rowLabel][headers[i]] = val;
        }
    }
    file.close();


    map<string,double> rowTotals, colTotals;
    double grandTotal = 0;
    for(auto &r: data){
        double sum = 0;
        for(size_t i=1;i<headers.size();i++){
            string col = headers[i];
            sum += r.second[col];
            colTotals[col] += r.second[col];
        }
        rowTotals[r.first] = sum;
        grandTotal += sum;
    }

   
    ofstream out(outputFile);
    out << headers[0];
    for(size_t i=1;i<headers.size();i++)
        out << "," << headers[i] << " Value," << headers[i] << " t-weight," << headers[i] << " d-weight";
    out << ",Row Total,Row t-weight,Row d-weight\n";

    for(auto &r: data){
        out << r.first;
        for(size_t i=1;i<headers.size();i++){
            string col = headers[i];
            double val = r.second[col];
            double t = rowTotals[r.first]==0 ? 0 : val/rowTotals[r.first]*100;
            double d = colTotals[col]==0 ? 0 : val/colTotals[col]*100;
            out << "," << fixed << setprecision(2) << val
                << "," << fixed << setprecision(2) << t << "%"
                << "," << fixed << setprecision(2) << d << "%";
        }
        out << "," << fixed << setprecision(2) << rowTotals[r.first]
            << "," << "100.00%"
            << "," << fixed << setprecision(2) << rowTotals[r.first]/grandTotal*100 << "%" << endl;
    }

    out << "Total";
    for(size_t i=1;i<headers.size();i++){
        string col = headers[i];
        double val = colTotals[col];
        double t = val/grandTotal*100;
        out << "," << fixed << setprecision(2) << val
            << "," << fixed << setprecision(2) << t << "%"
            << "," << "100.00%";
    }
    out << "," << fixed << setprecision(2) << grandTotal
        << "," << "100.00%"
        << "," << "100.00%" << endl;

    out.close();
    cout << "✅ " << outputFile << " is created successfully!\n";
    return 0;
}
