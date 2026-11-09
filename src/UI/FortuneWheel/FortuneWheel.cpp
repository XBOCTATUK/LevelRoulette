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
        int segments = 83;
        float rimWidth = 5.0f;
        float innerRad = 2.0f;
        float hubRad = 4.0f;
        float radius = getContentWidth() / 2.0f;
        CCPoint center{};

        int steps = std::max(1, edges / segments);
        float segmentAngle = 2.0f * std::numbers::pi_v<float> / segments;

        m_drawNode = CCDrawNode::create();
        m_drawNode->setAnchorPoint({ 0.5f, 0.5f });
        m_drawNode->setID("wheel-base");

        m_drawNode->drawCircle(
            center, radius, { 1.0f, 1.0f, 1.0f, 1.0f },
            0.0f, { 0.0f, 0.0f, 0.0f, 0.0f }, edges
        );

        for (int i = 0; i < segments; i++) {
            float startRad = segmentAngle * i;
            float endRad = startRad + segmentAngle;

            std::vector<CCPoint> verts;
            verts.push_back(
                center + ccp(std::cos(startRad), std::sin(startRad)) * innerRad
            );

            for (int j = 0; j <= steps; j++) {
                float rad = segmentAngle * (i + static_cast<float>(j) / steps);
                auto point = center + ccp(std::cos(rad), std::sin(rad)) * (radius - rimWidth);
                
                verts.push_back(point);
            }

            verts.push_back(
                center + ccp(std::cos(endRad), std::sin(endRad)) * innerRad
            );

            float hue = static_cast<float>(i) / segments * 6.0f;
            float c = std::fmod(hue, 1.0f);

            ccColor4F color;
            switch (static_cast<int>(hue)) {
                case 0: color = { 1.0f, c, 0.0f, 1.0f };
                break;
                case 1: color = { 1.0f - c, 1.0f, 0.0f, 1.0f };
                break;
                case 2: color = { 0.0f, 1.0f, c, 1.0f };
                break;
                case 3: color = { 0.0f, 1.0f - c, 1.0f, 1.0f };
                break;
                case 4: color = { c, 0.0f, 1.0f, 1.0f };
                break;
                default: color = { 1.0f, 0.0f, 1.0f - c, 1.0f };
            }

            m_drawNode->drawPolygon(
                verts.data(),
                static_cast<unsigned int>(verts.size()),
                color, 0.0f, { 0.0f, 0.0f, 0.0f, 1.0f }
            );
        }

        m_drawNode->drawCircle(
            center, hubRad,
            { 0.0f, 0.0f, 0.0f, 1.0f },
            0.0f, { 1.0f, 1.0f, 1.0f, 1.0f }, 32
        );

        addChildAtPosition(
            m_drawNode, Anchor::Center, {}
        );

        m_arrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        m_arrowSpr->setScaleX(0.8f);
        m_arrowSpr->setScaleY(0.6f);
        m_arrowSpr->setID("wheel-arrow");
        addChildAtPosition(
            m_arrowSpr, Anchor::Right, {}
        );

        scheduleUpdate();

        return true;
    }

    void FortuneWheel::update(float dt) {
        CCNode::update(dt);
        m_drawNode->setRotation(m_drawNode->getRotation() + 32.0f * dt);
    }
}