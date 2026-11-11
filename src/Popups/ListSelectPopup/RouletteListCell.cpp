#include "RouletteListCell.hpp"
#include "../../Models/ListManager/ListManager.hpp"
#include "../LevelListPopup/LevelListPopup.hpp"
#include "../RenamePopup/RenamePopup.hpp"

using namespace geode::prelude;

RouletteListCell* RouletteListCell::create(const RouletteList& listData, float width, geode::Function<void(int listID, bool checked)> callback) {
	auto ret = new RouletteListCell();
	if (ret->init(listData, width, std::move(callback))) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RouletteListCell::init(const RouletteList& listData, float width, geode::Function<void(int listID, bool checked)> callback) {
	if (!CCNode::init()) return false;

	setContentSize({ width, 32.0f });
	m_callback = std::move(callback);

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
		0.6f, [this, listData](CCMenuItemToggler* self) {
			if (m_callback) {
				m_callback(listData.id, !self->isToggled());
			}
		}
	);

	auto renameBtnSpr = CircleButtonSprite::createWithSprite(
		"pencil-icon.png"_spr, 0.8f,
		CircleBaseColor::Green, CircleBaseSize::Tiny
	);

	m_renameBtn = CCMenuItemExt::createSpriteExtra(
		renameBtnSpr, [this, listData](auto) {
			RenamePopup::create(
				listData, [this, listData](std::string newName) {
					ListManager::renameList(listData.id, newName);
				}
			)->show();
		}
	);

	auto listsBtnSpr = CircleButtonSprite::createWithSprite(
		"list-icon.png"_spr, 0.9f,
		CircleBaseColor::Green, CircleBaseSize::Tiny
	);

	m_levelListBtn = CCMenuItemExt::createSpriteExtra(
		listsBtnSpr, [this, listData](auto) {
			LevelListPopup::create(listData)->show();
		}
	);

	auto useSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongBtn_001.png");
	useSpr->setScale(29.0f / useSpr->getContentHeight());
	auto inUseSpr = CCSprite::createWithSpriteFrameName("GJ_selectSongOnBtn_001.png");
	inUseSpr->setScale(29.0f / inUseSpr->getContentHeight());

	m_useToggler = CCMenuItemExt::createToggler(
		inUseSpr, useSpr,
		[this, listData](CCMenuItemToggler* self) {
			if (!self->isToggled()) {
				ListManager::setCurrentList(listData.id);
			}
		}
	);

	m_btnMenu = CCMenu::create(
		m_checkToggler, m_renameBtn, m_levelListBtn, m_useToggler, nullptr
	);
	m_btnMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisScaling(AxisScaling::Fit)
		->setCrossAxisScaling(AxisScaling::Fit)
		->setGap(2.5f)
	);
	m_btnMenu->setAnchorPoint({ 1.0f, 0.5f });
	m_btnMenu->ignoreAnchorPointForPosition(false);
	addChildAtPosition(
		m_btnMenu, Anchor::Right, { -5.0f, 0.0f }
	);

	m_btnMenu->updateLayout();

	return true;
}