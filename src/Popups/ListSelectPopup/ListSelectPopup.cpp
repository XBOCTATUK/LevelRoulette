#include "ListSelectPopup.hpp"

using namespace geode::prelude;

ListSelectPopup* ListSelectPopup::create() {
	auto ret = new ListSelectPopup();
	if (ret->init()) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool ListSelectPopup::init() {
    if (!Popup::init(280.0f, 280.0f)) return false;



    return true;
}