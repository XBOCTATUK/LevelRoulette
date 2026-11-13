#pragma once

#include "../../Models/RouletteList.hpp"

class RouletteListCell : public cocos2d::CCNode {
public:
	static RouletteListCell* create(const RouletteList& listData, float width, const std::vector<LevelData>& levelsToAdd = {});
	void setCheckCallback(geode::Function<void(int listID, bool checked)> checkCallback);
	
protected:
	geode::Function<void(int listID, bool checked)> m_checkCallback = nullptr;

	geode::NineSlice* m_bg = nullptr;
	geode::Label* m_listName = nullptr;
	geode::Label* m_levelCount = nullptr;

	CCMenuItemToggler* m_checkToggler = nullptr;
	CCMenuItemSpriteExtra* m_renameBtn = nullptr;
	CCMenuItemSpriteExtra* m_levelListBtn = nullptr;
	CCMenuItemSpriteExtra* m_addLevelsBtn = nullptr;
	CCMenuItemToggler* m_useToggler = nullptr;
	cocos2d::CCMenu* m_btnMenu = nullptr;

	geode::ListenerHandle m_currentListListener;
	geode::ListenerHandle m_updateCellListener;
	
	bool init(const RouletteList& listData, float width, const std::vector<LevelData>& levelsToAdd);
};