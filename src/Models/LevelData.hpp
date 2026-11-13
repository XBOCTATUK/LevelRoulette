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
		LevelData levelData;

		GEODE_UNWRAP_INTO(levelData.levelID, value["levelID"].asInt());
		GEODE_UNWRAP_INTO(levelData.levelName, value["levelName"].asString());
		GEODE_UNWRAP_INTO(levelData.creatorName, value["creator"].asString());
		GEODE_UNWRAP_INTO(levelData.difficulty, stringResultToDifficulty(value["diff"].asString()));

		return geode::Ok(levelData);
	}

	static matjson::Value toJson(const LevelData& levelData) {
		return matjson::makeObject({
			{ "levelID", levelData.levelID },
			{ "levelName", levelData.levelName },
			{ "creator", levelData.creatorName },
			{ "diff", difficultyToString(levelData.difficulty) }
		});
	}
};