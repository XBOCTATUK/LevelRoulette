#pragma once

class ListSelectPopup : public geode::Popup {
public:
	static ListSelectPopup* create();

protected:
	cocos2d::CCLayerColor* m_scrollBG = nullptr;
	geode::ScrollLayer* m_listScroll = nullptr;
	geode::ListBorders* m_scrollBorders = nullptr;
	geode::Label* m_noListsLabel = nullptr;

	CCMenuItemSpriteExtra* m_createBtn = nullptr;
	CCMenuItemSpriteExtra* m_importBtn = nullptr;
	CCMenuItemSpriteExtra* m_exportBtn = nullptr;
	CCMenuItemSpriteExtra* m_deleteBtn = nullptr;
	cocos2d::CCMenu* m_bottomMenu = nullptr;

	std::unordered_set<int> m_checkedListIDs;
	geode::ListenerHandle m_updateScrollListener;

	bool init();
	void loadLists();
};