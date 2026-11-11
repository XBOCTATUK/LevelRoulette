#pragma once

class InputPopup : public geode::Popup {
public:
    static InputPopup* create(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback);
	void setCharLimits(int min, int max);

private:
	geode::Function<void(std::string)> m_callback = nullptr;
	geode::TextInput* m_input = nullptr;
	CCMenuItemSpriteExtra* m_applyBtn = nullptr;

	int m_minChars = 0;
	int m_maxChars = INT_MAX;

	bool init(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback);
};