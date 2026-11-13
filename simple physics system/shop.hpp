#pragma once
#include "SDL_ttf.h"
#include "physics.hpp"
#include "Item.hpp"
//instance, singleton
class Shop final {
private:
	static constexpr float shopScale = 3.f;
	static constexpr float playerPromptDistSqr = 69000.f;
	static constexpr int shopTabDifference = 129;
	static constexpr int enter_shop_key = SDL_SCANCODE_E;
	static constexpr int num_tabs = 4;
	static const tuple<float, float, float> shopRenderDrawColor;
	static const std::string shopTitleMessage;
	static const std::string tabMessages[];
	int currentTabIndex;
	std::string promptMessage;
	Node<std::function<void(void)>>* pauseUpdateNode;
	Node<Entity*>* backgroundNode;
	Node<Entity*>* foregroundNode;
	Node<Entity*>* backDropUINode;
	Node<Entity*>* tabUINode;
	Node<Entity*>* cellUINode;
	Node<Entity*>* buttonUINode;
	Entity* foregroundEnt;
	Entity* tabUIEnt;
	void Update(void);
	inline bool GetShopKey(void) {
		return Main::KeyPressed(enter_shop_key);
	}
	void PauseUpdate(void);
	void DestroyPrompt(void);
	Text::TextData GetShopData(IntVec2 size, IntVec2 offset, std::string message);
	Node<Entity *> *GetShopUINode(const char *, CCList, FVector2 offset, IntVec2 size);
	Text* promptText;
	Text* shopTitle;
	Text* tabText;
	static constexpr IntVec2 promptSize = IntVec2(315, 40);
	static constexpr IntVec2 promptOffset = IntVec2(-10, -35);
	static constexpr IntVec2 shopTitleSize = IntVec2(270, 145);
	static constexpr IntVec2 shopTitleOffset = IntVec2(-28, -388);
	static constexpr IntVec2 shopTabOffset = IntVec2(-190.f, -271.f);
	static constexpr IntVec2 shopSize = IntVec2(207 * shopScale, 183 * shopScale);
	static constexpr float backDropScale = 3.9f;
	static constexpr IntVec2 shopTabSize = IntVec2(32 * backDropScale, 23 * backDropScale);
	static constexpr SDL_Color shopFontColor = SDL_Color(112, 98, 85, 255);
	static constexpr IntVec2 backDropSize = IntVec2(224 * backDropScale, 256 * backDropScale);
	static constexpr FVector2 backDropOffset = FVector2(.0f, -70.0f); 
	static constexpr FVector2 shopFrontOffset = FVector2(.0f, 165.f);
	Text::TextData promptData;
	Text::TextData shopTitleData;
	Text::TextData tabData;
	~Shop(void);
public:
	Shop(void);
	friend class Environment;
};