//
// Created by Guy on 30/06/2021.
//

#include <scene2d/Event.h>

void Event::Handle() {
    handled = true;
}

void Event::Cancel() {
    cancelled = true;
    stopped = true;
    handled = true;
}

void Event::Reset() {
    stage = nullptr;
    targetActor = nullptr;
    listeningActor = nullptr;
    capture = false;
    bubbles = true;
    handled = false;
    stopped = false;
    cancelled = false;
}
