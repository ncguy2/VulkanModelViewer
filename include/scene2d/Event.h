//
// Created by Guy on 30/06/2021.
//

#ifndef GLMODELVIEWER_EVENT_H
#define GLMODELVIEWER_EVENT_H

#include "Scene2d.h"

class Stage;
class Actor;

class Event {
public:
    void Handle();
    void Cancel();
    void Reset();

    Stage* stage = nullptr;
    Actor* targetActor = nullptr;
    Actor* listeningActor = nullptr;

    bool capture = false; // Event occurred during capture phase
    bool bubbles = true; // Propagate to target parents
    bool handled = false; // Event was handled
    bool stopped = false; // Event propagation stopped
    bool cancelled = false; // Propagation stopped and actions should not happen
};

#endif//GLMODELVIEWER_EVENT_H
