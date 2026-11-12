#pragma once

#include "../RouletteList.hpp"

class ListManager {
public:
	static const std::map<int, RouletteList>& getLists();
	static RouletteList* getList(int listID);

	static RouletteList* getCurrentList();
	static void setCurrentList(int listID);
	static bool isCurrentList(int listID);

	static int createList(const std::string& listName);
	static void renameList(int listID, const std::string& newName);
	static void removeList(int listID);

	static void addLevelToList(int listID, GJGameLevel* level);
	static void removeLevelFromList(int listID, int levelID);

	static void importLists();
	static void exportLists(const std::unordered_set<int>& listIDs);

private:
	static geode::Result<matjson::Value> getData();
	static bool saveData(const matjson::Value& data, const std::string& fileName = "lists.json");
	static bool load();
	static bool save();
	static bool saveBackup(const std::string& backupName);
	static void migrateData(const matjson::Value& data);
	static void markBroken(const std::string& reason);

	static inline bool s_isLoaded = false;
	static inline std::map<int, RouletteList> s_lists;
	static inline int s_currentListID = 0;
	static inline int s_lastID = 0;

	static inline bool s_isBroken = false;
};