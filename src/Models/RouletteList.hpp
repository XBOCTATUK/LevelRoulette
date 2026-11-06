#pragma once

#include "LevelData.hpp"

struct RouletteList {
    int id = 0;
    std::string name;
    std::vector<LevelData> levels;
};

template <>
struct matjson::Serialize<RouletteList> {
    static geode::Result<RouletteList> fromJson(matjson::Value const& value) {
        RouletteList list;

        GEODE_UNWRAP_INTO(list.id, value["id"].asInt());
        GEODE_UNWRAP_INTO(list.name, value["name"].asString());
        GEODE_UNWRAP_INTO(list.levels, value["levels"].as<std::vector<LevelData>>());
        
        return geode::Ok(list);
    }

    static matjson::Value toJson(RouletteList const& list) {
        return matjson::makeObject({
            { "id", list.id },
            { "name", list.name },
            { "levels", list.levels }
        });
    }
};