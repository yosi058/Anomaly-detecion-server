
#include <queue>
#include "Server.h"

Server::Server(int port) throw(const char *) {
    this->should_stop = false;
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw "socket failed";
    }
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);
    this->server.sin_addr.s_addr = INADDR_ANY;
    int connect = bind(fd, (struct sockaddr *) &server, sizeof(server));
    if (connect < 0) {
        throw "bind failed";
    }
    if (listen(fd, 5) < 0) {
        throw "listen failed";
    }
}

/*
void threadClient(ClientHandler &ch,Server& s) {
    socklen_t size_client = sizeof(client);
    while (!should_stop) {
        int fd_client = accept(fd, (struct sockaddr *) &client, &size_client);
        if (fd_client < 0) {
            throw "connect failed";
        }
        ch.handle(fd_client);
        close(fd_client);
    }
    close(fd);
}
 */

void Server::start(ClientHandler &ch) throw(const char *) {
    this->t = new thread([&ch, this] {
        socklen_t size_client = sizeof(client);
        while (!should_stop) {
            int fd_client = accept(fd, (struct sockaddr *) &client, &size_client);
            if (fd_client < 0) {
                throw "connect failed";
            }
            ch.handle(fd_client);
            close(fd_client);
        }
        close(fd);
    });
}


void Server::stop() {
    this->should_stop = true;
    t->join(); // do not delete this!
}

Server::~Server() {
    delete t;
}
