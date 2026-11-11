#include "RenamePopup.hpp"

using namespace geode::prelude;

RenamePopup* RenamePopup::create(const RouletteList& listData, geode::Function<void(std::string)> callback) {
	auto ret = new RenamePopup();
	if (ret && ret->init(listData, std::move(callback))) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool RenamePopup::init(const RouletteList& listData, geode::Function<void(std::string)> callback) {
	if (!Popup::init(300.0f, 120.0f)) return false;

	setTitle("Rename list");
	m_callback = std::move(callback);

	m_input = TextInput::create(160.0f, "Enter new name...");
	m_input->setString(listData.name);
	m_buttonMenu->addChildAtPosition(
        m_input, Anchor::Center, { 0.0f, 5.0f }
    );

	auto applySpr = ButtonSprite::create("Set");
	auto applyBtn = CCMenuItemExt::createSpriteExtra(
		applySpr, [this](auto) {
			if (m_input->getString().empty()) return;

			if (m_callback) {
				m_callback(m_input->getString());
			}

			onClose(nullptr);
		}
	);
	m_buttonMenu->addChildAtPosition(
        applyBtn, Anchor::Bottom, { 0.0f, 25.0f }
    );

	return true;
}