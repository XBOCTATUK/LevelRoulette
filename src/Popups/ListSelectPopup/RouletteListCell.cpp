#include "RouletteListCell.hpp"
#include "../../Models/ListManager/ListManager.hpp"
#include "../LevelListPopup/LevelListPopup.hpp"
#include "../InputPopup/InputPopup.hpp"
#include "../../Events/CurrentListEvent.hpp"
#include "../../Events/UpdateListCellEvent.hpp"

using namespace geode::prelude;

RouletteListCell* RouletteListCell::create(const RouletteList& listData, float width, const std::vector<LevelData>& levelsToAdd) {
	auto ret = new RouletteListCell();
	if (ret->init(listData, width, levelsToAdd)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteListCell::init(const RouletteList& listData, float width, const std::vector<LevelData>& levelsToAdd) {
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

	m_listName = Label::create(
		listData.name, "bigFont.fnt"
	);
	m_listName->setLimitLabelWidth(200.0f, 0.45f, 0.3f);
	m_listName->setAnchorPoint({ 0.0f, 1.0f });
	m_listName->setID("list-name");
	addChildAtPosition(
		m_listName, Anchor::TopLeft,
		{ 5.0f, -2.0f }
	);

	auto levelCountStr = listData.levels.empty()
	? "Empty list"
	: fmt::format("{} level{}", listData.levels.size(), listData.levels.size() == 1 ? "" : "s");

	m_levelCount = Label::create(
		levelCountStr, "goldFont.fnt"
	);
	m_levelCount->setScale(0.4f);
	m_levelCount->setAnchorPoint({ 0.0f, 0.0f });
	m_levelCount->setID("level-count-text");
	addChildAtPosition(
		m_levelCount, Anchor::BottomLeft,
		{ 5.0f, 3.0f }
	);

	m_checkToggler = CCMenuItemExt::createTogglerWithStandardSprites(
		0.6f, [this, &listData](CCMenuItemToggler* self) {
			if (m_checkCallback) {
				m_checkCallback(listData.id, !self->isToggled());
			}
		}
	);
	m_checkToggler->setID("check-toggler");

	auto renameBtnSpr = CircleButtonSprite::createWithSprite(
		"pencil-icon.png"_spr, 0.8f,
		CircleBaseColor::Green, CircleBaseSize::Tiny
	);

	m_renameBtn = CCMenuItemExt::createSpriteExtra(
		renameBtnSpr, [this, &listData](auto) {
			auto inputPopup = InputPopup::create(
				"Rename list", listData.name, "Set",
				[this, &listData](std::string newListName) {
					auto oldListName = listData.name;
					ListManager::renameList(listData.id, newListName);

					Notification::create(
						fmt::format("Renamed '{}' to '{}'", oldListName, newListName),
						NotificationIcon::Success
					)->show();
				}
			);
			inputPopup->setCharLimits(1, 32);
			inputPopup->show();
		}
	);
	m_renameBtn->setID("rename-button");

	m_levelListBtn = CCMenuItemExt::createSpriteExtraWithFrameName(
		"GJ_viewListsBtn_001.png", 0.57f, [this, &listData](auto) {
			LevelListPopup::create(listData)->show();
		}
	);
	m_levelListBtn->setID("level-list-button");

	if (levelsToAdd.empty()) {
		auto useBtnSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
		useBtnSpr->setScale(28.0f / useBtnSpr->getContentHeight());
		auto inUseBtnSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png");
		inUseBtnSpr->setScale(28.0f / inUseBtnSpr->getContentHeight());

		m_useToggler = CCMenuItemExt::createToggler(
			inUseBtnSpr, useBtnSpr,
			[this, &listData](CCMenuItemToggler* self) {
				if (!self->isToggled()) {
					ListManager::setCurrentList(listData.id);

					Notification::create(
						"List selected",
						NotificationIcon::Success
					)->show();
				}
			}
		);
		m_useToggler->toggle(ListManager::isCurrentList(listData.id));
		m_useToggler->setID("use-toggler");

		m_btnMenu = CCMenu::create(
			m_checkToggler, m_renameBtn, m_levelListBtn, m_useToggler, nullptr
		);
	}
	else {
		auto addLevelsBtnSpr = CircleButtonSprite::createWithSpriteFrameName(
			"GJ_listAddBtn_001.png", 1.0f,
			CircleBaseColor::Green, CircleBaseSize::Tiny
		);

		m_addLevelsBtn = CCMenuItemExt::createSpriteExtra(
			addLevelsBtnSpr, [this, &listData, levelsToAdd](auto) {
				ListManager::addLevelsToList(listData.id, levelsToAdd);

				Notification::create(
					fmt::format("Level{} added", levelsToAdd.size() == 1 ? "" : "s"),
					NotificationIcon::Success
				)->show();
			}
		);
		m_addLevelsBtn->setID("add-levels-button");

		m_btnMenu = CCMenu::create(
			m_checkToggler, m_renameBtn, m_levelListBtn, m_addLevelsBtn, nullptr
		);
	}

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
		m_btnMenu, Anchor::Right, { -5.0f, 0.0f }
	);

	m_btnMenu->updateLayout();

	if (levelsToAdd.empty()) {	
		m_currentListListener = CurrentListEvent().listen(
			[this, ownListID = listData.id](int listID) {
				m_useToggler->toggle(ownListID == listID);
				m_useToggler->setEnabled(ownListID != listID);
			}
		);
	}

	m_updateCellListener = UpdateListCellEvent(listData.id).listen(
		[this, &listData]() {
			auto levelCountStr = listData.levels.empty()
			? "Empty list"
			: fmt::format("{} level{}", listData.levels.size(), listData.levels.size() == 1 ? "" : "s");

			m_listName->setString(listData.name.c_str());
			m_levelCount->setString(levelCountStr.c_str());
		}
	);

	return true;
}

void RouletteListCell::setCheckCallback(geode::Function<void(int listID, bool checked)> checkCallback) {
	m_checkCallback = std::move(checkCallback);
}