

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include<iostream>
#include <cstring>
#include <syscall.h>

#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"

using namespace std;

class DefaultIO {
public:
    virtual string read() = 0;

    virtual void write(string text) = 0;

    virtual void write(float f) = 0;

    virtual void read(float *f) = 0;

    void createFile(const string &fileName) {
        ofstream file;
        file.open(fileName);
        string input;
        input = this->read();
        while (!(input == "done")) {
            file << input << endl;
            input = this->read();
        }
        file.close();
    }

    virtual ~DefaultIO() {}

    // you may add additional methods here
};

// you may add here helper classes
typedef struct commend_data {
    TimeSeries *ts1;
    TimeSeries *ts2;
    vector<AnomalyReport> reporst;
    HybridAnomalyDetector *algo = new HybridAnomalyDetector();
} commend_data;

// you may edit this class
class Command {
protected:
    DefaultIO *dio;
    string description;
    commend_data *data;

public:
    Command(DefaultIO *dio) : dio(dio) {}

    virtual void execute() = 0;

    virtual void printDescption() {
        this->dio->write(this->description);
    }

    virtual ~Command() {}
};

class UploudFile : virtual public Command {
public:
    UploudFile(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "1.upload a time series csv file\n";
    }

    void execute() override {
        const char *train_file = "anomalyTrain.csv";
        const char *test_file = "anomalyTest.csv";
        this->dio->write("Please upload your local train CSV file.\n");
        this->dio->createFile("anomalyTrain.csv");
        data->ts1 = new TimeSeries(train_file);
        this->dio->write("Upload complete.\n");
        this->dio->write("Please upload your local test CSV file.\n");
        this->dio->createFile("anomalyTest.csv");
        data->ts2 = new TimeSeries(test_file);
        this->dio->write("Upload complete.\n");
    }

    ~UploudFile() override {}
};


class ChengeThreshold : virtual public Command {
public:
    ChengeThreshold(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "2.algorithm settings\n";
    }


    void execute() override {
        float newThreshold;
        this->dio->write("The current correlation threshold is ");
        this->dio->write(data->algo->get_threshold());
        this->dio->write("\n");
        this->dio->write("Type a new threshold\n");
        newThreshold = stof(this->dio->read());
        while (!inRange(newThreshold)) {
            this->dio->write("please choose a value between 0 and 1.\n");
        }
        data->algo->set_threshold(newThreshold);
    }

    static bool inRange(float x) {
        return (x <= 1 && x >= 0);
    }

    ~ChengeThreshold() override {}
};


class HybridAlgo : public Command {
public:
    explicit HybridAlgo(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "3.detect anomalies\n";
    }

    void execute() override {
        data->algo->learnNormal(*(data->ts1));
        data->reporst = data->algo->detect(*(data->ts2));
        this->dio->write("anomaly detection complete.\n");
    }
};

class ReporstDev : public Command {
public:
    explicit ReporstDev(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "4.display results\n";
    }

    void execute() override {
        for (const AnomalyReport &a:data->reporst) {
            string text = to_string(a.timeStep) + "\t" + a.description + "\n";
            this->dio->write(text);
        }
        this->dio->write("Done.\n");
    }
};


class AnlayzeData : public Command {
public:
    explicit AnlayzeData(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "5.upload anomalies and analyze results\n";
    }

