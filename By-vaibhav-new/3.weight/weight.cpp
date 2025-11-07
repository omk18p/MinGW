#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct Row {
    string class_name;
    int cat1;
    int cat2;
};

int main() {
    ifstream file("input.csv");
    if (!file) {
        cout << "File not found!\n";
        return 1;
    }

    string line;
    vector<Row> data;
    int totalCat1 = 0, totalCat2 = 0;

    getline(file, line); // skip header line
    while (getline(file, line)) {
        stringstream ss(line);
        string className, c1s, c2s;

        getline(ss, className, ',');
        getline(ss, c1s, ',');
        getline(ss, c2s, ',');

        int cat1 = stoi(c1s);
        int cat2 = stoi(c2s);

        Row r;
        r.class_name = className;
        r.cat1 = cat1;
        r.cat2 = cat2;
        data.push_back(r);

        totalCat1 += cat1;
        totalCat2 += cat2;
    }
    file.close();

    ofstream out("twt_dwt_output.csv");
    out << "Class,Category1,Twt,Dwt,Category2,Twt,Dwt,Total\n";

    cout << "\nTwt and Dwt Results:\n\n";
    cout << "Class         Cat1   Twt%   Dwt%   Cat2   Twt%   Dwt%   Total\n";

    for (int i = 0; i < data.size(); i++) {
        string name = data[i].class_name;
        int cat1 = data[i].cat1;
        int cat2 = data[i].cat2;
        int total = cat1 + cat2;

        double cat1Twt = (cat1 * 100.0) / total;
        double cat1Dwt = (cat1 * 100.0) / totalCat1;
        double cat2Twt = (cat2 * 100.0) / total;
        double cat2Dwt = (cat2 * 100.0) / totalCat2;

        cout << name << "   " << cat1 << "   " << cat1Twt << "   " << cat1Dwt
             << "   " << cat2 << "   " << cat2Twt << "   " << cat2Dwt
             << "   " << total << endl;

        out << name << "," << cat1 << "," << cat1Twt << "%," << cat1Dwt << "%,"
            << cat2 << "," << cat2Twt << "%," << cat2Dwt << "%," << total << "\n";
    }

    cout << "Total         " << totalCat1 << "   100   100   " << totalCat2
         << "   100   100   " << totalCat1 + totalCat2 << endl;

    out << "Total," << totalCat1 << ",100%,100%," << totalCat2
        << ",100%,100%," << totalCat1 + totalCat2 << "\n";

    out.close();

    cout << "\nSaved to twt_dwt_output.csv\n";
    return 0;
}
