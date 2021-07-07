//
// Created by Guy on 30/06/2021.
//

#ifndef GLMODELVIEWER_EVENTLISTENER_H
#define GLMODELVIEWER_EVENTLISTENER_H

class Event;

class EventListener {
public:
    /**
     * Try to handle the given event, if it is applicable
     * @return true if the event should be considered handled by scene2d
     */
    bool Handle(std::shared_ptr<Event> event)=0;
};

#endif//GLMODELVIEWER_EVENTLISTENER_H
