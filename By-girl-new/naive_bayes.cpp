// #include <bits/stdc++.h>
// using namespace std;

// vector<string> split(const string &s){
//     vector<string> res;
//     string temp;
//     stringstream ss(s);
//     while(getline(ss,temp,',')) res.push_back(temp);
//     return res;
// }

// int main(){
//     string filename;
//     cout<<"Enter file: ";
//     cin>>filename;
//     ifstream file(filename);
//     vector<vector<string>> data;
//     string line;
//     getline(file,line);
//     while(getline(file,line)) if(!line.empty()) data.push_back(split(line));
//     file.close();

//     int n = data.size();
//     int m = data[0].size()-1; 
//     vector<string> decision_classes;
//     map<string,int> class_count;

//     for(auto &row:data) class_count[row[m]]++;
//     for(auto &p:class_count) decision_classes.push_back(p.first);

//     cout<<"decision_classes: ";
//     for(auto &c:decision_classes) cout<<c<<" ";
//     cout<<"\n";

//     map<string,double> prior;
//     for(auto &c:decision_classes) prior[c] = (double)class_count[c]/n;

//     map<string,vector<map<string,int>>> condition;
//     for(auto &c:decision_classes) condition[c] = vector<map<string,int>>(m);
//     for(auto &row:data)
//     {
//         string cl = row[m];
//         for(int i=0;i<m;i++) condition[cl][i][row[i]]++;
//     }

//     cout<<"\nEnter condition (comma separated "<<m<<" features): ";
//     cin.ignore();
//     getline(cin,line);
//     vector<string> test = split(line);

//     string pred;
//     double maxp=0;

//     cout<<"\nProbabilities:\n";
//     for(auto &c:decision_classes){
//         double p = prior[c];
//         cout<<" P("<<c<<") = "<<p<<"\n";
//         for(int i=0;i<m;i++){
//             int cnt = condition[c][i][test[i]];
//             double px = (double)cnt / class_count[c];
//             cout<<" * P("<<test[i]<<"|"<<c<<") = "<<cnt<<"/"<<class_count[c]<<" = "<<px<<"\n";
//             p *= px;
//         }
//         cout<<"P("<<c<<"|sample) = "<<p<<"\n\n";
//         if(p>maxp){ maxp=p; pred=c; }
//     }

//     cout<<"Predicted class: "<<pred<<"\n";
// }
#include <bits/stdc++.h>
using namespace std;

vector<string> split(const string &s, char d) {
    vector<string> t; string x; stringstream ss(s);
    while (getline(ss, x, d)) t.push_back(x);
    return t;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file.\n";
        return 1;
    }

    string line;
    getline(file, line);
    vector<string> headers = split(line, ',');
    int n = headers.size() - 1;

    vector<vector<string>> data;
    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (row.size() == headers.size()) data.push_back(row);
    }
    file.close();

    map<string, int> classCount;
    for (auto &r : data) classCount[r.back()]++;
    int total = data.size();

    cout << "\nPrior Probabilities:\n";
    map<string, double> prior;
    for (auto &c : classCount) {
        prior[c.first] = (double)c.second / total;
        cout << "P(" << c.first << ") = " << classCount[c.first] << "/" << total
             << " = " << fixed << setprecision(3) << prior[c.first] << endl;
    }

    cout << "\nConditional Probabilities:\n";
    map<string, map<string, map<string, double>>> featureProb;
    for (int i = 0; i < n; i++) {
        map<string, map<string, int>> freq;
        for (auto &r : data) freq[r.back()][r[i]]++;

        for (auto &c : classCount) {
            cout << "\nFor Class = " << c.first << " (" << headers[i] << "):\n";
            for (auto &f : freq[c.first]) {
                featureProb[headers[i]][c.first][f.first] =
                    (double)f.second / classCount[c.first];
                cout << "P(" << f.first << " | " << c.first << ") = "
                     << f.second << "/" << classCount[c.first]
                     << " = " << featureProb[headers[i]][c.first][f.first] << endl;
            }
        }
    }

    cout << "\nEnter values for a test case:\n";
    map<string, string> inst;
    for (int i = 0; i < n; i++) {
        string v;
        cout << headers[i] << ": ";
        cin >> v;
        inst[headers[i]] = v;
    }

    cout << "\nCalculation of Posterior Probabilities:\n";
    map<string, double> post;

    for (auto &c : classCount) {
        double prob = prior[c.first];
        cout << "\nFor " << c.first << ":\n";
        cout << "P(" << c.first << ") = " << prior[c.first] << endl;
        for (int i = 0; i < n; i++) {
            string f = headers[i], v = inst[f];
            double cond = featureProb[f][c.first].count(v) ?
                          featureProb[f][c.first][v] : 0.0;
            cout << "P(" << v << " | " << c.first << ") = " << cond << endl;
            prob *= cond;
        }
        cout << "=> Final P(" << c.first << " | Case) = " << prob << endl;
        post[c.first] = prob;
    }

    cout << "\nComparison:\n";
    for (auto &p : post)
        cout << "P(" << p.first << " | Case) = " << p.second << endl;

    string pred = max_element(post.begin(), post.end(),
                              [](auto &a, auto &b) {
                                  return a.second < b.second;
                              })->first;

    cout << "\nBased on calculations:\nPredicted classification for case is "
         << pred << endl;

    return 0;
}



