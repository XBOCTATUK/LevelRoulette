#include "ListManager.hpp"
#include <Geode/utils/file.hpp>
#include "../../Utils/DifficultyFromLevel.hpp"
#include "../../Events/CurrentListEvent.hpp"
#include "../../Events/UpdateCurrentListEvent.hpp"
#include "../../Events/UpdateListScrollEvent.hpp"
#include "../../Events/UpdateLevelScrollEvent.hpp"
#include "../../Events/UpdateListCellEvent.hpp"

using namespace geode::prelude;

Result<matjson::Value> ListManager::getData() {
	auto listsPath = Mod::get()->getPersistentDir() / "lists.json";

	if (!std::filesystem::exists(listsPath)) {
		return Ok(matjson::Value::object());
	}

	auto dataResult = file::readJson(listsPath);
	if (dataResult.isErr()) return Err(dataResult.unwrapErr());

	return Ok(dataResult.unwrap());
}

bool ListManager::saveData(const matjson::Value& data, const std::string& fileName) {
	auto filePath = Mod::get()->getPersistentDir() / fileName;

	auto writeResult = file::writeToJson(filePath, data);
	if (writeResult.isErr()) {
		log::error("Failed to save {}: {}", fileName, writeResult.unwrapErr());
		FLAlertLayer::create(
			"Error",
			"Couldn't save your lists. Recent changes may be lost after restarting the game.",
			"OK"
		)->show();

		return false;
	}

	return true;
}

bool ListManager::load() {
	if (s_isLoaded) return !s_isBroken;
	s_isLoaded = true;

	auto dataResult = getData();
	if (dataResult.isErr()) {
		markBroken(dataResult.unwrapErr());
		return false;
	}

	auto data = dataResult.unwrap();
	if (!data.isObject()) {
		markBroken("the root value is not a JSON object");
		return false;
	}
	if (data.size() == 0) return true;

	if (!data.contains("fileVersion") || !data["fileVersion"].isNumber()) {
		auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
		auto timestamp = fmt::format("{:%Y-%m-%d_%H-%M-%S}", now);

		if (!saveBackup(fmt::format("lists-backup-{}.json", timestamp))) {
			markBroken("couldn't create a backup before migrating to the new format");
			return false;
		}

		migrateData(data);
		if (!save()) return false;

		return true;
	}

	auto listsResult = data["lists"].as<std::vector<RouletteList>>();
	if (listsResult.isErr()) {
		markBroken(listsResult.unwrapErr());
		return false;
	}
	
	s_lists.clear();
	for (const auto& listData : listsResult.unwrap()) {
		s_lists[listData.id] = listData;
		s_lastID = listData.id > s_lastID ? listData.id : s_lastID;
	}

	s_currentListID = data["currentListID"].asInt().unwrapOr(0);

	return true;
}

bool ListManager::save() {
	if (s_isBroken) {
		log::error("Lists weren't saved because the lists file failed to load");
		return false;
	}

	std::vector<RouletteList> lists{};
	lists.reserve(s_lists.size());

	for (const auto& [listID, listData] : s_lists) {
		lists.push_back(listData);
	}

	return saveData(matjson::makeObject({
		{ "fileVersion", 2 },
		{ "currentListID", s_currentListID },
		{ "lists", lists }
	}));
}

const std::map<int, RouletteList>& ListManager::getLists() {
	load();
	return s_lists;
}

RouletteList* ListManager::getList(int listID) {
	load();

	auto listIt = s_lists.find(listID);
	if (listIt == s_lists.end()) return nullptr;

	return &listIt->second;
}

RouletteList* ListManager::getCurrentList() {
	load();
	return getList(s_currentListID);
}

void ListManager::setCurrentList(int listID) {
	load();
	s_currentListID = listID;
	
	CurrentListEvent().send(listID);
	save();
}

bool ListManager::isCurrentList(int listID) {
	return s_currentListID == listID;
}

int ListManager::createList(const std::string& listName) {
	load();

	int listID = ++s_lastID;

	s_lists.emplace(
		listID, RouletteList{listID, listName, {}}
	);

	UpdateListScrollEvent().send();
	save();

	return listID;
}

void ListManager::renameList(int listID, const std::string& newListName) {
	if (auto listData = getList(listID)) {
		listData->name = newListName;

		save();
		UpdateListCellEvent(listID).send();
		if (isCurrentList(listID)) {
			UpdateCurrentListEvent().send();
		}
	}
}

void ListManager::removeList(int listID) {
	removeLists({ listID });
}

