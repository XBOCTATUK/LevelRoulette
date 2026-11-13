#pragma once

enum class Difficulty {
	Auto = -1, NA, Easy, Normal, Hard, Harder, Insane,
	HardDemon, EasyDemon, MediumDemon, InsaneDemon, ExtremeDemon
};

inline std::string difficultyToString(Difficulty difficulty) {
	switch (difficulty) {
		case Difficulty::NA: return "N/A";
		case Difficulty::Auto: return "Auto";
		case Difficulty::Easy: return "Easy";
		case Difficulty::Normal: return "Normal";
		case Difficulty::Hard: return "Hard";
		case Difficulty::Harder: return "Harder";
		case Difficulty::Insane: return "Insane";
		case Difficulty::EasyDemon: return "EasyDemon";
		case Difficulty::MediumDemon: return "MediumDemon";
		case Difficulty::HardDemon: return "HardDemon";
		case Difficulty::InsaneDemon: return "InsaneDemon";
		case Difficulty::ExtremeDemon: return "ExtremeDemon";
		default: return "N/A";
	}
}

inline Difficulty stringToDifficulty(const std::string& difficultyName) {
	if (difficultyName == "N/A") return Difficulty::NA;
	if (difficultyName == "Auto") return Difficulty::Auto;
	if (difficultyName == "Easy") return Difficulty::Easy;
	if (difficultyName == "Normal") return Difficulty::Normal;
	if (difficultyName == "Hard") return Difficulty::Hard;
	if (difficultyName == "Harder") return Difficulty::Harder;
	if (difficultyName == "Insane") return Difficulty::Insane;
	if (difficultyName == "EasyDemon") return Difficulty::EasyDemon;
	if (difficultyName == "MediumDemon") return Difficulty::MediumDemon;
	if (difficultyName == "HardDemon") return Difficulty::HardDemon;
	if (difficultyName == "InsaneDemon")  return Difficulty::InsaneDemon;
	if (difficultyName == "ExtremeDemon") return Difficulty::ExtremeDemon;

	return Difficulty::NA;
}

inline geode::Result<Difficulty> stringResultToDifficulty(geode::Result<std::string> difficultyResult) {
	if (difficultyResult.isOk()) return geode::Ok(stringToDifficulty(difficultyResult.unwrap()));
	return geode::Ok(Difficulty::NA);
}