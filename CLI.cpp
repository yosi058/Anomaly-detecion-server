#include "CLI.h"


/*
 * create the commend and init the DefualtIO.in addition, sent the data struct to every class that
 * can change the data inside it.
 *
 */
CLI::CLI(DefaultIO *newdio) {
    this->dio = newdio;
    this->data = new commend_data;
    auto *uploudFile = new UploudFile(this->dio, data);
    this->commands.push_back(uploudFile);
    auto *chengeThreshold = new ChengeThreshold(this->dio, data);
    this->commands.push_back(chengeThreshold);
    auto hybridAlgo = new HybridAlgo(this->dio, data);
    this->commands.push_back(hybridAlgo);
    auto reporstDev = new ReporstDev(this->dio, data);
    this->commands.push_back(reporstDev);
    auto anlayzeData = new AnlayzeData(this->dio, data);
    this->commands.push_back(anlayzeData);
    auto exitManu = new ExitMenu(this->dio, data);
    this->commands.push_back(exitManu);
}

void CLI::start() {
    int input;
    // while the client did not press 6 keep the menu alive again.
    do {
        this->dio->write("Welcome to the Anomaly Detection Server.\n"
                         "Please choose an option:\n");
        for (auto command : commands) {
            command->printDescption();
        }
       // string opt = this->dio->read();
        input = stoi(this->dio->read());
        // execute the commend that the client request.
        commands.at(input - 1)->execute();
    } while (input != 6);
}


CLI::~CLI() {
    for (auto &a:this->commands) {
        delete a;
    }
    delete data;
}

