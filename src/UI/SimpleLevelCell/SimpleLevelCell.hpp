#pragma once

#include "../../Models/LevelData.hpp"

namespace TailyUI {
	class SimpleLevelCell : public cocos2d::CCNode {
	public:
		static SimpleLevelCell* create(const LevelData& levelData, float width);
		
	protected:
		geode::NineSlice* m_bg = nullptr;
		geode::Label* m_levelName = nullptr;
		geode::Label* m_creatorName = nullptr;
		
		bool init(const LevelData& levelData, float width);
	};
}