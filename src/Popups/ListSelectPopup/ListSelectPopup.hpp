#pragma once

class ListSelectPopup : public geode::Popup {
public:
    static ListSelectPopup* create();

protected:
    cocos2d::CCLayerColor* m_scrollBG = nullptr;
    geode::ScrollLayer* m_listScroll = nullptr;
    geode::ListBorders* m_scrollBorders = nullptr;

    cocos2d::CCMenu* m_importExportMenu = nullptr;
    CCMenuItemSpriteExtra* m_importBtn = nullptr;
    CCMenuItemSpriteExtra* m_exportBtn = nullptr;

    std::vector<int> m_checkedListIDs;

    bool init();
};