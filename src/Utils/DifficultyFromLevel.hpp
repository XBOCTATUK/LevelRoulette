#pragma once

#include "../Models/Difficulty.hpp"

namespace Utils {
	inline Difficulty difficultyFromLevel(GJGameLevel* level) {
		if (level->m_autoLevel) return Difficulty::Auto;

		if (level->m_demon.value() == 1) {
			switch (level->m_demonDifficulty) {
				case 3: return Difficulty::EasyDemon;
				case 4: return Difficulty::MediumDemon;
				case 5: return Difficulty::InsaneDemon;
				case 6: return Difficulty::ExtremeDemon;
				default: return Difficulty::HardDemon;
			}
		}

		if (level->m_ratings < 5) return Difficulty::NA;

		int difficulty = static_cast<int>(std::roundf(
			static_cast<float>(level->m_ratingsSum) / level->m_ratings)
		);

		return static_cast<Difficulty>(difficulty);
	}
}