#include <Geode/modify/LevelCell.hpp>

using namespace geode::prelude;

class $modify(LevelCell) {
	void loadFromLevel(GJGameLevel* level) {
		LevelCell::loadFromLevel(level);

		// log::info(
		//     "{} - {}", level->m_levelName, difficultyToString(Utils::difficultyFromLevel(level))
		// );
	}
};