//
// Created by yosi on 17.11.2020.
//

#include "timeseries.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

TimeSeries::TimeSeries(const char *CSVfileName) {
    ifstream file;
    file.open(CSVfileName);
    string line, feature;
    getline(file, line);
    stringstream stream(line);
    vector<float> values;
    int counter;
    while (getline(stream, feature, ',')) {
        string myKey = feature;
        _myFeaturs.push_back(myKey);
        _myMap.insert(make_pair(myKey, values));
    }
    while (getline(file, line)) {
        stringstream streamfile(line);
        counter = 0;
        string value;
        while (getline(streamfile, value, ',')) {
            _myMap.find(_myFeaturs.at(counter))->second.push_back(stod(value));
            counter++;
        }
    }
    file.close();
}

map<string, vector<float>> TimeSeries::getMap() const {
    return _myMap;
}

int TimeSeries::getSize() const {
    return this->columnKey(this->_myFeaturs[0]).size();
}

vector<float> TimeSeries::columnKey(const string &key) const {
    map<string, vector<float>>::const_iterator it;
    it = _myMap.find(key);
    return it->second;
}

vector<string> TimeSeries::features() const {
    return _myFeaturs;
}

void TimeSeries::print() const {
    map<string, vector<float>>::const_iterator it;
    for (it = _myMap.begin(); it != _myMap.end(); it++) {
        cout << "[" << it->first << ":";
        for (float i : it->second) {
            cout << i << " ";
        }
        cout << "]" << "\n";
    }
}

void TimeSeries::addLIne(const char *CSVfileName) {
    vector<string> features = this->features();
    stringstream stream(CSVfileName);
    string newline;
    int counter = 0;
    while (getline(stream, newline, ',')) {
        _myMap.find(features.at(counter))->second.push_back(stod(newline));
        counter++;
    }
}

float TimeSeries::valueFeature(const string &feature, int time) const {
    return _myMap.find(feature)->second[time];
}





