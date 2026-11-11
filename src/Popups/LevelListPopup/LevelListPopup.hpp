#pragma once

#include "../../Models/RouletteList.hpp"

class LevelListPopup : public geode::Popup {
public:
    static LevelListPopup* create(const RouletteList& listData);

protected:
    cocos2d::CCLayerColor* m_scrollBG = nullptr;
	geode::ScrollLayer* m_listScroll = nullptr;
	geode::ListBorders* m_scrollBorders = nullptr;
	geode::Label* m_noLevelsLabel = nullptr;

    bool init(const RouletteList& listData);
    void loadLevels(const RouletteList& listData);
};