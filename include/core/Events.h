//
// Created by Guy on 12/06/2021.
//

#ifndef GLMODELVIEWER_EVENTS_H
#define GLMODELVIEWER_EVENTS_H

#include <vector>
#include <functional>

template <typename... ARGS>
struct Delegate {

    typedef std::function<void(ARGS...)> Signature;

    std::vector<std::function<void(ARGS...)>*> funcs;

    template<class F>
    Delegate<ARGS...>& operator += (F& func) {
        funcs.push_back(&func);
        return *this;
    }

    template<class F>
    Delegate<ARGS...>& operator -= (F& func) {
        auto r = std::remove(funcs.begin(), funcs.end(), &func);
        return *this;
    }

    void operator()(ARGS... args) {
        for(auto& f : funcs) (*f)(args...);
    }
};

#endif//GLMODELVIEWER_EVENTS_H
