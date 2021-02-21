
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
    // TODO Auto-generated destructor stub
}
/*
 * if the max correlate more then the threshold then call the parent,if its bigger then 0.5 but less than
 * the threshold then find the min circle and create a correlatedFeatures ant push it to the list.
 * the function get tow pointers to a vector and the correlate and the tow index to know the strings.
 */
void HybridAnomalyDetector::learnNormalHelper(float max, int i, int j, float *first, float *second, int size) {

    if (max >= threshold) {
        SimpleAnomalyDetector::learnNormalHelper(max, i, j, first, second, size);
    } else if (max >= 0.5) {
        Point **points = point_array(first, second, size);
        Circle min_radius = findMinCircle(points, size);
        delete_points(points, size);
        correlatedFeatures c;
        c.feature1 = min_feature(i, j);
        c.feature2 = max_feature(i, j);
        c.corrlation = max;
        c.threshold = min_radius.radius * float(1.1);
        c.x = min_radius.center.x;
        c.y = min_radius.center.y;
        _cf.push_back(c);
    }
}

/*
    * again - if the correlation are bigger than the threshold then call the parent to check if there is dev.
    * if not then check if the point is inside the circle.
    * get the correlatedFeatures and the point to check.
    */
bool HybridAnomalyDetector::devHelper(const correlatedFeatures &c, Point p) {
    if (c.corrlation >= threshold) {
        return SimpleAnomalyDetector::devHelper(c, p);
    } else {
        Circle circle(Point(c.x, c.y), c.threshold);
        return !findInCircle(p, circle);
    }
}
