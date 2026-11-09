#include "ListSelectPopup.hpp"

using namespace geode::prelude;

ListSelectPopup* ListSelectPopup::create() {
	auto ret = new ListSelectPopup();
	if (ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool ListSelectPopup::init() {
    if (!Popup::init(280.0f, 280.0f)) return false;

    setTitle("List Selection");

	m_scrollBG = CCLayerColor::create({ 0, 0, 0, 51 });
	m_scrollBG->setContentSize({ 240.0f, 200.0f });
	m_scrollBG->setAnchorPoint({ 0.5f, 0.5f });
	m_scrollBG->ignoreAnchorPointForPosition(false);
	m_mainLayer->addChildAtPosition(
		m_scrollBG, Anchor::Center, { 0.0f, 2.0f }
	);

	m_listScroll = ScrollLayer::create(m_scrollBG->getContentSize() - ccp(20.0f, 20.0f));
    m_listScroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

	m_listScroll->setAnchorPoint({ 0.5f, 0.5f });
	m_listScroll->setPosition(m_scrollBG->getPosition());
	m_listScroll->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(
		m_listScroll, Anchor::Center, { 0.0f, 2.0f }
	);

	m_scrollBorders = ListBorders::create();
    m_scrollBorders->setContentSize(m_scrollBG->getContentSize());
	m_scrollBorders->setPosition(m_scrollBG->getPosition());
    m_mainLayer->addChildAtPosition(
		m_scrollBorders, Anchor::Center, { 0.0f, 2.0f }
	);

	auto importBtnSpr = ButtonSprite::create(
		"Import", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_01.png"
	);
	importBtnSpr->setScale(0.8f);

	m_importBtn = CCMenuItemExt::createSpriteExtra(
		importBtnSpr, [this](auto) {
			
		}
	);
	m_importBtn->setID("import-button");

	auto exportBtnSpr = ButtonSprite::create(
		"Export", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_02.png"
	);
	exportBtnSpr->setScale(0.8f);

	m_exportBtn = CCMenuItemExt::createSpriteExtra(
		exportBtnSpr, [this](auto) {
			
		}
	);
	m_exportBtn->setID("export-button");

	m_importExportMenu = CCMenu::create(m_importBtn, m_exportBtn, nullptr);
	m_importExportMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Between)
		->setCrossAxisAlignment(CrossAxisAlignment::Center)
		->setMainAxisDirection(AxisDirection::LeftToRight)
		->setMainAxisScaling(AxisScaling::Fit)
		->setCrossAxisScaling(AxisScaling::Fit)
		->setGap(8.0f)
	);
	m_importExportMenu->setContentSize({});
	m_importExportMenu->ignoreAnchorPointForPosition(false);
	m_importExportMenu->setID("import-export-menu");
	m_mainLayer->addChildAtPosition(
		m_importExportMenu, Anchor::Bottom, { 0.0f, 24.0f }
	);

	m_importExportMenu->updateLayout();

    return true;
}