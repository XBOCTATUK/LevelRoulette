#pragma once

#include "LevelData.hpp"

struct RouletteList {
	int id = 0;
	std::string name;
	std::vector<LevelData> levels{};
};

template <>
struct matjson::Serialize<RouletteList> {
	static geode::Result<RouletteList> fromJson(const matjson::Value& value) {
		RouletteList listData;

		GEODE_UNWRAP_INTO(listData.id, value["id"].asInt());
		GEODE_UNWRAP_INTO(listData.name, value["name"].asString());
		GEODE_UNWRAP_INTO(listData.levels, value["levels"].as<std::vector<LevelData>>());

		return geode::Ok(listData);
	}

	static matjson::Value toJson(const RouletteList& listData) {
		return matjson::makeObject({
			{ "id", listData.id },
			{ "name", listData.name },
			{ "levels", listData.levels }
		});
	}
};