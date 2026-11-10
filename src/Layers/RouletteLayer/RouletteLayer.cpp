#include "RouletteLayer.hpp"
#include "../../Models/ListManager/ListManager.hpp"
#include "../../Popups/ListSelectPopup/ListSelectPopup.hpp"
#include "SimpleLevelCell.hpp"

using namespace geode::prelude;
using namespace TailyUI;

RouletteLayer* RouletteLayer::create() {
	auto ret = new RouletteLayer();
	if (ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

CCScene* RouletteLayer::scene() {
	auto ret = CCScene::create();
	AppDelegate::get()->m_runningScene = ret;
	ret->addChild(RouletteLayer::create());
	return ret;
}

bool RouletteLayer::init() {
	if (!CCLayer::init()) return false;

	setID("RouletteLayer");
	setAnchorPoint({ 0.0f, 0.0f });

	m_bg = TailyUI::RepeatBG::create("game_bg_01_001.png");
	m_bg->setID("background");
	addChildAtPosition(
		m_bg, Anchor::BottomLeft, {}
	);

	float width = std::min(getContentWidth(), 750.0f);
	float padding = width > 600.0f ? 20.0f : 0.0f;

	m_mainContent = CCNode::create();
	m_mainContent->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Between)
		->setMainAxisScaling(AxisScaling::None)
		->setCrossAxisScaling(AxisScaling::None)
		->setPadding(Padding::horizontal(padding))
	);
	m_mainContent->setContentSize({
		width, getContentHeight()
	});
	m_mainContent->setAnchorPoint({ 0.5f, 0.5f });
	m_mainContent->setID("main-content");
	addChildAtPosition(
		m_mainContent, Anchor::Center, {}
	);

	m_leftContent = CCNode::create();
	m_leftContent->setContentSize({
		std::min(width * 0.6f, 340.0f),
		m_mainContent->getContentHeight()
	});
	m_leftContent->setAnchorPoint({ 0.0f, 0.5f });
	m_leftContent->setID("left-content");
	m_mainContent->addChildAtPosition(
		m_leftContent, Anchor::Left, {}
	);

	m_rightContent = CCNode::create();
	m_rightContent->setContentSize({
		std::min(width * 0.4f, 230.0f),
		m_mainContent->getContentHeight()
	});
	m_rightContent->setAnchorPoint({ 1.0f, 0.5f });
	m_rightContent->setID("right-content");
	m_mainContent->addChildAtPosition(
		m_rightContent, Anchor::Right, {}
	);

	m_mainContent->updateLayout();

	m_fortuneWheel = FortuneWheel::create();
	m_fortuneWheel->setID("fortune-wheel");
	m_leftContent->addChildAtPosition(
		m_fortuneWheel, Anchor::Center, { 0.0f, -21.0f }
	);

	m_levelBG = NineSlice::create("square02b_001.png");
	m_levelBG->setContentSize({
		(m_leftContent->getContentWidth() - 20.0f) * 2.0f,
		32.0f * 2.0f
	});
	m_levelBG->setScale(0.5f);
	m_levelBG->setColor({ 0, 0, 0 });
	m_levelBG->setOpacity(51);
	m_levelBG->setID("level-bg");
	m_leftContent->addChildAtPosition(
		m_levelBG, Anchor::Top, { 0.0f, -26.0f }
	);

	m_levelName = Label::create(
		"Artemis 13", "bigFont.fnt"
	);
	m_levelName->setScale(0.7f);
	m_levelName->setID("selected-level-name");
	m_leftContent->addChildAtPosition(
		m_levelName, Anchor::Top, { 0.0f, -19.0f }
	);

	m_creatorName = Label::create(
		"By mvngo", "goldFont.fnt"
	);
	m_creatorName->setScale(0.5f);
	m_creatorName->setID("selected-level-creator");
	m_leftContent->addChildAtPosition(
		m_creatorName, Anchor::Top, { 0.0f, -34.0f }
	);

	m_mainMenu = CCMenu::create();
	m_mainMenu->setLayout(
		SimpleColumnLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::End)
		->setCrossAxisAlignment(CrossAxisAlignment::Center)
		->setMainAxisDirection(AxisDirection::BottomToTop)
		->setCrossAxisScaling(AxisScaling::None)
		->setGap(5.0f)
	);
	m_mainMenu->setContentSize(m_rightContent->getContentSize() - ccp(20.0f, 20.0f));
	m_mainMenu->ignoreAnchorPointForPosition(false);
	m_mainMenu->setID("main-menu");
	m_rightContent->addChildAtPosition(
		m_mainMenu, Anchor::Center, {}
	);

	int btnWidth = static_cast<int>(m_mainMenu->getContentWidth()) - 16;
	int halfBtnWidth = btnWidth / 2 - 12;

	auto closeBtnSpr = ButtonSprite::create(
		"Close", btnWidth, btnWidth, 0.8f, false,
		"goldFont.fnt", "GJ_button_06.png"
	);

	m_closeBtn = CCMenuItemExt::createSpriteExtra(
		closeBtnSpr, [](auto) {
			CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
		}
	);
	m_closeBtn->setID("close-button");
	m_mainMenu->addChild(m_closeBtn);

	auto spinBtnSpr = ButtonSprite::create(
		"Spin", halfBtnWidth, halfBtnWidth, 0.8f, false,
		"bigFont.fnt", "GJ_button_01.png"
	);

	m_spinBtn = CCMenuItemExt::createSpriteExtra(
		spinBtnSpr, [this](auto) {

		}
	);
	m_spinBtn->setID("spin-button");

	auto skipBtnSpr = ButtonSprite::create(
		"Skip", halfBtnWidth, halfBtnWidth, 0.8f, false,
		"bigFont.fnt", "GJ_button_03.png"
	);

	m_skipBtn = CCMenuItemExt::createSpriteExtra(
		skipBtnSpr, [this](auto) {

		}
	);
	m_skipBtn->setID("skip-button");

	auto rouletteBtnMenu = CCMenu::create(m_spinBtn, m_skipBtn, nullptr);
	rouletteBtnMenu->setContentSize({ m_mainMenu->getContentWidth(), 40.0f });
	rouletteBtnMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Between)
		->setMainAxisScaling(AxisScaling::None)
		->setCrossAxisScaling(AxisScaling::None)
		->setGap(10.0f)
	);
	rouletteBtnMenu->setID("roulette-button-container");
	m_mainMenu->addChild(rouletteBtnMenu);

	m_levelListMenu = CCMenu::create();
	m_levelListMenu->setContentSize(
		m_mainMenu->getContentSize() - ccp(0.0f, m_closeBtn->getContentHeight() + rouletteBtnMenu->getContentHeight() + 10.0f)
	);
	m_levelListMenu->setID("level-list-menu");
	m_mainMenu->addChild(m_levelListMenu);

	m_mainMenu->updateLayout();

	m_levelListBG = NineSlice::create("square01_001.png");
	m_levelListBG->setContentSize(m_levelListMenu->getContentSize() * 1.5f);
	m_levelListBG->setScale(1.0f / 1.5f);
	m_levelListBG->setID("level-list-bg");
	m_levelListMenu->addChildAtPosition(
		m_levelListBG, Anchor::Center, {}
	);

	m_selectedListLabel = Label::create("", "goldFont.fnt");
	m_selectedListLabel->setScale(0.75f);
	m_selectedListLabel->setVisible(false);
	m_selectedListLabel->setID("level-list-name");
	m_levelListMenu->addChildAtPosition(
		m_selectedListLabel, Anchor::Top, { 0.0f, -23.0f }
	);

	auto selectListBtnSpr = CCSprite::createWithSpriteFrameName("GJ_viewListsBtn_001.png");
	selectListBtnSpr->setScale(0.6f);

	m_selectListBtn = CCMenuItemExt::createSpriteExtra(
		selectListBtnSpr, [](auto) {
			ListSelectPopup::create()->show();
		}
	);
	m_selectListBtn->setID("select-list-button");
	m_levelListMenu->addChildAtPosition(
		m_selectListBtn, Anchor::TopRight, { -25.0f, -23.0f }
	);

	m_levelScroll = ScrollLayer::create(m_levelListMenu->getContentSize() - ccp(20.0f, 50.0f));
	m_levelScroll->ignoreAnchorPointForPosition(false);
	auto content = m_levelScroll->m_contentLayer;

	content->setLayout(
		ScrollLayer::createDefaultListLayout()
	);
	m_levelScroll->setID("level-scroll");
	m_levelListMenu->addChildAtPosition(
		m_levelScroll, Anchor::Center, { 0.0f, -15.0f }
	);

	m_noListLabel = Label::create(
		"No list selected", "bigFont.fnt"
	);
	m_noListLabel->setScale(0.5f);
	m_noListLabel->setVisible(false);
	m_noListLabel->setID("no-list-text");
	m_levelListMenu->addChildAtPosition(
		m_noListLabel, Anchor::Center, {}
	);

	loadCurrentList();

	setKeyboardEnabled(true);
	setKeypadEnabled(true);

	return true;
}

void RouletteLayer::loadCurrentList() {
	auto currentList = ListManager::getCurrentList();

	if (!currentList) {
		m_noListLabel->setVisible(true);
		return;
	}

	m_selectedListLabel->setString(currentList->name.c_str());
	m_selectedListLabel->setVisible(true);

	for (auto& level : currentList->levels) {
		auto cell = SimpleLevelCell::create(
			level, m_levelScroll->getContentWidth()
		);
		m_levelScroll->m_contentLayer->addChild(cell);
	}
	m_levelScroll->m_contentLayer->updateLayout();
	m_levelScroll->scrollToTop();
}

void RouletteLayer::keyBackClicked() {
	CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
}