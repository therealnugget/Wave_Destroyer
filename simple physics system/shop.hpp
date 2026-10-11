#pragma once
#include "SDL_ttf.h"
#include "physics.hpp"
#include "Item.hpp"
//instance, singleton
class Shop final {
private:
	static constexpr float shopScale = 3.f;
	static constexpr float playerPromptDistSqr = 69000.f;
	static constexpr int enter_shop_key = SDL_SCANCODE_E;
	static std::string promptMessage;
	static const tuple<float, float, float> shopRenderDrawColor;
	static const std::string shopTitleMessage;
	static const std::string tabMessages[];
	static Node<std::function<void(void)>>* pauseUpdateNode;
	Node<Entity*>* backgroundNode;
	Node<Entity*>* foregroundNode;
	Node<Entity*>* backDropUINode;
	Node<Entity*>* tabUINode;
	Node<Entity*>* cellUINode;
	Node<Entity*>* buttonUINode;
	Entity* foregroundEnt;
	void Update(void);
	inline bool GetShopKey(void) {
		return Main::KeyPressed(enter_shop_key);
	}
	void PauseUpdate(void);
	void DestroyPrompt(void);
	static Text::TextData GetShopData(IntVec2 size, IntVec2 offset, std::string message);
	static Node<Entity *> *GetShopUINode(const char *, CCList, FVector2 offset, IntVec2 size);
	static Text* promptText;
	static Text* shopTitle;
	static Text* tabText;
	static constexpr IntVec2 promptSize = IntVec2(315, 40);
	static constexpr IntVec2 promptOffset = IntVec2(-10, -35);
	static constexpr IntVec2 shopTitleSize = IntVec2(270, 145);
	static constexpr IntVec2 shopTitleOffset = IntVec2(-28, -388);
	static constexpr IntVec2 shopSize = IntVec2(207 * shopScale, 183 * shopScale);
	static constexpr IntVec2 shopTabSize = IntVec2(100, 100);
	static constexpr SDL_Color shopFontColor = SDL_Color(112, 98, 85, 255);
	static constexpr float backDropScale = 3.9f;
	static constexpr IntVec2 backDropSize = IntVec2(224 * backDropScale, 256 * backDropScale);
	static constexpr FVector2 backDropOffset = FVector2(.0f, -70.0f); 
	static constexpr FVector2 shopFrontOffset = FVector2(.0f, 165.f);
	static constexpr FVector2 shopTabOffset = FVector2(-190.f, -275.f);
	static Text::TextData promptData;
	static Text::TextData shopTitleData;
	static Text::TextData tabData;
	~Shop(void);
public:
	Shop(void);
	friend class Environment;
};