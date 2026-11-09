#pragma once

#include "../../UI/FortuneWheel/FortuneWheel.hpp"
#include "../../UI/RepeatBG/RepeatBG.hpp"

class RouletteLayer : public cocos2d::CCLayer {
public:
    static RouletteLayer* create();
    static cocos2d::CCScene* scene();

protected:
    TailyUI::RepeatBG* m_bg = nullptr;
    cocos2d::CCNode* m_mainContent = nullptr;
    cocos2d::CCNode* m_leftContent = nullptr;
    cocos2d::CCNode* m_rightContent = nullptr;

    TailyUI::FortuneWheel* m_fortuneWheel = nullptr;
    geode::NineSlice* m_levelBG = nullptr;
    geode::Label* m_levelName = nullptr;
    geode::Label* m_creatorName = nullptr;

    cocos2d::CCMenu* m_mainMenu = nullptr;
    CCMenuItemSpriteExtra* m_closeBtn = nullptr;
    CCMenuItemSpriteExtra* m_spinBtn = nullptr;
    CCMenuItemSpriteExtra* m_skipBtn = nullptr;
    CCMenuItemSpriteExtra* m_listBtn = nullptr;

    cocos2d::CCMenu* m_levelListMenu = nullptr;
    geode::NineSlice* m_levelListBG = nullptr;
    geode::Label* m_selectedListLabel = nullptr;
    CCMenuItemSpriteExtra* m_selectListBtn = nullptr;
    geode::ScrollLayer* m_levelScroll = nullptr;
    geode::Label* m_noListLabel = nullptr;

    bool init() override;
    void loadCurrentList();

    virtual void keyBackClicked() override;
};