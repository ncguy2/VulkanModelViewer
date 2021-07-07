//
// Created by Guy on 30/06/2021.
//

#ifndef GLMODELVIEWER_ACTOR_H
#define GLMODELVIEWER_ACTOR_H

#include <scene2d/Scene2d.h>

class Actor {
public:
    void Draw(vk::CommandBuffer buffer);
    bool Fire(std::shared_ptr<Event> event);
    bool Notify(std::shared_ptr<Event> event, bool capture);
    Actor* Hit(float x, float y, bool touchable);
    bool Remove();
    bool AddListener(std::shared_ptr<EventListener> eventListener);
    bool RemoveListener(std::shared_ptr<EventListener> eventListener);

    void ClearListeners();

    bool IsDescendantOf(Actor* actor);
    bool IsAscendantOf(Actor* actor);

    bool AscendantsVisible();

protected:
    Stage* stage;
    Group* parent;
    Touchable touchable = touch_enabled;
    bool visible = true;
    bool debug = false;

    SceneRect sceneRect;
    Colour colour = new Colour(1, 1, 1, 1);
};

#endif//GLMODELVIEWER_ACTOR_H
