#include <Geode/modify/LevelSearchLayer.hpp>
#include "../Layers/RouletteLayer/RouletteLayer.hpp"

using namespace geode::prelude;

class $modify(LevelSearchLayer) {
	bool init(int type) {
		if (!LevelSearchLayer::init(type)) return false;

		auto rightFilterMenu = getChildByID("other-filter-menu");
		if (!rightFilterMenu) return true;

		auto logo = createModLogo(Mod::get());
		auto rouletteBtnSpr = CircleButtonSprite::create(
			logo, CircleBaseColor::Green, CircleBaseSize::Medium
		);

		auto rouletteBtn = CCMenuItemExt::createSpriteExtra(
			rouletteBtnSpr, [](auto) {
				CCDirector::get()->pushScene(
					CCTransitionFade::create(0.5f, RouletteLayer::scene())
				);
			}
		);
		rouletteBtn->setID("roulette-btn"_spr);

		rightFilterMenu->addChild(rouletteBtn);
		rightFilterMenu->updateLayout();

		return true;
	}
};