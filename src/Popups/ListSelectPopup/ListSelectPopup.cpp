#include "ListSelectPopup.hpp"
#include "../../Models/ListManager/ListManager.hpp"
#include "../InputPopup/InputPopup.hpp"
#include "RouletteListCell.hpp"

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
	if (!Popup::init(400.0f, 280.0f)) return false;

	setTitle("Select a List");

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

	m_noListsLabel = Label::create(
		"You have no lists", "bigFont.fnt"
	);
	m_noListsLabel->setScale(0.5f);
	m_noListsLabel->setVisible(false);
	m_noListsLabel->setID("no-list-text");
	m_mainLayer->addChildAtPosition(
		m_noListsLabel, Anchor::Center, { 0.0f, 2.0f }
	);

	auto createBtnSpr = ButtonSprite::create(
		"Create", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_01.png"
	);
	createBtnSpr->setScale(0.8f);

	m_createBtn = CCMenuItemExt::createSpriteExtra(
		createBtnSpr, [this](auto) {
			InputPopup::create(
				"Create a list", "", "Create",
				[](std::string listName) {
					if (listName.empty()) {
						FLAlertLayer::create(
							"Empty list name",
							"The list name cannot be empty.",
							"Ok"
						)->show();

						return;
					}

					ListManager::createList(listName);
				}
			)->show();
		}
	);
	m_createBtn->setID("create-button");

	auto importBtnSpr = ButtonSprite::create(
		"Import", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_02.png"
	);
	importBtnSpr->setScale(0.8f);

	m_importBtn = CCMenuItemExt::createSpriteExtra(
		importBtnSpr, [this](auto) {
			ListManager::importLists();
			loadLists();
		}
	);
	m_importBtn->setID("import-button");

	auto exportBtnSpr = ButtonSprite::create(
		"Export", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_03.png"
	);
	exportBtnSpr->setScale(0.8f);

	m_exportBtn = CCMenuItemExt::createSpriteExtra(
		exportBtnSpr, [this](auto) {
			if (m_checkedListIDs.empty()) {
				FLAlertLayer::create(
					"No Lists Selected",
					"Select the lists you want to export first.",
					"OK"
				)->show();

				return;
			}

			ListManager::exportLists(m_checkedListIDs);
		}
	);
	m_exportBtn->setID("export-button");

	auto deleteBtnSpr = ButtonSprite::create(
		"Delete", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_06.png"
	);
	deleteBtnSpr->setScale(0.8f);

	m_deleteBtn = CCMenuItemExt::createSpriteExtra(
		deleteBtnSpr, [this](auto) {
			if (m_checkedListIDs.empty()) {
				FLAlertLayer::create(
					"No Lists Selected",
					"Select the lists you want to delete first.",
					"OK"
				)->show();

				return;
			}

			createQuickPopup(
				"Confirm delete",
				fmt::format("Delete {} list{}? This can't be undone.", m_checkedListIDs.size(), m_checkedListIDs.size() == 1 ? "" : "s"),
				"No", "Yes",
				[this](auto, bool yesBtn) {
					if (yesBtn) {
						for (int listID : m_checkedListIDs) {
							ListManager::removeList(listID);
						}
						m_checkedListIDs.clear();

						loadLists();
					}
				}
			);
		}
	);
	m_deleteBtn->setID("delete-button");

	m_bottomMenu = CCMenu::create(
		m_createBtn, m_importBtn, m_exportBtn, m_deleteBtn, nullptr
	);
	m_bottomMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Between)
		->setCrossAxisAlignment(CrossAxisAlignment::Center)
		->setMainAxisDirection(AxisDirection::LeftToRight)
		->setMainAxisScaling(AxisScaling::Fit)
		->setCrossAxisScaling(AxisScaling::Fit)
		->setGap(10.0f)
	);
	m_bottomMenu->ignoreAnchorPointForPosition(false);
	m_bottomMenu->setID("bottom-menu");
	m_mainLayer->addChildAtPosition(
		m_bottomMenu, Anchor::Bottom, { 0.0f, 24.0f }
	);

	m_bottomMenu->updateLayout();

	loadLists();

	return true;
}

void ListSelectPopup::loadLists() {
	m_listScroll->m_contentLayer->removeAllChildren();

	const auto& lists = ListManager::getLists();
	if (lists.empty()) {
		m_noListsLabel->setVisible(true);
		return;
	}

	m_noListsLabel->setVisible(false);

	for (const auto& [key, list] : lists) {
		auto cell = RouletteListCell::create(
			list, m_listScroll->getContentWidth() - 20.0f,
			[this](int listID, bool checked) {
				if (checked) m_checkedListIDs.insert(listID);
				else if (!checked) m_checkedListIDs.erase(listID);
			}
		);
		m_listScroll->m_contentLayer->addChild(cell);
	}

	m_listScroll->m_contentLayer->updateLayout();
	m_listScroll->scrollToTop();
}