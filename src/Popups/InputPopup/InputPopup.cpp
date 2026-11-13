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

	auto applyBtnSpr = ButtonSprite::create(btnText.c_str());
	m_applyBtn = CCMenuItemExt::createSpriteExtra(
		applyBtnSpr, [this](auto) {
			auto inputText = m_input->getString();
			if (inputText.size() < m_minChars || inputText.size() > m_maxChars) {
				FLAlertLayer::create(
					"Invalid Length",
					fmt::format("Enter between {} and {} characters.", m_minChars, m_maxChars),
					"OK"
				)->show();
				
				return;
			}

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

void InputPopup::setCharLimits(int minChars, int maxChars) {
	if (minChars > maxChars || minChars < 0 || maxChars < 0) return;

	m_minChars = minChars;
	m_maxChars = maxChars;
}