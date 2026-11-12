#pragma once

class UpdateCurrentListEvent : public geode::Event<UpdateCurrentListEvent, bool()> {
public:
    using Event::Event;
};