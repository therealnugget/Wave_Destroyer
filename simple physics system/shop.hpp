#pragma once
#include "SDL_ttf.h"
#include "physics.hpp"
#include "Item.hpp"
//instance, singleton
class Shop final {
private:
	static constexpr float shopScale = 3.f;
	static constexpr float player_prompt_dst_sqr = 69000.f;
	static constexpr float confirm_button_jiggle = 17.f;
	static constexpr float confirm_button_sin_scale = 23.f;
	static constexpr float confirm_button_size_dilation_add = .3f;
	static constexpr float purchase_attempt_max_time = .4f;
	static constexpr int shopTabDifference = 129;
	static constexpr int buttonDifference = 270;
	static constexpr int enter_shop_key = SDL_SCANCODE_E;
	static constexpr int num_tabs = 4;
	static constexpr int num_button_slots = 2;
	static constexpr int num_item_col = 5;
	static constexpr int num_item_row = 3;
	static constexpr int cancel_button_index = 0;
	static constexpr int confirm_button_index = 1;
	static constexpr const char* item_icons_path = "UI/item icons";
	static constexpr const char* shop_ui_path = "UI/shop";
	//the number of shop items per item page
	static constexpr int num_shop_items = num_item_col * num_item_row;
	static const tuple<float, float, float> shopRenderDrawColor;
	typedef ShopItem* shopItemBucket;
	ShopItem** shopItems[num_tabs];
	//text, size, position
	static const std::tuple<std::string, FVector2, FVector2> tabDatas[];
	int currentTabIndex;
	int currentButtonIndex;
	std::string promptMessage;
	Node<std::function<void(void)>>* pauseUpdateNode;
	Node<Entity*>* backgroundNode;
	Node<Entity*>* foregroundNode;
	Node<Entity*>* backDropUINode;
	Node<Entity*>* tabUINode;
	Node<Entity*>* selectedButtonUINode;
	Node<Entity*>* itemUINode;
	Node<Entity*>* buttonUINode;
	Node<Entity*>* cellUINode;
	Node<Entity*>* shopItemNodes[num_shop_items];
	Entity* foregroundEnt;
	Entity* tabUIEnt;
	Entity *buttonUIEnt;
	Entity* itemUIEnt;
	Timer purchaseAttemptTime;
	IntVec2 selectedItemCellPos;
	void Update(void);
	inline bool GetShopKey(void) {
		return Main::KeyPressed(enter_shop_key);
	}
	void PauseUpdate(void);
	void DestroyPrompt(void);
	struct TextUI {
	public:
		Text::TextData data;
		Text* text = nullptr;
	};
	ShopItem* GetSelectedShopItem(void);
	void InitShopData(TextUI *textUI, IntVec2 size, IntVec2 offset, std::string message);
	void CreateShopText(TextUI &textUI);
	Node<Entity *> *CreateShopUINode(const char *, CCList, FVector2 offset, IntVec2 size);
	void SetHorizontalDifference(Entity* entity, IntVec2 offset, IntVec2 size, int index, int difference);
	void SetHorizontalDifference(Entity* entity, IntVec2 offset, IntVec2 size, IntVec2 cellPosition, IntVec2 difference);
	void SetButtonDifference(void);
	void RenderItemPage(void);
	const inline IntVec2 GetTabTextSize(FVector2 scale) const {
		return static_cast<FVector2>(IntVec2(shopTabSize)) * shopTabTextScale * scale;
	}
	Text* promptText;
	TextUI shopTitle = TextUI{
		.data = Text::TextData(
			IntVec2(270, 145),
			IntVec2(930 + ShopOffset.x, 155 + ShopOffset.y),
			"Shop"
			)
	};
	TextUI itemNameText = TextUI{
		.data = Text::TextData(
			IntVec2(270, 145),
			IntVec2(-28 + ShopOffset.x, -388 + ShopOffset.y),
			"broken"
			)
	};
	TextUI itemDescText = TextUI{
		.data = Text::TextData(
			IntVec2(270, 145),
			IntVec2(-28 + ShopOffset.x, -388 + ShopOffset.y),
			"broken"
			)
	};
	TextUI tabText;
	TextUI confirmButtonText = TextUI{
		.data = Text::TextData(
			IntVec2(110, 45),
			IntVec2(1062 + ShopOffset.x, 896 + ShopOffset.y),
			"Confirm"
			)
	};
	TextUI cancelButtonText = TextUI{
		.data = Text::TextData(
			IntVec2(110, 45),
			IntVec2(1062 + ShopOffset.x, 896 + ShopOffset.y) - IntVec2::GetRight() * buttonDifference,
			"Cancel"
		)
	};
	static constexpr FVector2 ShopOffset = FVector2(-351.f, .0f);
	static constexpr IntVec2 promptSize = IntVec2(315, 40);
	static constexpr IntVec2 promptOffset = IntVec2(-10, -35);
	static constexpr IntVec2 shopTabOffset = IntVec2(-191.f + ShopOffset.x, -271.99f + ShopOffset.y);
	static constexpr IntVec2 selectedButtonOffset = IntVec2(-133.f + ShopOffset.x, 306.f + ShopOffset.y);
	static constexpr IntVec2 selectedItemOffset = IntVec2(-249.f + ShopOffset.x, -136.f + ShopOffset.y);
	static constexpr IntVec2 buttonOffset = IntVec2(-133.f + ShopOffset.x, 290.f + ShopOffset.y);
	static constexpr IntVec2 shopTabTextOffset = IntVec2(20.f, 37.f);
	static constexpr IntVec2 shopSize = IntVec2(207 * shopScale, 183 * shopScale);
	static constexpr float backDropScale = 3.9f;
	static constexpr IntVec2 shopTabSize = IntVec2(32 * backDropScale, 23 * backDropScale);
	static constexpr IntVec2 buttonSize = IntVec2(64.f * backDropScale, 27.f * backDropScale);
	static constexpr IntVec2 selectedItemSize = IntVec2(34.f * backDropScale, 34.f * backDropScale);
	static constexpr IntVec2 buttonDenyDifference = IntVec2(-.5f * buttonSize.x, -.5f * buttonSize.y);
	static constexpr FVector2 shopTabTextScale = FVector2(.7f, .55f);
	static constexpr SDL_Color shopFontColor = SDL_Color(176, 154, 134, 255);
	static constexpr IntVec2 backDropSize = IntVec2(404 * backDropScale, 256 * backDropScale);
	static constexpr FVector2 backDropOffset = FVector2(.0f, -70.0f);
	static constexpr FVector2 shopFrontOffset = FVector2(.0f, 165.f);
	static constexpr FVector2 firstItemOffset = FVector2(-245.f + ShopOffset.x, -120.f + ShopOffset.y);
	static constexpr FVector2 itemDifference = FVector2(125.f, 125.f);
	Text::TextData promptData;
	~Shop(void);
public:
	Shop(void);
	friend class Environment;
};