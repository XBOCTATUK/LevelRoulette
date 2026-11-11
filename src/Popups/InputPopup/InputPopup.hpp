#pragma once

class InputPopup : public geode::Popup {
public:
    static InputPopup* create(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback);

private:
	geode::Function<void(std::string)> m_callback = nullptr;
	geode::TextInput* m_input = nullptr;

	bool init(const std::string& title, const std::string& text, const std::string& btnText, geode::Function<void(std::string)> callback);
};