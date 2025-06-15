#include <cstddef>
#include <iostream>
#include <pthread.h>

#include "server/tcp-server.h"
#include "thread-pool/ThreadPool.hpp"
#include "task/Task.hpp"

int main() {
  TCPServer tcpServer;
  tcpServer.start();
}