void ListManager::removeLists(std::unordered_set<int> listIDs) {
	if (listIDs.empty()) return;
	load();

	auto oldListCount = s_lists.size();
	for (int listID : listIDs) {
		if (s_lists.erase(listID) == 0) continue;

		if (s_currentListID == listID) {
			s_currentListID = 0;
			CurrentListEvent().send(0);
		}
	}

	if (s_lists.size() == oldListCount) return;
	
	UpdateListScrollEvent().send();
	save();
}

void ListManager::addLevelToList(int listID, GJGameLevel* level) {
	addLevelsToList(listID, { level });
}

void ListManager::addLevelToList(int listID, const LevelData& levelData) {
	addLevelsToList(listID, { levelData });
}

void ListManager::addLevelsToList(int listID, const std::vector<GJGameLevel*>& levels) {
	if (levels.empty()) return;

	auto listData = getList(listID);
	if (!listData) {
		log::error("List with ID {} not found", listID);
		return;
	}

	auto oldLevelCount = listData->levels.size();
	for (const auto& level : levels) {
		if (!level || std::ranges::contains(listData->levels, level->m_levelID.value(), &LevelData::levelID)) continue;
		
		auto levelData = LevelData{
		level->m_levelID.value(), level->m_levelName,
		level->m_creatorName, Utils::difficultyFromLevel(level)
		};
		listData->levels.push_back(levelData);
	}

	if (listData->levels.size() == oldLevelCount) return;

	notifyLevelsChanged(listID);
	save();
}

void ListManager::addLevelsToList(int listID, const std::vector<LevelData>& levels) {
	if (levels.empty()) return;

	auto listData = getList(listID);
	if (!listData) {
		log::error("List with ID {} not found", listID);
		return;
	}

	auto oldLevelCount = listData->levels.size();
	for (const auto& levelData : levels) {
		if (std::ranges::contains(listData->levels, levelData.levelID, &LevelData::levelID)) continue;
		listData->levels.push_back(levelData);
	}

	if (listData->levels.size() == oldLevelCount) return;

	notifyLevelsChanged(listID);
	save();
}

void ListManager::removeLevelFromList(int listID, int levelID) {
	removeLevelsFromList(listID, { levelID });
}

void ListManager::removeLevelsFromList(int listID, std::unordered_set<int> levelIDs) {
	if (levelIDs.empty()) return;

	auto listData = getList(listID);
	if (!listData) {
		log::error("List with ID {} not found", listID);
		return;
	}

	auto removedCount = std::erase_if(
		listData->levels, [levelIDs](const auto& levelData) {
			return levelIDs.contains(levelData.levelID);
		}
	);

	if (removedCount == 0) return;

	notifyLevelsChanged(listID);
	save();
}

void ListManager::importLists() {
	async::spawn(
		file::pick(file::PickMode::OpenFile, file::FilePickOptions {
			.filters = { file::FilePickOptions::Filter {
				.description = "JSON files",
				.files = { "*.json" },
			}}
		}),
		[](Result<std::optional<std::filesystem::path>> pickResult) {
			if (pickResult.isErr()) {
				log::error("Failed to open the file picker: {}", pickResult.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't open the file picker. Please try again.",
					"OK"
				)->show();

				return;
			}

			if (!pickResult.unwrap().has_value()) return;

			auto dataResult = file::readJson(pickResult.unwrap().value());
			if (dataResult.isErr()) {
				log::error("Failed to read the import file: {}", dataResult.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't read the selected file. Make sure it's a valid JSON file.",
					"OK"
				)->show();

				return;
			}

			auto data = dataResult.unwrap();
			if (!data.contains("lists")) {
				log::error("Import file has no \"lists\" field");

				FLAlertLayer::create(
					"Error",
					"This file doesn't contain any Level Roulette lists.",
					"OK"
				)->show();

				return;
			}

			auto listsResult = data["lists"].as<std::vector<RouletteList>>();
			if (listsResult.isErr()) {
				log::error("Import file contains invalid list data: {}", listsResult.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"This file contains invalid list data and can't be imported.",
					"OK"
				)->show();

				return;
			}

			auto lists = listsResult.unwrap();
			if (lists.empty()) {
				FLAlertLayer::create(
					"Nothing to Import",
					"This file doesn't contain any lists.",
					"OK"
				)->show();

				return;
			}

			load();
			if (s_isBroken) {
				FLAlertLayer::create(
					"Error",
					"Your lists failed to load, so importing is disabled to prevent data loss.",
					"OK"
				)->show();

				return;
			}

			for (auto& listData : lists) {
				int listID = ++s_lastID;
				listData.id = listID;

				s_lists[listID] = listData;
			}
			
			if (save()) {
				Notification::create(
					fmt::format("Imported {} list{}", lists.size(), lists.size() == 1 ? "" : "s"),
					NotificationIcon::Success
				)->show();
			}

			UpdateListScrollEvent().send();
		}
	);
}

