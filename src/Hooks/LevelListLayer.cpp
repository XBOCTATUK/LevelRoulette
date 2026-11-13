#include <Geode/modify/LevelListLayer.hpp>
#include "../Popups/ListSelectPopup/ListSelectPopup.hpp"
#include "../Models/LevelData.hpp"
#include "../Utils/DifficultyFromLevel.hpp"

using namespace geode::prelude;

class $modify(LevelListLayer) {
    struct Fields {
        std::vector<LevelData> m_levelsToAdd{};
    };

    void loadLevelsFinished(CCArray* levels, char const* key, int type) {
        LevelListLayer::loadLevelsFinished(levels, key, type);
        
        for (const auto& level : CCArrayExt<GJGameLevel*>(levels)) {
            LevelData levelData{
                level->m_levelID, level->m_levelName,
                level->m_creatorName, Utils::difficultyFromLevel(level)
            };

            m_fields->m_levelsToAdd.push_back(levelData);
        }
    }

    bool init(GJLevelList* list) {
        if (!LevelListLayer::init(list)) return false;
        
        auto rightSideMenu = getChildByID("right-side-menu");
		if (!rightSideMenu) return true;

		auto logo = createModLogo(Mod::get());
		auto rouletteBtnSpr = CircleButtonSprite::create(
			logo, CircleBaseColor::Green, CircleBaseSize::Small
		);

		auto rouletteBtn = CCMenuItemExt::createSpriteExtra(
			rouletteBtnSpr, [this](auto) {
				if (m_fields->m_levelsToAdd.empty()) {
                    FLAlertLayer::create(
                        "No levels",
                        "List is empty or levels are still loading. Please try again.",
                        "Ok"
                    )->show();

                    return;
                }

                ListSelectPopup::create(m_fields->m_levelsToAdd)->show();
			}
		);
        rouletteBtn->_setZOrder(1);
		rouletteBtn->setID("roulette-btn"_spr);

		rightSideMenu->addChild(rouletteBtn);
		rightSideMenu->updateLayout();

        return true;
    }
};