/// /Code that will work for the numeric data as well
// #include <bits/stdc++.h>
// using namespace std;

// // helper: trim whitespace both ends
// string trim(const string &s) {
//     size_t a = s.find_first_not_of(" \t\r\n");
//     if (a == string::npos) return "";
//     size_t b = s.find_last_not_of(" \t\r\n");
//     return s.substr(a, b - a + 1);
// }

// // helper: lowercase
// string lowerstr(string s) {
//     for (char &c : s) c = tolower((unsigned char)c);
//     return s;
// }

// bool isNumber(const string &s) {
//     for (char c : s)
//         if (!isdigit(c) && c!='.' && c!='-') return false;
//     return true;
// }

// double gaussian(double x, double mean, double sd) {
//     if (sd == 0) return (x == mean) ? 1 : 1e-9;
//     return (1.0/(sqrt(2*M_PI)*sd)) * exp(-((x-mean)*(x-mean))/(2*sd*sd));
// }

// double laplace(int count, int total, int distinct) {
//     return double(count+1) / double(total + distinct);
// }

// int main() {
//     cout << fixed << setprecision(3); // intermediate numbers with 3 decimals

//     string filename;
//     cout << "Enter CSV file name: ";
//     cin >> filename;

//     ifstream file(filename);
//     if (!file.is_open()) { cout << "File not found!\n"; return 0; }

//     string line;
//     getline(file, line);
//     stringstream ss(line);
//     vector<string> head;
//     string tok;
//     while (getline(ss, tok, ',')) head.push_back(trim(lowerstr(tok))); // normalize headers too

//     vector<vector<string>> data;
//     while (getline(file, line)) {
//         stringstream s(line);
//         vector<string> row;
//         while (getline(s, tok, ',')) {
//             string v = trim(lowerstr(tok)); // normalize CSV tokens
//             row.push_back(v);
//         }
//         if (!row.empty()) data.push_back(row);
//     }
//     file.close();

//     int rows = data.size();
//     if (rows == 0) { cout << "No data rows found.\n"; return 0; }
//     int features = head.size() - 1; // last col is class

//     vector<bool> isNum(features);
//     for (int f = 0; f < features; ++f) isNum[f] = isNumber(data[0][f]);

//     unordered_map<string,int> classCount;
//     for (auto &r : data) classCount[r.back()]++;

//     unordered_map<string, vector<vector<double>>> numVals;
//     unordered_map<string, vector<vector<string>>> catVals;
//     for (auto &p : classCount) {
//         numVals[p.first].assign(features, {});
//         catVals[p.first].assign(features, {});
//     }

//     for (auto &r : data) {
//         string cls = r.back();
//         for (int f = 0; f < features; ++f) {
//             if (isNum[f]) numVals[cls][f].push_back(stod(r[f]));
//             else catVals[cls][f].push_back(r[f]);
//         }
//     }

//     unordered_map<string, vector<double>> mean, sd;
//     for (auto &p : classCount) {
//         string cls = p.first;
//         mean[cls].assign(features, 0.0);
//         sd[cls].assign(features, 0.0);
//         for (int f = 0; f < features; ++f) {
//             if (!isNum[f]) continue;
//             auto &vals = numVals[cls][f];
//             double s=0;
//             for (double v : vals) s += v;
//             double m = vals.empty() ? 0.0 : s / vals.size();
//             mean[cls][f] = m;
//             double sq = 0;
//             for (double v : vals) sq += (v - m) * (v - m);
//             sd[cls][f] = vals.empty() ? 0.0 : sqrt(sq / vals.size());
//         }
//     }

//     vector<unordered_set<string>> distinct(features);
//     for (auto &r : data)
//         for (int f = 0; f < features; ++f)
//             if (!isNum[f]) distinct[f].insert(r[f]);

//     // get input and normalize too
//     vector<string> input(features);
//     cout << "\nEnter values for prediction:\n";
//     cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush newline
//     for (int f = 0; f < features; ++f) {
//         cout << head[f] << ": ";
//         string tmp;
//         getline(cin, tmp);
//         input[f] = trim(lowerstr(tmp));
//     }

//     string bestClass;
//     long double bestScore = -1.0L;

//     for (auto &p : classCount) {
//         string cls = p.first;
//         int totalInClass = p.second;
//         long double score = (long double)totalInClass / rows; // prior
//         cout << "\nClass: " << cls << "\nP(Class) = " << (double)score << endl;

