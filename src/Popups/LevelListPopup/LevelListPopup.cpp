#include "LevelListPopup.hpp"
#include "../../UI/SimpleLevelCell/SimpleLevelCell.hpp"

using namespace geode::prelude;

LevelListPopup* LevelListPopup::create(const RouletteList& listData) {
	auto ret = new LevelListPopup();
	if (ret->init(listData)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool LevelListPopup::init(const RouletteList& listData) {
	if (!Popup::init(400.0f, 280.0f)) return false;

	setTitle(fmt::format("'{}' list", listData.name));

    m_scrollBG = CCLayerColor::create({ 0, 0, 0, 51 });
	m_scrollBG->setContentSize(
		{ m_mainLayer->getContentWidth() - 40.0f, 200.0f }
	);
	m_scrollBG->setAnchorPoint({ 0.5f, 0.5f });
	m_scrollBG->ignoreAnchorPointForPosition(false);
	m_scrollBG->setID("scroll-bg");
	m_mainLayer->addChildAtPosition(
		m_scrollBG, Anchor::Center, { 0.0f, 2.0f }
	);

	m_listScroll = ScrollLayer::create(m_scrollBG->getContentSize());
	m_listScroll->m_contentLayer->setLayout(
		SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setMainAxisAlignment(MainAxisAlignment::End)
        ->setMainAxisScaling(AxisScaling::Fit)
		->setPadding(Padding::uniform(10.0f))
        ->setGap(2.5f)
        ->ignoreInvisibleChildren(false)
	);
	m_listScroll->setAnchorPoint({ 0.5f, 0.5f });
	m_listScroll->ignoreAnchorPointForPosition(false);
	m_listScroll->setID("list-scroll");
	m_mainLayer->addChildAtPosition(
		m_listScroll, Anchor::Center, { 0.0f, 2.0f }
	);

	m_scrollBorders = ListBorders::create();
	m_scrollBorders->setContentSize(m_scrollBG->getContentSize());
	m_scrollBorders->setID("level-borders");
	m_mainLayer->addChildAtPosition(
		m_scrollBorders, Anchor::Center, { 0.0f, 2.0f }
	);

	m_noLevelsLabel = Label::create(
		"List is empty", "bigFont.fnt"
	);
	m_noLevelsLabel->setScale(0.5f);
	m_noLevelsLabel->setVisible(false);
	m_noLevelsLabel->setID("no-levels-text");
	m_mainLayer->addChildAtPosition(
		m_noLevelsLabel, Anchor::Center, { 0.0f, 2.0f }
	);

	loadLevels(listData);

    return true;
}

void LevelListPopup::loadLevels(const RouletteList& listData) {
	m_listScroll->m_contentLayer->removeAllChildren();
	
    if (listData.levels.empty()) {
		m_noLevelsLabel->setVisible(true);
		return;
	}

	m_noLevelsLabel->setVisible(false);

	for (const auto& level : listData.levels) {
		auto cell = TailyUI::SimpleLevelCell::create(
			level, m_listScroll->getContentWidth() - 20.0f
		);
		m_listScroll->m_contentLayer->addChild(cell);
	}

	m_listScroll->m_contentLayer->updateLayout();
	m_listScroll->scrollToTop();
}