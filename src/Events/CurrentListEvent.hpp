#pragma once

class CurrentListEvent : public geode::Event<CurrentListEvent, bool(int listID)> {
public:
    using Event::Event;
};