#pragma once

#include "../../Models/LevelData.hpp"

namespace TailyUI {
	class RouletteLevelCell : public cocos2d::CCNode {
	public:
		static RouletteLevelCell* create(const LevelData& levelData, float width, bool showButtons = false);
		void setCheckCallback(geode::Function<void(int levelID, bool checked)> checkCallback);
		
	protected:
		geode::Function<void(int levelID, bool checked)> m_checkCallback = nullptr;

		geode::NineSlice* m_bg = nullptr;
		geode::Label* m_levelName = nullptr;
		geode::Label* m_creatorName = nullptr;

		CCMenuItemToggler* m_checkToggler = nullptr;
		cocos2d::CCMenu* m_btnMenu = nullptr;
		
		bool init(const LevelData& levelData, float width, bool showButtons);
	};
}