#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

double median(vector<double>& v) {
    sort(v.begin(), v.end());
    int n = v.size();
    if (n % 2 == 0)
        return (v[n / 2 - 1] + v[n / 2]) / 2.0;
    else
        return v[n / 2];
}

int main() {
    vector<double> data;
    string filenm;

  
    cout << "Enter  file name : ";
    cin >> filenm;

    ifstream file(filenm);
    string line;

    if (file.is_open() && getline(file, line)) {
        stringstream ss(line);
        string value;
        while (getline(ss, value, ',')) {
            data.push_back(stod(value));
        }
        file.close();
    } else {
        cout << "Cant open the file" << endl;
        return 1;
    }

    if (data.empty()) {
        cout << "Your file is Empty" << endl;
        return 1;
    }

    sort(data.begin(), data.end());

    double minval = data.front();
    double maxval = data.back();
    double Q2 = median(data);

    vector<double> lowerhalf(data.begin(), data.begin() + data.size() / 2);
    vector<double> upperhalf;
    if (data.size() % 2 == 0)
        upperhalf = vector<double>(data.begin() + data.size() / 2, data.end());
    else
        upperhalf = vector<double>(data.begin() + data.size() / 2 + 1, data.end());

    double Q1 = median(lowerhalf);
    double Q3 = median(upperhalf);

    double IQR = Q3 - Q1;

    double lowerbound = Q1 - 1.5 * IQR;
    double upperbound = Q3 + 1.5 * IQR;

    double lowerwhisker = minval;
    for (double val : data) {
        if (val >= lowerbound) {
            lowerwhisker = val;
            break;
        }
    }

    double upperwhisker = maxval;
    for (auto it = data.rbegin(); it != data.rend(); ++it) {
        if (*it <= upperbound) {
            upperwhisker = *it;
            break;
        }
    }

    vector<double> outliers;
    for (double val : data) {
        if (val < lowerwhisker || val > upperwhisker)
            outliers.push_back(val);
    }

 
    cout << "\n 5 Number Summary " << endl;
    cout << "Minimum Value: " << minval << endl;
    cout << "First Quartile (Q1): " << Q1 << endl;
    cout << "Median (Q2): " << Q2 << endl;
    cout << "Third Quartile (Q3): " << Q3 << endl;
    cout << "Maximum Value: " << maxval << endl;

    cout<<"------------------------------------------"<<endl;
    
    cout << "Interquartile Range (IQR): " << IQR << endl;

    cout << "Lower Bound: " << lowerbound << endl;
    cout << "Upper Bound: " << upperbound << endl;

    cout << "Lower Whisker: " << lowerwhisker << endl;
    cout << "Upper Whisker: " << upperwhisker << endl;

    cout << "Outliers: ";
    if (outliers.empty())
        cout << "None";
    else
        for (double o : outliers)
            cout << o << " ";
    cout << endl;

    return 0;
}
