#include "ListManager.hpp"
#include <Geode/utils/file.hpp>
#include "../../Utils/DifficultyFromLevel.hpp"

using namespace geode::prelude;

Result<matjson::Value> ListManager::getData() {
	auto listsPath = Mod::get()->getPersistentDir() / "lists.json";

	if (!std::filesystem::exists(listsPath)) {
		return Ok(matjson::Value::object());
	}

	auto data = file::readJson(listsPath);
	if (data.isErr()) return Err(data.unwrapErr());

	return Ok(data.unwrap());
}

void ListManager::saveData(const matjson::Value& data, const std::string& fileName) {
	auto listsPath = Mod::get()->getPersistentDir() / fileName;

	auto writeResult = file::writeToJson(listsPath, data);
	if (writeResult.isErr()) {
		log::error("Failed to save {}: {}", fileName, writeResult.unwrapErr());
		FLAlertLayer::create(
			"Error",
			"Couldn't save your lists. Recent changes may be lost after restarting the game.",
			"OK"
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
		markBroken("the root value is not a JSON object");
		return;
	}
	if (data.size() == 0) return;

	if (!data.contains("fileVersion")) {
		auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
		auto time = fmt::format("{:%Y-%m-%d_%H-%M-%S}", now);

		if (!saveBackup(fmt::format("lists-backup-{}.json", time))) {
			markBroken("couldn't create a backup before migrating to the new format");
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
	
	s_lists.clear();
	for (const auto& list : lists.unwrap()) {
		s_lists[list.id] = list;
		s_lastID = list.id > s_lastID ? list.id : s_lastID;
	}

	s_currentListID = data["currentListID"].asInt().unwrapOr(0);
}

void ListManager::save() {
	if (s_isBroken) {
		log::error("Lists weren't saved because the lists file failed to load");
		return;
	}

	std::vector<RouletteList> lists{};
	lists.reserve(s_lists.size());

	for (const auto& [key, list] : s_lists) {
		lists.push_back(list);
	}

	saveData(matjson::makeObject({
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

	auto it = s_lists.find(listID);
	if (it == s_lists.end()) return nullptr;

	return &it->second;
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

	int id = ++s_lastID;

	s_lists.emplace(
		id, RouletteList{id, listName, {}}
	);
	save();

	return id;
}

void ListManager::renameList(int listID, const std::string& newName) {
	if (auto it = getList(listID)) {
		it->name = newName;
		save();
	}
}

void ListManager::removeList(int listID) {
	load();

	if (s_lists.erase(listID) == 0) return;

	if (s_currentListID == listID) {
		s_currentListID = 0;
	}

	save();
}

void ListManager::addLevelToList(int listID, GJGameLevel* level) {
	if (!level) return;

	auto list = getList(listID);
	if (!list) {
		log::error("List with ID {} not found", listID);
		return;
	}

	if (std::ranges::contains(list->levels, level->m_levelID.value(), &LevelData::levelID)) return;

	auto listLevel = LevelData{
		level->m_levelID.value(), level->m_levelName,
		level->m_creatorName, Utils::difficultyFromLevel(level)
	};
	list->levels.push_back(listLevel);

	save();
}

void ListManager::removeLevelFromList(int listID, int levelID) {
	auto list = getList(listID);
	if (!list) {
		log::error("List with ID {} not found", listID);
		return;
	}

	std::erase_if(
		list->levels, [levelID](const auto& level) {
			return level.levelID == levelID;
		}
	);

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
		[](Result<std::optional<std::filesystem::path>> result) {
			if (result.isErr()) {
				log::error("Failed to open the file picker: {}", result.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't open the file picker. Please try again.",
					"OK"
				)->show();

				return;
			}

			if (!result.unwrap().has_value()) return;

			auto dataResult = file::readJson(result.unwrap().value());
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

			for (auto& list : lists) {
				int id = ++s_lastID;
				list.id = id;

				s_lists[id] = list;
			}
			save();

			FLAlertLayer::create(
				"Import Complete",
				fmt::format("Imported {} list{}.", lists.size(), lists.size() == 1 ? "" : "s"),
				"OK"
			)->show();
		}
	);
}

void ListManager::exportLists(const std::unordered_set<int> &listIDs) {
	load();

	std::vector<RouletteList> listsToExport;
	listsToExport.reserve(listIDs.size());

	for (int id : listIDs) {
		if (auto list = getList(id)) {
			listsToExport.push_back(*list);
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
		[listsToExport = std::move(listsToExport)](Result<std::optional<std::filesystem::path>> result) {
			if (result.isErr()) {
				log::error("Failed to open the file picker: {}", result.unwrapErr());

				FLAlertLayer::create(
					"Error",
					"Couldn't open the file picker. Please try again.",
					"OK"
				)->show();

				return;
			}

			if (!result.unwrap().has_value()) return;

			auto path = result.unwrap().value();
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

			FLAlertLayer::create(
				"Export Complete",
				fmt::format("Exported {} list{}.", listsToExport.size(), listsToExport.size() == 1 ? "" : "s"),
				"OK"
			)->show();
		}
	);
}

bool ListManager::saveBackup(const std::string& backupName) {
	auto listsPath = Mod::get()->getPersistentDir() / "lists.json";
	auto backupPath = Mod::get()->getPersistentDir() / backupName;

	std::error_code code;
	std::filesystem::copy_file(listsPath, backupPath, code);
	if (code) {
		log::error("Failed to create backup: {}", code.message());
	}

	return !code;
}

void ListManager::migrateData(const matjson::Value &data) {
	s_lists.clear();

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
		s_lists[rouletteList.id] = rouletteList;
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