//         for (int f = 0; f < features; ++f) {
//             if (isNum[f]) {
//                 double x = stod(input[f]);
//                 double g = gaussian(x, mean[cls][f], sd[cls][f]);
//                 cout << "  Gaussian P(" << head[f] << "=" << x << " | " << cls << ") = " << g << endl;
//                 score *= (long double)g;
//             } else {
//                 int cnt = 0;
//                 for (auto &val : catVals[cls][f]) if (val == input[f]) ++cnt;

//                 long double condProb;
//                 if (cnt > 0) {
//                     condProb = (long double)cnt / totalInClass;
//                     cout << "  Categorical (raw) P(" << head[f] << "=" << input[f] << " | " << cls
//                          << ") = " << cnt << "/" << totalInClass << " = " << (double)condProb << endl;
//                 } else {
//                     int V = (int)distinct[f].size();
//                     condProb = laplace(cnt, totalInClass, V);
//                     cout << "  Categorical (smoothed) P(" << head[f] << "=" << input[f] << " | " << cls
//                          << ") = (" << cnt << "+1)/(" << totalInClass << "+" << V << ") = " << (double)condProb << endl;
//                 }
//                 score *= condProb;
//             }
//         }

//         // print final unnormalized score in scientific (6 digits)
//         cout << "  Final (unnormalized) score for " << cls << " = ";
//         cout << scientific << setprecision(6) << (double)score << defaultfloat << setprecision(3) << endl;

//         if (score > bestScore) {
//             bestScore = score;
//             bestClass = cls;
//         }
//     }

//     cout << "\n==============================\n";
//     cout << "Predicted Class = " << bestClass << " ✅\n";
//     cout << "==============================\n";

//     return 0;
// }
//covert numeric value into category and perform opration on it
#include <bits/stdc++.h>
using namespace std;

// Split CSV line
vector<string> split(const string &s, char d) {
    vector<string> t; 
    string x; 
    stringstream ss(s);
    while (getline(ss, x, d)) t.push_back(x);
    return t;
}

int main() {
    string filename;
    cout << "Enter CSV filename: ";
    cin >> filename;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file.\n";
        return 1;
    }

    // Read header
    string line;
    getline(file, line);
    vector<string> headers = split(line, ',');
    int n = headers.size() - 1; // last column is class

    // Read data
    vector<vector<string>> data;
    while (getline(file, line)) {
        vector<string> row = split(line, ',');
        if (row.size() == headers.size()) data.push_back(row);
    }
    file.close();

    // Count classes
    map<string, int> classCount;
    for (auto &r : data) classCount[r.back()]++;
    int total = data.size();

    // Collect all possible feature values
    map<string, set<string>> featureValues;
    for (int i = 0; i < n; i++)
        for (auto &r : data)
            featureValues[headers[i]].insert(r[i]);

    // Compute prior probabilities
    map<string, double> prior;
    for (auto &c : classCount)
        prior[c.first] = (double)c.second / total;

    // Compute conditional probabilities
    map<string, map<string, map<string, double>>> featureProb;
    for (int i = 0; i < n; i++) {
        map<string, map<string, int>> freq;
        for (auto &r : data) freq[r.back()][r[i]]++;

        for (auto &c : classCount)
            for (auto &f : featureValues[headers[i]])
                featureProb[headers[i]][c.first][f] = 
                    (double)(freq[c.first][f]) / classCount[c.first];
    }

    // Input test case
    cout << "\nEnter values for a test case:\n";
    map<string, string> inst;
    for (int i = 0; i < n; i++) {
        string v;
        cout << headers[i] << ": ";
        cin >> v;
        inst[headers[i]] = v;
    }

    // Function to calculate conditional probability with Laplace smoothing
    auto getConditionalProb = [&](const string &feature, const string &value, const string &cls) {
        int count = featureProb[feature][cls].count(value) ? 
                    featureProb[feature][cls][value] * classCount[cls] : 0;
        int V = featureValues[feature].size();
        return (count + 1.0) / (classCount[cls] + V); // Laplace smoothing
    };

    // Calculate posterior probabilities
    cout << "\nCalculation of Posterior Probabilities:\n";
    map<string, double> post;
    for (auto &c : classCount) {
        double prob = prior[c.first];
        cout << "\nFor " << c.first << ":\n";
        cout << "P(" << c.first << ") = " << prior[c.first] << endl;
        for (int i = 0; i < n; i++) {
            string f = headers[i], v = inst[f];
            double cond = getConditionalProb(f, v, c.first);
            cout << "P(" << v << " | " << c.first << ") = " << cond << endl;
            prob *= cond;
        }
        cout << "=> Final P(" << c.first << " | Case) = " << prob << endl;
        post[c.first] = prob;
    }

    // Predict class
    string pred = max_element(post.begin(), post.end(),
                              [](auto &a, auto &b){ return a.second < b.second; })->first;

    cout << "\nPredicted classification for the case: " << pred << endl;

    return 0;
}
