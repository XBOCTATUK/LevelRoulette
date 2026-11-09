#pragma once

class ListSelectPopup : public geode::Popup {
public:
    static ListSelectPopup* create();

protected:
    bool init();
};