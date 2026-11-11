#include <Geode/modify/LevelListLayer.hpp>

using namespace geode::prelude;

class $modify(LevelListLayer) {
    void loadLevelsFinished(CCArray* levels, char const* key, int type) {
        LevelListLayer::loadLevelsFinished(levels, key, type);

        // for (auto [key, obj] : CCDictionaryExt<int, CCObject*>(m_levelList->m_levelsDict)) {
        //     auto level = typeinfo_cast<GJGameLevel*>(obj);
        //     if (level) {
        //         log::info("{}", level->m_levelName);
        //     }
        // }
    }
};