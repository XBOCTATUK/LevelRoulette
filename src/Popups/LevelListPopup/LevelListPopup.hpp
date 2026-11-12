#pragma once

#include "../../Models/RouletteList.hpp"

class LevelListPopup : public geode::Popup {
public:
    static LevelListPopup* create(const RouletteList& listData);

protected:
    cocos2d::CCLayerColor* m_scrollBG = nullptr;
	geode::ScrollLayer* m_levelScroll = nullptr;
	geode::ListBorders* m_scrollBorders = nullptr;
	geode::Label* m_noLevelsLabel = nullptr;

    CCMenuItemSpriteExtra* m_deleteBtn = nullptr;
	cocos2d::CCMenu* m_bottomMenu = nullptr;

    std::unordered_set<int> m_checkedLevelIDs;

    bool init(const RouletteList& listData);
    void loadLevels(const RouletteList& listData);
};