#pragma once

class UpdateListCellEvent : public geode::Event<UpdateListCellEvent, bool(), int> {
public:
    using Event::Event;
};