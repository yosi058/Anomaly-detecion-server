

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector : public SimpleAnomalyDetector {
public:
    HybridAnomalyDetector();

    virtual ~HybridAnomalyDetector();

    void learnNormalHelper(float max, int i, int j, float *first, float *second, int size) override;

    bool devHelper(const correlatedFeatures &c, Point p) override;

};

#endif /* HYBRIDANOMALYDETECTOR_H_ */
