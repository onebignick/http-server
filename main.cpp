#include <cstddef>
#include <iostream>
#include <pthread.h>

// #include "queue/task-queue.h"
// #include "server/tcp-server.h"

#include "thread-pool/ThreadPool.hpp"
#include "task/Task.hpp"

void* testFunction(void* args) {
  int* num = (int*)args;
  std::cout << "task number: " << *num << std::endl;
  delete num;
  return NULL;
}

int main() {
  // TCPServer tcpServer;
  // tcpServer.start();
  ThreadPool threadPool;

  int cur = 0;
  while (cur < 100) {
    Task task;
    task.taskFunction = (TaskFunction)testFunction;
    task.args = (void*)(new int(cur));
    threadPool.push(task);
    cur++;
  }
}
