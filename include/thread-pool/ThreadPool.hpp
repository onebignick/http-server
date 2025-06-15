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
    for(int i = 0; i < _size; i++) {
        pthread_create(&_threads[i], NULL, ThreadHandler, this);
    }
}

ThreadPool::~ThreadPool() {
    for(int i = 0;i < _size; ++i) {
        Task sentinal_task;
        sentinal_task.taskFunction = nullptr;
        sentinal_task.args = nullptr;
        _queue.push(sentinal_task);
    }

    for (int i = 0; i < _size; i++) {
        pthread_join(_threads[i], NULL);
    }
    delete[] _threads;
}

void* ThreadPool::ThreadHandler(void* args) {
    ThreadPool* pool = (ThreadPool*) args;
    while (true) {
        Task task = pool->_queue.pop();
        if (task.taskFunction == nullptr) {
            break;
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