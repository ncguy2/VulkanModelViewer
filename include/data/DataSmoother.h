//
// Created by Guy on 17/07/2021.
//

#ifndef GLMODELVIEWER_INTSMOOTHER_H
#define GLMODELVIEWER_INTSMOOTHER_H

#include <data/LimitedQueue.h>

template <typename T>
class DataSmoother {
public:
    DataSmoother() : queue(5) {}

    void Plot(T& item) {
        queue.Add(item);
    }
    void Plot(T item) {
        queue.Add(item);
    }

    T GetLatest() {
        if(queue.GetVector().empty())
            return 0;
        return *(queue.GetVector().end()-1);
    }

    T Smooth() {
        auto vec = queue.GetVector();
        if(vec.empty())
            return 0;

        // TODO implement better smoothing algorithm
        T v = 0;
        for (const auto &item : vec)
            v += item;
        return v / vec.size();
    }

protected:
    LimitedQueue<T> queue;
};

#endif//GLMODELVIEWER_INTSMOOTHER_H
