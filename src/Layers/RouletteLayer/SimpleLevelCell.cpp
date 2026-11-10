#include "SimpleLevelCell.hpp"

using namespace geode::prelude;

SimpleLevelCell* SimpleLevelCell::create(const LevelData& levelData, float width) {
	auto ret = new SimpleLevelCell();
	if (ret->init(levelData, width)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool SimpleLevelCell::init(const LevelData& levelData, float width) {
	if (!CCNode::init()) return false;

	setContentSize({ width, 32.0f });

	m_bg = NineSlice::create("square02b_001.png");
	m_bg->setContentSize({
		getContentWidth() * 2.0f, getContentHeight() * 2.0f
	});
	m_bg->setScale(0.5f);
	m_bg->setColor({ 0, 0, 0 });
	m_bg->setOpacity(51);
	m_bg->setID("background");
	addChildAtPosition(
		m_bg, Anchor::Center, {}
	);

	m_levelName = Label::create(
		levelData.levelName, "bigFont.fnt"
	);
	m_levelName->setLimitLabelWidth(width - 10.0f, 0.45f, 0.35f);
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

	return true;
}