void ListManager::exportLists(const std::unordered_set<int>& listIDs) {
	load();

	std::vector<RouletteList> listsToExport{};
	listsToExport.reserve(listIDs.size());

	for (int listID : listIDs) {
		if (auto listData = getList(listID)) {
			listsToExport.push_back(*listData);
		}
	}

	if (listsToExport.empty()) {
		FLAlertLayer::create(
			"Nothing to Export",
			"There are no lists to export.",
			"OK"
		)->show();

		return;
	}

	std::string fileName = listsToExport.front().name;
	for (char& c : fileName) {
		if (std::string_view("<>:\"/\\|?*").find(c) != std::string_view::npos) {
			c = '_';
		}
	}

	auto defaultPath = listsToExport.size() == 1
	? fileName + ".json"
	: "level-roulette-lists.json";

	async::spawn(
		file::pick(file::PickMode::SaveFile, file::FilePickOptions {
			.defaultPath = defaultPath,
			.filters = { file::FilePickOptions::Filter {
				.description = "JSON files",
				.files = { "*.json" },
			}}
		}),
		[listsToExport = std::move(listsToExport)](Result<std::optional<std::filesystem::path>> pickResult) {
			if (pickResult.isErr()) {
				log::error("Failed to open the file picker: {}", pickResult.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't open the file picker. Please try again.",
					"OK"
				)->show();

				return;
			}

			if (!pickResult.unwrap().has_value()) return;

			auto path = pickResult.unwrap().value();
			if (path.extension() != ".json") path += ".json";

			auto exportData = matjson::makeObject({
				{ "fileVersion", 2 },
				{ "lists", listsToExport }
			});

			auto writeResult = file::writeToJson(path, exportData);
			if (writeResult.isErr()) {
				log::error("Failed to export to {}: {}", path, writeResult.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't export your lists. Please try again.",
					"OK"
				)->show();

				return;
			}

			Notification::create(
				fmt::format("Exported {} list{}", listsToExport.size(), listsToExport.size() == 1 ? "" : "s"),
				NotificationIcon::Success
			)->show();
		}
	);
}

bool ListManager::saveBackup(const std::string& backupName) {
	auto listsPath = Mod::get()->getPersistentDir() / "lists.json";
	auto backupPath = Mod::get()->getPersistentDir() / backupName;

	std::error_code errorCode;
	std::filesystem::copy_file(listsPath, backupPath, errorCode);
	if (errorCode) {
		log::error("Failed to create backup: {}", errorCode.message());
	}

	return !errorCode;
}

void ListManager::migrateData(const matjson::Value& data) {
	s_lists.clear();

	for (const auto& [listKey, listJson] : data) {
		if (!listJson.isObject()) continue;

		std::vector<LevelData> levels{};
		for (const auto& [levelKey, levelJson] : listJson) {
			if (!levelJson.isObject()) continue;

			auto levelIDResult = levelJson["levelID"].asInt();
			auto levelNameResult = levelJson["name"].asString();
			auto creatorNameResult = levelJson["creator"].asString();
			auto difficultyResult = levelJson["diff"].asString();
			if (levelIDResult.isErr() || levelNameResult.isErr() || creatorNameResult.isErr() || difficultyResult.isErr()) continue;

			LevelData levelData{
				static_cast<int>(levelIDResult.unwrap()),
				levelNameResult.unwrap(),
				creatorNameResult.unwrap(),
				stringToDifficulty(difficultyResult.unwrap())
			};

			levels.push_back(levelData);
		}

		RouletteList listData{ ++s_lastID, listKey, levels };
		s_lists[listData.id] = listData;
	}
}

void ListManager::markBroken(const std::string& reason) {
	log::error("Failed to load lists: {}", reason);
	s_isBroken = true;

	FLAlertLayer::create(
		"Error",
		"Couldn't load your lists. Your save file hasn't been changed.",
		"OK"
	)->show();
}

void ListManager::notifyLevelsChanged(int listID) {
	UpdateListCellEvent(listID).send();
	UpdateLevelScrollEvent().send();
	if (isCurrentList(listID)) {
		UpdateCurrentListEvent().send();
	}
}