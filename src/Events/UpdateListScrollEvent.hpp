#pragma once

class UpdateListScrollEvent : public geode::Event<UpdateListScrollEvent, bool()> {
public:
    using Event::Event;
};