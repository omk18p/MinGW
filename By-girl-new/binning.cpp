#include <bits/stdc++.h>
using namespace std;

// Print bins
void printBins(vector<vector<double>> &bins){
    cout << "\nResulting Bins:\n";
    for(int i = 0; i < bins.size(); i++){
        cout << "Bin " << i+1 << ": ";
        for(double v : bins[i]) cout << v << " ";
        cout << endl;
    }
}

// Equal Width Binning
void equalWidthBinning(vector<double> data, int k){
    sort(data.begin(), data.end());
    double minVal = data.front();
    double maxVal = data.back();
    double width = (maxVal - minVal) / k;

    vector<vector<double>> bins(k);

    for(double v : data){
        int index = (int)((v - minVal) / width);
        if(index == k) index--;
        bins[index].push_back(v);
    }

    cout << "\n--- Equal Width Binning ---\n";
    printBins(bins);
}

// Equal Frequency Binning
void equalFrequencyBinning(vector<double> data, int k){
    sort(data.begin(), data.end());
    int n = data.size();
    int binSize = n / k;

    vector<vector<double>> bins;
    int start = 0;
    while(start < n){
        vector<double> bin(data.begin() + start, data.begin() + min(start + binSize, n));
        bins.push_back(bin);
        start += binSize;
    }

    cout << "\n--- Equal Frequency Binning ---\n";
    printBins(bins);
}

int main(){

    string filename;
    cout << "Enter CSV File Name (Example: data.csv): ";
    cin >> filename;

    ifstream file(filename);
    if(!file.is_open()){
        cout << "File not found!\n";
        return 0;
    }

    vector<string> header;
    string line, cell;

    // Read header
    if(getline(file, line)){
        stringstream ss(line);
        while(getline(ss, cell, ',')) header.push_back(cell);
    }

    cout << "\nAvailable Columns: \n";
    for(int i = 0; i < header.size(); i++)
        cout << i+1 << ". " << header[i] << endl;

    cout << "Select column number to apply binning on (Example: 2 for Marks): ";
    int col;
    cin >> col;
    col--;

    vector<double> data;
    while(getline(file, line)){
        stringstream ss(line);
        vector<string> row;
        while(getline(ss, cell, ',')) row.push_back(cell);

        if(row.size() > col){
            try{
                data.push_back(stod(row[col]));
            } catch(...){}
        }
    }

    if(data.empty()){
        cout << "No numeric data found in selected column!\n";
        return 0;
    }

    int k;
    cout << "Enter number of bins (Example: 3): ";
    cin >> k;

    cout << "\nChoose Binning Method:\n";
    cout << "1. Equal Width Binning\n";
    cout << "2. Equal Frequency Binning\n";
    cout << "Choice: ";
    int choice;
    cin >> choice;

    if(choice == 1)
        equalWidthBinning(data, k);
    else if(choice == 2)
        equalFrequencyBinning(data, k);
    else
        cout << "Invalid choice!";
}
