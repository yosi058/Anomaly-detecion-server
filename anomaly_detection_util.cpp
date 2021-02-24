/*
 * animaly_detection_util.cpp
 *
 * Author:yosef-natan berebi 208486365.
 */

#include <math.h>
#include "anomaly_detection_util.h"

/*
 * calculate the average of the array.
 */
float avg(const float *x, int size) {
    int i;
    float sum = 0;
    for (i = 0; i < size; i++) {
        sum += x[i];
    }
    float avg = sum / size;
    return avg;
}

// returns the variance of X  by calculate the average of the sum of each number minus the avg pow 2.
float var(float *x, int size) {
    float sum = 0;
    int i;
    float avgX = avg(x, size);
    for (i = 0; i < size; i++) {
        sum += pow(x[i] - avgX, 2);
    }
    float var = sum / size;
    return var;
}

// returns the covariance of X and Y
float cov(float *x, float *y, int size) {
    float arrayZ[size];
    int i;
    for (i = 0; i < size; i++) {
        arrayZ[i] = x[i] * y[i];
    }
    float avgX = avg(x, size);
    float avgY = avg(y, size);
    float avgZ = avg(arrayZ, size);
    float cov = avgZ - (avgX * avgY);
    return cov;
}


// returns the Pearson correlation coefficient of X and Y
float pearson(float *x, float *y, int size) {
    float varX = var(x, size);
    float varY = var(y, size);
    float squarVar = sqrt(varX) * sqrt(varY);
    float pearson = cov(x, y, size) / squarVar;
    return pearson;
}

// performs a linear regression and returns the line equation
Line linear_reg(Point **points, int size) {
    float arrX[size];
    float arrY[size];
    int i;
    for (i = 0; i < size; i++) {
        arrX[i] = points[i]->x;
        arrY[i] = points[i]->y;
    }
    float a = cov(arrX, arrY, size) / var(arrX, size);
    float b = avg(arrY, size) - (a * (avg(arrX, size)));
    return Line(a, b);

}

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size) {
    Line line = linear_reg(points, size);
    float dis = line.f(p.x);
    return fabs(dis - p.y);
}

// returns the deviation between point p and the line from the function.
float dev(Point p, Line l) {
    return fabs(l.f(p.x) - p.y);
}