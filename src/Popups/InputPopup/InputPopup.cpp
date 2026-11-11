#include "InputPopup.hpp"

using namespace geode::prelude;

InputPopup* InputPopup::create(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback) {
	auto ret = new InputPopup();
	if (ret && ret->init(title, text, btnText, std::move(callback))) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool InputPopup::init(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback) {
	if (!Popup::init(300.0f, 120.0f)) return false;

	setTitle(title);
	m_callback = std::move(callback);

	m_input = TextInput::create(240.0f, "");
	m_input->setString(text);
	m_input->setID("text-input");
	m_buttonMenu->addChildAtPosition(
        m_input, Anchor::Center, { 0.0f, 5.0f }
    );

	auto applySpr = ButtonSprite::create(btnText.c_str());
	m_applyBtn = CCMenuItemExt::createSpriteExtra(
		applySpr, [this](auto) {
			if (m_callback) {
				m_callback(m_input->getString());
			}

			onClose(nullptr);
		}
	);
	m_applyBtn->setID("apply-button");
	m_buttonMenu->addChildAtPosition(
        m_applyBtn, Anchor::Bottom, { 0.0f, 25.0f }
    );

	return true;
}