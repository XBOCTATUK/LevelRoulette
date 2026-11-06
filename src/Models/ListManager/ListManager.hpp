#pragma once

#include "../RouletteList.hpp"

class ListManager {
public:
    static const std::vector<RouletteList>& getLists();
    static RouletteList* getList(int listID);

    static RouletteList* getCurrentList();
    static void setCurrentList(int listID);

    static int createList(const std::string& listName);
    static void renameList(int listID, const std::string& newName);
    static void removeList(int listID);

    static void addLevelToList(int listID, GJGameLevel* level);
    static void removeLevelFromList(int listID, int levelID);

private:
    static geode::Result<matjson::Value> getData();
    static void saveData(const matjson::Value& data, const std::string& fileName = "lists.json");
    static void load();
    static void save();
    static bool saveBackup(const std::string& backupName);
    static void migrateData(const matjson::Value& data);
    static void markBroken(const std::string& reason);

    static inline bool s_isLoaded = false;
    static inline std::vector<RouletteList> s_lists;
    static inline int s_currentListID = 0;
    static inline int s_lastID = 0;

    static inline bool s_isBroken = false;
};