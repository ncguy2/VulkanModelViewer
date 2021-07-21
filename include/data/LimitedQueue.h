//
// Created by Guy on 17/07/2021.
//

#ifndef GLMODELVIEWER_LIMITEDQUEUE_H
#define GLMODELVIEWER_LIMITEDQUEUE_H

#include <vector>

template <typename T>
class LimitedQueue {
public:
    LimitedQueue(int limit) : limit(limit) {}

    void Add(T& t) {
        data.push_back(t);
        Check();
    }

    std::vector<T>& GetVector() {
        return data;
    }

    int Limit() {
        return limit;
    }

protected:
    void Check() {
        while(data.size() > limit)
            data.erase(data.begin());
    }

    int limit;
    std::vector<T> data;
};

#endif//GLMODELVIEWER_LIMITEDQUEUE_H
