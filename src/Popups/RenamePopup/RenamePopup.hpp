#pragma once

#include "../../Models/RouletteList.hpp"

class RenamePopup : public geode::Popup {
public:
    static RenamePopup* create(const RouletteList& listData, geode::Function<void(std::string)> callback);

private:
	geode::Function<void(std::string)> m_callback = nullptr;
	geode::TextInput* m_input = nullptr;

	bool init(const RouletteList& listData, geode::Function<void(std::string)> callback);
};