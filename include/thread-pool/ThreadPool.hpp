#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#define DEFAULT_THREADS 10

#include <pthread.h>
#include <functional>
#include <cstddef>
#include <iostream>

#include "queue/ThreadSafeTaskQueue.hpp"
#include "task/Task.hpp"

class ThreadPool {
    public:
    // constructors
    ThreadPool();
    ~ThreadPool();

    // modifiers
    void push(Task);

    private:
    int _size;
    pthread_t* _threads;
    ThreadSafeTaskQueue<Task> _queue;

    static void* ThreadHandler(void*);
};

ThreadPool::ThreadPool() :
    _size{DEFAULT_THREADS},
    _threads{new pthread_t[DEFAULT_THREADS]},
    _queue{}
{
    // std::cout << "ThreadPool initialization" << std::endl;
    for(int i = 0; i < _size; i++) {
        pthread_create(&_threads[i], NULL, ThreadHandler, this);
    }
    // std::cout << "ThreadPool created size: " << _size << std::endl;
}

ThreadPool::~ThreadPool() {
    for (int i = 0; i < _size; i++) {
        pthread_cancel(_threads[i]);
    }
    delete[] _threads;
}

void* ThreadPool::ThreadHandler(void* args) {
    ThreadPool* pool = (ThreadPool*) args;
    std::cout << "thread created" << std::endl;
    while (true) {
        Task task = pool->_queue.pop();
        if (task.taskFunction == nullptr) {
            std::cout << "Invalid task ignoring..." << std::endl;
        } else {
            task.taskFunction(task.args);
        }
    }
    return NULL;
}

// modifiers
void ThreadPool::push(Task task) {
    _queue.push(task);
}

#endif