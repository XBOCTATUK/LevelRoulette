#include "FortuneWheel.hpp"
#include <numbers>

using namespace geode::prelude;

namespace TailyUI {
    FortuneWheel* FortuneWheel::create() {
        auto ret = new FortuneWheel();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool FortuneWheel::init() {
        if (!CCNode::init()) return false;

        setContentSize({ 240.0f, 240.0f });
        setAnchorPoint({ 0.5f, 0.5f });

        int edges = 256;
        int segments = 64;
        float radius = getContentWidth() / 2.0f;
        // const CCPoint center = getContentSize() / 2.0f;
        CCPoint center{};
        float rimWidth = 5.0f;

        m_drawNode = CCDrawNode::create();
        m_drawNode->setAnchorPoint({ 0.5f, 0.5f });
        m_drawNode->setPosition(getContentSize() / 2.0f);

        m_drawNode->drawCircle(
            center, getContentWidth() / 2.0f,
            { 1.0f, 1.0f, 1.0f, 1.0f },
            0.0f, { 1, 1, 1, 1 }, edges
        );

        int steps = std::max(1, edges / segments);
        float segmentAngle = 2.0f * std::numbers::pi_v<float> / segments;

        for (int i = 0; i < segments; i++) {
            std::vector<CCPoint> verts{ center };

            for (int j = 0; j <= steps; j++) {
                float rad = segmentAngle * (i + static_cast<float>(j) / steps);
                auto point = center + ccp( std::cos(rad), std::sin(rad) ) * (radius - rimWidth);
                
                verts.push_back(point);
            }

            float hue = static_cast<float>(i) / segments * 6.0f;
            float c = 1.0f - std::abs(std::fmod(hue, 2.0f) - 1.0f);

            ccColor4F color;
            switch (static_cast<int>(hue)) {
                case 0: color = { 1.0f, c, 0.0f, 1.0f };
                break;
                case 1: color = { c, 1.0f, 0.0f, 1.0f };
                break;
                case 2: color = { 0.0f, 1.0f, c, 1.0f };
                break;
                case 3: color = { 0.0f, c, 1.0f, 1.0f };
                break;
                case 4: color = { c, 0.0f, 1.0f, 1.0f };
                break;
                default: color = { 1.0f, 0.0f, c, 1.0f };
            }

            m_drawNode->drawPolygon(
                verts.data(),
                static_cast<unsigned int>(verts.size()),
                color, 0.25f, { 0.0f, 0.0f, 0.0f, 1.0f }
            );
        }

        addChild(m_drawNode);
        scheduleUpdate();

        return true;
    }

    void FortuneWheel::update(float dt) {
        CCNode::update(dt);
        m_drawNode->setRotation(m_drawNode->getRotation() + 32.0f * dt);
    }
}