    void execute() override {
        this->dio->write("Please upload your local anomalies file.\n");
        //init a vector of pair that the first is a number and the second is also a pair - int and bool.
        vector<pair<int, pair<int, bool>>> timeClient;
        // the sum of the lines that was a report of dev.
        int sum = 0;
        //the lines of report of the client.
        int counter_line = 0;
        int first, second;
        string input = this->dio->read();
        // get all the report form the client and keep it inside the vector.
        while (!(input == "done")) {
            counter_line++;
            string part;
            stringstream stream(input);
            getline(stream, part, ',');
            first = stoi(part);
            getline(stream, part, ',');
            second = stoi(part);
            sum += (second + 1) - first;
            timeClient.emplace_back(first, make_pair(second, false));
            input = this->dio->read();
        }
        this->dio->write("Upload complete.\n");
        // init another vector to compare the reports from the detect and what the client gave us.
        vector<pair<long, long>> compare;
        // start with the first report
        // string des = this->data->reporst[0].description;
        long time = this->data->reporst[0].timeStep;
        long left = time;
        long right;
        /*
         get over the reports and get all the same description and there is one step between one report to the next one,
         and get them all together. inside the vecotr.
         */
        for (int i = 0; i < this->data->reporst.size() - 1; ++i) {
            string name1 = this->data->reporst[i].description;
            string name2 = this->data->reporst[i + 1].description;
            long step2 = this->data->reporst[i + 1].timeStep;
            if ((name1 == name2) && (time == step2 - 1)) {
                right = step2;
            } else {
                compare.emplace_back(left, right);
                right = left = step2;
            }
            time = step2;
        }
        compare.emplace_back(left, right);

        //sort the vector to be a up series.
        sort(compare.begin(), compare.end());
        // get the size of the lines.
        int size_steps = this->data->ts2->getSize();
        // the lines of the report from the client.
        int positive = counter_line;
        // get the amount of lines  that was not a reports above.
        int negative = size_steps - sum;
        int is_cros = 0;
        float TruePositive = 0, TrueNegative = 0, FalsePositive = 0, FalseNegative = 0;
        bool intersection;
        for (const pair<long, long> &c:compare) {
            intersection = false;
            for (pair<int, pair<int, bool>> &step:timeClient) {
                // if there is a intersection between the tow
                if (inRange(step.first, step.second.first, c.first, c.second)) {
                    is_cros += (isMax(step.second.first, c.second) + 1) - isMin(step.first, c.first);
                    intersection = true;
                    // update the bool of the step to be true,because was a intersection.
                    step.second.second = true;
                }
            }
            // if there no intersection.
            if (!intersection) {
                FalsePositive++;
            }
        }
        for (const pair<int, pair<int, bool>> &client:timeClient) {
            if (!client.second.second) {
                FalseNegative++;
            } else {
                TruePositive++;
            }
        }
        TrueNegative = size_steps - (is_cros + FalsePositive + FalseNegative);
        float True_positive_rate = (floor((TruePositive / float(positive)) * 1000)) / 1000;
        float False_alarm_rate = (floor((FalsePositive / float(negative)) * 1000)) / 1000;
        stringstream s;
        s << True_positive_rate;
        string text = "True Positive Rate: " + s.str() + "\n";
        this->dio->write(text);
        s.str(string());
        s << False_alarm_rate;
        text = "False Positive Rate: " + s.str() + "\n";
        this->dio->write(text);
    }

    bool inRange(int x1, int x2, int y1, int y2) {
        if ((x1 >= y1) && (x1 <= y2) || (x2 >= y1) && (x2 <= y2) || (y1 >= x1) && (y1 <= x2) ||
            (y2 >= x1) && (y2 <= x2)) {
            return true;
        }
        return false;
    }

    int isMin(int x1, int x2) {
        if (x1 < x2) {
            return x1;
        } else {
            return x2;
        }
    }

    int isMax(int x1, int x2) {
        if (x1 > x2) {
            return x1;
        } else {
            return x2;
        }
    }

};

class ExitMenu : public Command {
public:
    explicit ExitMenu(DefaultIO *dio, commend_data *commendData) : Command(dio) {
        this->data = commendData;
        this->description = "6.exit\n";
    }

    void execute() override {
        delete this->data->ts1;
        delete this->data->ts2;
        delete this->data->algo;
    }
};


class StandartIo : public DefaultIO {
public:
    string read() override {
        string input;
        cin >> input;
        return input;
    }

    void write(string text) override {
        cout << text;
    }

    void write(float f) override {
        cout << f;
    }

    void read(float *f) override {
        cin >> *f;
    }
};


#endif /* COMMANDS_H_ */
