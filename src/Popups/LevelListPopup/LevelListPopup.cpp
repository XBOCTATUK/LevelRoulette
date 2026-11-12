#include "LevelListPopup.hpp"
#include "../../Models/ListManager/ListManager.hpp"
#include "../../UI/RouletteLevelCell/RouletteLevelCell.hpp"
#include "../../Events/UpdateLevelScrollEvent.hpp"
#include "../../Events/UpdateListCellEvent.hpp"
#include "../../Events/UpdateCurrentListEvent.hpp"

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

	m_levelScroll = ScrollLayer::create(m_scrollBG->getContentSize());
	m_levelScroll->m_contentLayer->setLayout(
		SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setMainAxisAlignment(MainAxisAlignment::End)
        ->setMainAxisScaling(AxisScaling::Fit)
		->setPadding(Padding::uniform(10.0f))
        ->setGap(2.5f)
        ->ignoreInvisibleChildren(false)
	);
	m_levelScroll->setAnchorPoint({ 0.5f, 0.5f });
	m_levelScroll->ignoreAnchorPointForPosition(false);
	m_levelScroll->setID("level-scroll");
	m_mainLayer->addChildAtPosition(
		m_levelScroll, Anchor::Center, { 0.0f, 2.0f }
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

	auto deleteBtnSpr = ButtonSprite::create(
		"Delete", 80, 80, 0.8f, false,
		"goldFont.fnt", "GJ_button_06.png"
	);
	deleteBtnSpr->setScale(0.8f);

	m_deleteBtn = CCMenuItemExt::createSpriteExtra(
		deleteBtnSpr, [this, &listData](auto) {
			if (m_checkedLevelIDs.empty()) {
				FLAlertLayer::create(
					"No Levels Selected",
					"Select the levels you want to delete first.",
					"OK"
				)->show();

				return;
			}

			createQuickPopup(
				"Confirm delete",
				fmt::format("Delete {} level{}? This can't be undone.", m_checkedLevelIDs.size(), m_checkedLevelIDs.size() == 1 ? "" : "s"),
				"No", "Yes",
				[this, &listData](auto, bool yesBtn) {
					if (yesBtn) {
						for (int levelID : m_checkedLevelIDs) {
							ListManager::removeLevelFromList(listData.id, levelID);
						}
						m_checkedLevelIDs.clear();

						UpdateListCellEvent(listData.id).send();
						UpdateLevelScrollEvent().send();
						UpdateCurrentListEvent().send();
					}
				}
			);
		}
	);
	m_deleteBtn->setID("delete-button");

	m_bottomMenu = CCMenu::create(
		m_deleteBtn, nullptr
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

	m_updateScrollListener = UpdateLevelScrollEvent().listen(
		[this, &listData]() {
			auto content = m_levelScroll->m_contentLayer;
			float topY = m_levelScroll->getContentHeight() - content->getContentHeight();
			float scrollOffset = content->getPositionY() - topY;

			loadLevels(listData);

			topY = m_levelScroll->getContentHeight() - content->getContentHeight();
			content->setPositionY(
				topY + std::clamp(scrollOffset, 0.0f, std::max(-topY, 0.0f))
			);
		}
	);

	loadLevels(listData);

    return true;
}

void LevelListPopup::loadLevels(const RouletteList& listData) {
	m_levelScroll->m_contentLayer->removeAllChildren();
	m_checkedLevelIDs.clear();
	
    if (listData.levels.empty()) {
		m_noLevelsLabel->setVisible(true);
		return;
	}

	m_noLevelsLabel->setVisible(false);

	for (const auto& level : listData.levels) {
		auto cell = TailyUI::RouletteLevelCell::create(
			level, m_levelScroll->getContentWidth() - 20.0f, true
		);
		cell->setCheckCallback(
			[this](int levelID, bool checked) {
				if (checked) m_checkedLevelIDs.insert(levelID);
				else if (!checked) m_checkedLevelIDs.erase(levelID);
			}
		);
		m_levelScroll->m_contentLayer->addChild(cell);
	}

	m_levelScroll->m_contentLayer->updateLayout();
	m_levelScroll->scrollToTop();
}