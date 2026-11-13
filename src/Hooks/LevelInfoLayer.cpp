#include <Geode/modify/LevelInfoLayer.hpp>
#include "../Popups/ListSelectPopup/ListSelectPopup.hpp"
#include "../Utils/DifficultyFromLevel.hpp"

using namespace geode::prelude;

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        auto rightSideMenu = getChildByID("right-side-menu");
		if (!rightSideMenu) return true;

		auto logo = createModLogo(Mod::get());
		auto rouletteBtnSpr = CircleButtonSprite::create(
			logo, CircleBaseColor::Green, CircleBaseSize::Small
		);

		auto rouletteBtn = CCMenuItemExt::createSpriteExtra(
			rouletteBtnSpr, [this, level](auto) {
                LevelData levelData{
                    level->m_levelID, level->m_levelName,
                    level->m_creatorName, Utils::difficultyFromLevel(level)
                };

                ListSelectPopup::create({ levelData })->show();
			}
		);
        rouletteBtn->_setZOrder(1);
		rouletteBtn->setID("roulette-btn"_spr);

		rightSideMenu->addChild(rouletteBtn);
		rightSideMenu->updateLayout();
        
        return true;
    }
};