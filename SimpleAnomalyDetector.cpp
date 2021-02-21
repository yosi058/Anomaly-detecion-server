
#include "SimpleAnomalyDetector.h"
#include "AnomalyDetector.h"
#include "timeseries.h"
#include <cmath>


SimpleAnomalyDetector::SimpleAnomalyDetector() {
    threshold = 0.9;
}

SimpleAnomalyDetector::~SimpleAnomalyDetector() {
    // TODO Auto-generated destructor stub
}

/*
 * this function get the output from the file that are in a map table - and find the normal behavior.
 * for that,she learn all the correlate between tow feature and find the max.learning the line-reg
 * that contion all the points that are from the toe feature.after we find the line - finding the max devaion
 * in this line and keep it in a list.
 */
void SimpleAnomalyDetector::learnNormal(const TimeSeries &ts) {
    // get all the feature.
    _strings = ts.features();
    float max_correlate;
    //get over all the feature.
    for (int i = 0; i < _strings.size(); i++) {
        max_correlate = 0;
        //find the coronet feature
        string key_i = _strings.at(i);
        int feature1 = i;
        int feature2;
        // get the vector of this feature - is column.
        vector<float> first = ts.columnKey(key_i);
        // get the vector of the j feature - is column.
        vector<float> second;
        //create tow float* for pearson.(pointer to a vector)
        float *values_feature1 = first.data();
        float *values_feature2;
        // get the size of the column.
        int size_column = ts.columnKey(key_i).size();
        for (int j = i + 1; j < _strings.size(); j++) {
            //get the feature that we want to compare with coorolate.
            string key_j = _strings.at(j);
            // get his column of this feature.
            second = ts.columnKey(key_j);
            // get the float* of this feature.
            values_feature2 = second.data();
            // find the corolate between the tow features.
            float current_cor = fabs(pearson(values_feature1, values_feature2, size_column));
            //if the correlate is bigger than 0.9 and the max.
            if (current_cor > max_correlate) {
                max_correlate = current_cor;
                feature2 = j;
            }
        }
        second = ts.columnKey(_strings.at(feature2));
        values_feature2 = second.data();
        learnNormalHelper(max_correlate, feature1, feature2, values_feature1, values_feature2, size_column);
    }
}


/*
 *  find the dev of the new time series and reports if its bigger than the normal.
 */
vector<AnomalyReport> SimpleAnomalyDetector::detect(const TimeSeries &ts) {
    vector<AnomalyReport> reports;
    int size = ts.columnKey(_strings[0]).size();
    // get over the list of correlatedFeatures
    for (const correlatedFeatures &c:_cf) {
        vector<float> first = ts.columnKey(c.feature1);
        vector<float> second = ts.columnKey(c.feature2);
        // get over all the features
        for (int i = 0; i < size; ++i) {
            // get the column of the toe features that inside the list.
            float point_x = first[i];
            float point_y = second[i];
            // get the point in the i line.
            Point point(point_x, point_y);
            // if the dex id bigger than the threshold - report.
            if (devHelper(c, point)) {
                string description = c.feature1 + '-' + c.feature2;
                long step = i + 1;
                AnomalyReport rep(description, step);
                reports.push_back(rep);
            }
        }
    }
    return reports;
}

// create a array of new points.
Point **SimpleAnomalyDetector::point_array(float *x, float *y, int size) {
    // create a array of pointers that every one is a pointer to point.
    auto **p = new Point *[size];
    // create all the points.
    for (int i = 0; i < size; i++) {
        p[i] = new Point(x[i], y[i]);
    }
    return p;
}

// find  the max dev that are legal.
float SimpleAnomalyDetector::max_dev_point(Point **p, Line l, int size) {
    float max_dev = 0;
    // get over the array of points and find what is the max of the dev in this line.
    for (int i = 0; i < size; i++) {
        Point point(p[i]->x, p[i]->y);
        // get the dev of that specific point
        float current_dev = dev(point, l);
        if (current_dev > max_dev) {
            max_dev = current_dev;
        }
    }
    // return the max * 1.1
    return max_dev * float(1.1);
}

// find if the pair is already inside the list and not put it.
bool SimpleAnomalyDetector::find_pair(const string &key_i, const string &key_j) {
    for (const correlatedFeatures &c:_cf) {
        if ((key_j == c.feature1) && (key_i == c.feature2)) {
            return false;
        }
    }
    return true;
}

// print the list of the correlatedFeatures.
void SimpleAnomalyDetector::print() {
    vector<correlatedFeatures>::iterator it;
    for (it = _cf.begin(); it != _cf.end(); it++) {
        cout << it->feature1 << "-" << it->feature2 << endl;
    }
}

// create a correlatedFeatures by the order.
void SimpleAnomalyDetector::create_corrloate(int feature1, int feature2, float max, Point **points, int size) {
    correlatedFeatures correlated;
    correlated.feature1 = min_feature(feature1, feature2);
    correlated.feature2 = max_feature(feature1, feature2);
    correlated.corrlation = max;
    correlated.lin_reg = linear_reg(points, size);
    correlated.threshold = max_dev_point(points, correlated.lin_reg, size);
    _cf.push_back(correlated);

}

string SimpleAnomalyDetector::min_feature(int x, int y) {
    if (y > x) {
        return _strings.at(x);
    }
    return _strings.at(y);

}

// find the max feature
string SimpleAnomalyDetector::max_feature(int x, int y) {
    if (y > x) {
        return _strings.at(y);
    }
    return _strings.at(x);
}

// delete the  points we allocate.
void SimpleAnomalyDetector::delete_points(Point **p, int size) {
    for (int i = 0; i < size; i++) {
        delete p[i];
    }
    delete[] p;
}

void SimpleAnomalyDetector::set_threshold(float x) {
    threshold = x;
}

void SimpleAnomalyDetector::learnNormalHelper(float max, int i, int j, float *first, float *second, int size) {
    if (max >= threshold) {
        // create a array of points from the 2 features to find the line-reg.
        Point **points = point_array(first, second, size);
        // create the coorolate_feature and put it in the list.
        create_corrloate(i, j, max, points, size);
        // delete all the allocate memory - the points.we dont need them any more.
        delete_points(points, size);
    }
}


bool SimpleAnomalyDetector::devHelper(const correlatedFeatures &c, Point point) {
    float out_range = dev(point, c.lin_reg);
    return out_range > c.threshold;
}

float SimpleAnomalyDetector::get_threshold() const {
    return threshold;
}

