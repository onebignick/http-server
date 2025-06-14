#ifndef QUEUE_H
#define QUEUE_H

template<typename T>
class Queue {
public:
    virtual void push(T new_element) = 0;
    virtual T pop() = 0;
};

#endif