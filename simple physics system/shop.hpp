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
	static char* promptMessageCStr;
	static const tuple<float, float, float> shopRenderDrawColor;
	static Node<std::function<void(void)>>* pauseUpdateNode;
	Node<Entity*>* backgroundNode;
	Node<Entity*>* foregroundNode;
	Node<Entity*>* backDropUINode;
	Entity* foregroundEnt;
	void Update(void);
	inline bool GetShopKey(void) {
		return Main::KeyPressed(enter_shop_key);
	}
	void PauseUpdate(void);
	void DestroyPrompt(void);
	static Text* promptText;
	static constexpr IntVec2 promptSizeVec = IntVec2(315, 40);
	static constexpr IntVec2 promptOffset = IntVec2(-10, -35);
	static constexpr IntVec2 shopSize = IntVec2(207 * shopScale, 183 * shopScale);
	static constexpr float backDropScale = 3.9f;
	static constexpr IntVec2 backDropSize = IntVec2(224 * backDropScale, 256 * backDropScale);
	static constexpr FVector2 backDropOffset = FVector2(.0f, -70.0f); 
	static constexpr FVector2 shopFrontOffset = FVector2(.0f, 165.f);
	static Text::TextData promptData;
	~Shop(void);
public:
	Shop(void);
	friend class Environment;
};