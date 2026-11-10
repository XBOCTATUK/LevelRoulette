#pragma once

enum class Difficulty {
	Auto = -1, NA, Easy, Normal, Hard, Harder, Insane,
	HardDemon, EasyDemon, MediumDemon, InsaneDemon, ExtremeDemon
};

inline std::string difficultyToString(Difficulty d) {
	switch (d) {
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

inline Difficulty stringToDifficulty(const std::string& s) {
	if (s == "N/A") return Difficulty::NA;
	if (s == "Auto") return Difficulty::Auto;
	if (s == "Easy") return Difficulty::Easy;
	if (s == "Normal") return Difficulty::Normal;
	if (s == "Hard") return Difficulty::Hard;
	if (s == "Harder") return Difficulty::Harder;
	if (s == "Insane") return Difficulty::Insane;
	if (s == "EasyDemon") return Difficulty::EasyDemon;
	if (s == "MediumDemon") return Difficulty::MediumDemon;
	if (s == "HardDemon") return Difficulty::HardDemon;
	if (s == "InsaneDemon")  return Difficulty::InsaneDemon;
	if (s == "ExtremeDemon") return Difficulty::ExtremeDemon;

	return Difficulty::NA;
}

inline geode::Result<Difficulty> stringResultToDifficulty(geode::Result<std::string> r) {
	if (r.isOk()) return geode::Ok(stringToDifficulty(r.unwrap()));
	return geode::Ok(Difficulty::NA);
}