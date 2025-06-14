#ifndef TASK_H
#define TASK_H

typedef void*(*TaskFunction)(void*);

class Task {
public:
    TaskFunction taskFunction = nullptr;
    void* args = nullptr;
};

#endif