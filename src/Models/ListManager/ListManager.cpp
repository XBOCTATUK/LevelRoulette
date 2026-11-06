#include "ListManager.hpp"
#include <Geode/utils/file.hpp>
#include "../../Utils/DifficultyFromLevel.hpp"

using namespace geode::prelude;

geode::Result<matjson::Value> ListManager::getData() {
    auto listsPath = geode::Mod::get()->getPersistentDir() / "lists.json";

    if (!std::filesystem::exists(listsPath)) {
        return Ok(matjson::Value::object());
    }

    auto data = file::readJson(listsPath);
    if (data.isErr()) {
        return Err(data.unwrapErr());
    }

    return Ok(data.unwrap());
}

void ListManager::saveData(const matjson::Value& data, const std::string& fileName) {
    auto listsPath = geode::Mod::get()->getPersistentDir() / fileName;

    auto writeResult = file::writeToJson(listsPath, data);
    if (writeResult.isErr()) {
        log::error("Failed to save '{}': {}", fileName, writeResult.unwrapErr());
        FLAlertLayer::create(
            "Error",
            "Failed to save the list. Please try again later.",
            "Ok"
        )->show();
    }
}

void ListManager::load() {
    if (s_isLoaded) return;
    s_isLoaded = true;
    
    auto dataResult = getData();
    if (dataResult.isErr()) {
        markBroken(dataResult.unwrapErr());
        return;
    }

    auto data = dataResult.unwrap();
    if (!data.isObject()) {
        markBroken("Root is not an object");
        return;
    }
    if (data.size() == 0) return;

    if (!data.contains("fileVersion")) {
        auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        auto time = fmt::format("{:%Y-%m-%d_%H-%M-%S}", now);

        if (!saveBackup(fmt::format("lists-backup-{}.json", time))) {
            markBroken("Backup failed");
            return;
        }

        migrateData(data);
        save();

        return;
    }

    auto lists = data["lists"].as<std::vector<RouletteList>>();
    if (lists.isErr()) {
        markBroken(lists.unwrapErr());
        return;
    }
    s_lists = std::move(lists.unwrap());

    s_currentListID = data["currentListID"].asInt().unwrapOr(0);
    s_lastID = 0;

    for (const auto& list : s_lists) {
        s_lastID = list.id > s_lastID ? list.id : s_lastID;
    }
}

void ListManager::save() {
    if (s_isBroken) {
        log::error("Saving the file was cancelled. File is marked as broken.");
        return;
    }

    saveData(matjson::makeObject({
        { "fileVersion", 2 },
        { "lists", s_lists },
        { "currentListID", s_currentListID }
    }));
}

const std::vector<RouletteList>& ListManager::getLists() {
    load();
    return s_lists;
}

RouletteList* ListManager::getList(int listID) {
    load();

    auto it = std::ranges::find(s_lists, listID, &RouletteList::id);
    if (it == s_lists.end()) return nullptr;

    return &*it;
}

RouletteList* ListManager::getCurrentList() {
    load();
    return getList(s_currentListID);
}

void ListManager::setCurrentList(int listID) {
    load();
    s_currentListID = listID;
    save();
}

int ListManager::createList(const std::string& listName) {
    load();

    auto& list = s_lists.emplace_back(
        RouletteList{ ++s_lastID, listName, {} }
    );
    save();

    return list.id;
}

void ListManager::renameList(int listID, const std::string& newName) {
    if (auto it = getList(listID)) {
        it->name = newName;
        save();
    }
}

void ListManager::removeList(int listID) {
    load();

    std::erase_if(
        s_lists, [listID](const auto& list) {
            return list.id == listID;
        }
    );

    if (s_currentListID == listID) {
        s_currentListID = 0;
    }

    save();
}

void ListManager::addLevelToList(int listID, GJGameLevel* level) {
    if (!level) return;
    load();

    auto list = getList(listID);
    if (!list) {
        log::error("Invalid list ID ({})", listID);
        return;
    }

    auto it = std::find_if(
        list->levels.begin(), list->levels.end(),
        [levelID = level->m_levelID.value()](auto& listLevel) {
            return listLevel.levelID == levelID;
        }
    );
    if (it != list->levels.end()) return;

    auto listLevel = LevelData{
        level->m_levelID.value(), level->m_levelName,
        level->m_creatorName, Utils::difficultyFromLevel(level)
    };
    list->levels.push_back(listLevel);

    save();
}

void ListManager::removeLevelFromList(int listID, int levelID) {
    load();

    auto list = getList(listID);
    if (!list) {
        log::error("Invalid list ID ({})", listID);
        return;
    }

    std::erase_if(
        list->levels, [levelID](const auto& level) {
            return level.levelID == levelID;
        }
    );

    save();
}

bool ListManager::saveBackup(const std::string& backupName) {
    auto listsPath = geode::Mod::get()->getPersistentDir() / "lists.json";
    auto backupPath = geode::Mod::get()->getPersistentDir() / backupName;

    std::error_code code;
    std::filesystem::copy_file(listsPath, backupPath, code);
    if (code) {
        log::error("Failed to create backup: {}", code.message());
    }

    return !code;
}

void ListManager::migrateData(const matjson::Value &data) {
    for (auto& [listKey, list] : data) {
        if (!list.isObject()) continue;

        std::vector<LevelData> levels{};
        for (auto& [levelKey, level] : list) {
            if (!level.isObject()) continue;
            
            auto levelID = level["levelID"].asInt();
            auto levelName = level["name"].asString();
            auto creatorName = level["creator"].asString();
            auto difficulty = level["diff"].asString();
            if (levelID.isErr() || levelName.isErr() || creatorName.isErr() || difficulty.isErr()) continue;

            LevelData levelData{
                static_cast<int>(levelID.unwrap()),
                levelName.unwrap(),
                creatorName.unwrap(),
                stringToDifficulty(difficulty.unwrap())
            };

            levels.push_back(levelData);
        }

        RouletteList rouletteList{ ++s_lastID, listKey, levels };
        s_lists.push_back(rouletteList);
    }
}

void ListManager::markBroken(const std::string& reason) {
    log::error("Lists file is broken: {}", reason);
    s_isBroken = true;

    FLAlertLayer::create(
        "Error",
        "Failed to load your lists. Your save file was left untouched.",
        "Ok"
    )->show();
}