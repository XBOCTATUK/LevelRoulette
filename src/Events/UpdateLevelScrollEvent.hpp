#pragma once

class UpdateLevelScrollEvent : public geode::Event<UpdateLevelScrollEvent, bool()> {
public:
    using Event::Event;
};