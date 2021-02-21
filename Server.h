/*
 * Server.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Eli
 */

#ifndef SERVER_H_
#define SERVER_H_


#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <cstdlib>
#include "CLI.h"
#include <stdexcept>
#include "commands.h"

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler {
public:
    virtual void handle(int clientID) = 0;
};


// you can add helper classes here and implement on the cpp file

class SocketIO : public DefaultIO {
private:
    int fd;
public:
    SocketIO(int new_fd) {
        this->fd = new_fd;
    }

    string read() override {
        string buffer;
        char check = 0;
        recv(fd, &check, sizeof(char), 0);
        while (check != '\n') {
            buffer += check;
            recv(fd, &check, sizeof(char), 0);
        }
        return buffer;
    }

    void write(string text) override {
        send(fd, text.c_str(), text.size(), 0);
    }

    void write(float f) override {
        stringstream ss1;
        ss1 << f;
        string input = ss1.str();
        const char *text = input.c_str();
        send(fd, text, input.size(), 0);
    }

    void read(float *f) override {
        string buffer;
        char check = 0;
        recv(fd, &check, sizeof(char), 0);
        while (check != '\n') {
            buffer += check;
            recv(fd, &check, sizeof(char), 0);
        }
        float my_input = stof(buffer);
        *f = my_input;
    }
};


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler : public ClientHandler {
public:
    void handle(int clientID) override {
        SocketIO socketIo(clientID);
        CLI cli(&socketIo);
        cli.start();
    }
};

// implement on Server.cpp
class Server {
    thread *t;// the thread to run the start() method in
    bool should_stop;
    int fd;
    sockaddr_in server;
    sockaddr_in client;

    // you may add data members

public:
    Server(int port) throw(const char *);

    virtual ~Server();

    void start(ClientHandler &ch) throw(const char *);

    void stop();
};

#endif /* SERVER_H_ */
/*
 *  string buffer;
        char check = 0;
        recv(fd, &check, sizeof(char), 0);
        while (check != '\n') {
            buffer += check;
            recv(fd, &check, sizeof(char), 0);
        }
        return buffer;
 */