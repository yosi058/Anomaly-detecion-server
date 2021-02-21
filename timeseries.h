
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <sstream>


#ifndef TIMESERIES_H_
#define TIMESERIES_H_

using namespace std;

class TimeSeries {
private:
    map<string, vector<float>> _myMap;
    vector<string> _myFeaturs;

public:

     TimeSeries(const char *CSVfileName);

    map<string, vector<float>> getMap()const;

    vector<float> columnKey(const string& key)const;

    vector<string> features() const;

    void print()const;

    void addLIne(const char *CSVfileName);

    float valueFeature(const string& feature, int time)const;

    int getSize() const;
};


#endif /* TIMESERIES_H_ */
