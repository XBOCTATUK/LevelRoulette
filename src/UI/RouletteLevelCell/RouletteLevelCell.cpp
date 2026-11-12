#include "RouletteLevelCell.hpp"

using namespace geode::prelude;

TailyUI::RouletteLevelCell* TailyUI::RouletteLevelCell::create(const LevelData& levelData, float width, bool showButtons) {
	auto ret = new RouletteLevelCell();
	if (ret->init(levelData, width, showButtons)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool TailyUI::RouletteLevelCell::init(const LevelData& levelData, float width, bool showButtons) {
	if (!CCNode::init()) return false;

	setContentSize({ width, 32.0f });

	m_bg = NineSlice::create("square02b_001.png");
	m_bg->setContentSize({
		getContentWidth() * 2.0f, getContentHeight() * 2.0f
	});
	m_bg->setScale(0.5f);
	m_bg->setColor({ 0, 0, 0 });
	m_bg->setOpacity(51);
	m_bg->setID("bg");
	addChildAtPosition(
		m_bg, Anchor::Center, {}
	);

	m_levelName = Label::create(
		levelData.levelName, "bigFont.fnt"
	);
	m_levelName->setLimitLabelWidth(width - (showButtons ? 45.0f : 10.0f), 0.45f, 0.35f);
	m_levelName->setAnchorPoint({ 0.0f, 1.0f });
	m_levelName->setID("level-name");
	addChildAtPosition(
		m_levelName, Anchor::TopLeft,
		{ 5.0f, -2.0f }
	);

	m_creatorName = Label::create(
		levelData.creatorName, "goldFont.fnt"
	);
	m_creatorName->setScale(0.4f);
	m_creatorName->setAnchorPoint({ 0.0f, 0.0f });
	m_creatorName->setID("creator-name");
	addChildAtPosition(
		m_creatorName, Anchor::BottomLeft,
		{ 5.0f, 3.0f }
	);

	if (!showButtons) return true;

	m_checkToggler = CCMenuItemExt::createTogglerWithStandardSprites(
		0.6f, [this, &levelData](CCMenuItemToggler* self) {
			if (m_callback) {
				m_callback(levelData.levelID, !self->isToggled());
			}
		}
	);
	m_checkToggler->setID("check-toggler");

	m_btnMenu = CCMenu::create(
		m_checkToggler, nullptr
	);
	m_btnMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisScaling(AxisScaling::Fit)
		->setCrossAxisScaling(AxisScaling::Fit)
		->setGap(3.0f)
	);
	m_btnMenu->setAnchorPoint({ 1.0f, 0.5f });
	m_btnMenu->ignoreAnchorPointForPosition(false);
	m_btnMenu->setID("button-menu");
	addChildAtPosition(
		m_btnMenu, Anchor::Right, { -10.0f, 0.0f }
	);

	m_btnMenu->updateLayout();

	return true;
}

void TailyUI::RouletteLevelCell::setCheckCallback(geode::Function<void(int, bool)> callback) {
	m_callback = std::move(callback);
}