#pragma once

namespace TailyUI {
	class FortuneWheel : public cocos2d::CCNode {
	public:
		static FortuneWheel* create();

	protected:
		cocos2d::CCDrawNode* m_drawNode = nullptr;
		cocos2d::CCSprite* m_arrowSpr = nullptr;

		bool init() override;
		void update(float dt) override;
	};
}