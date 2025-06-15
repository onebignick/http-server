#ifndef THREAD_SAFE_TASK_QUEUE_H
#define THREAD_SAFE_TASK_QUEUE_H

#define DEFAULT_CAPACITY 10

#include <pthread.h>

#include "queue/queue.h"

template<typename T>
class ThreadSafeTaskQueue final : public Queue<T> {
    private:
    int _size;
    int _capacity;
    int _head, _tail;
    T* _queue;
    pthread_mutex_t lock;
    pthread_cond_t not_full, not_empty;

    public:
    ThreadSafeTaskQueue();
    ~ThreadSafeTaskQueue();

    // constructors
    ThreadSafeTaskQueue(int);

    // modifiers
    void push(T value) override;
    T pop() override;
};

template<typename T>
ThreadSafeTaskQueue<T>::ThreadSafeTaskQueue() :
    _size{0},
    _capacity{DEFAULT_CAPACITY},
    _head{0},
    _tail{0},
    _queue{new T[DEFAULT_CAPACITY]}
{
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);
}

template<typename T>
ThreadSafeTaskQueue<T>::~ThreadSafeTaskQueue() {
    delete[] _queue;
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);
}

// constructors
template<typename T>
ThreadSafeTaskQueue<T>::ThreadSafeTaskQueue(int capacity) :
    _size{0},
    _capacity{capacity},
    _head{0},
    _tail{0},
    _queue{new T[capacity]}
{
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&not_empty, NULL);
    pthread_cond_init(&not_full, NULL);
}

template<typename T>
void ThreadSafeTaskQueue<T>::push(T value) {
    pthread_mutex_lock(&lock);
    while (_size == _capacity) pthread_cond_wait(&not_full, &lock);

    _queue[_tail] = value;
    _tail = (_tail + 1) % _capacity;
    _size++;
    pthread_cond_signal(&not_empty);
    pthread_mutex_unlock(&lock);
}

template<typename T>
T ThreadSafeTaskQueue<T>::pop() {
    pthread_mutex_lock(&lock);
    while (_size == 0) pthread_cond_wait(&not_empty, &lock);

    T result = _queue[_head];
    _head = (_head + 1) % _capacity;
    _size--;
    pthread_cond_signal(&not_full);
    pthread_mutex_unlock(&lock);
    return result;
}

#endif