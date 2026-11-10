#pragma once

#include "Difficulty.hpp"

struct LevelData {
	int levelID = 0;
	std::string levelName;
	std::string creatorName;
	Difficulty difficulty = Difficulty::NA;
};

template <>
struct matjson::Serialize<LevelData> {
	static geode::Result<LevelData> fromJson(const matjson::Value& value) {
		LevelData level;

		GEODE_UNWRAP_INTO(level.levelID, value["levelID"].asInt());
		GEODE_UNWRAP_INTO(level.levelName, value["levelName"].asString());
		GEODE_UNWRAP_INTO(level.creatorName, value["creator"].asString());
		GEODE_UNWRAP_INTO(level.difficulty, stringResultToDifficulty(value["diff"].asString()));

		return geode::Ok(level);
	}

	static matjson::Value toJson(const LevelData& level) {
		return matjson::makeObject({
			{ "levelID", level.levelID },
			{ "levelName", level.levelName },
			{ "creator", level.creatorName },
			{ "diff", difficultyToString(level.difficulty) }
		});
	}
};