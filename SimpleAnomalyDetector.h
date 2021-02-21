

#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cmath>

struct correlatedFeatures {
    string feature1, feature2;  // names of the correlated features
    float corrlation;
    Line lin_reg;
    float threshold;
    float x, y;
};


class SimpleAnomalyDetector : public TimeSeriesAnomalyDetector {
protected:
    vector<correlatedFeatures> _cf;
    vector<string> _strings;
    float threshold;
public:
    SimpleAnomalyDetector();

    virtual ~SimpleAnomalyDetector();

    virtual void learnNormal(const TimeSeries &ts);

    virtual vector<AnomalyReport> detect(const TimeSeries &ts);

    vector<correlatedFeatures> getNormalModel() {
        return _cf;
    }

    vector<string> getStrings() {
        return _strings;
    }

    float get_threshold() const;

    void print();

    string min_feature(int x, int y);

    string max_feature(int x, int y);

    void delete_points(Point **p, int size);

    void set_threshold(float x);

    bool find_pair(const string &key_i, const string &key_j);

    void create_corrloate(int feature1, int feature2, float max, Point **p, int size);

protected:

    Point **point_array(float *x, float *y, int size);

    float max_dev_point(Point **p, Line l, int size);

    virtual void learnNormalHelper(float max, int i, int j, float *first, float *second, int size);

    virtual bool devHelper(const correlatedFeatures &c, Point p);
};


#endif /* SIMPLEANOMALYDETECTOR_H_ */
