#include "shop.hpp"
#include "Environment.hpp"
#include "player.hpp"
#include "Item.hpp"
const std::tuple<std::string, FVector2, FVector2> Shop::tabDatas[num_tabs] = { {std::string("weapons"), FVector2(1.f, .75f), FVector2(.0f, 5.f)}, {std::string("buffs"), FVector2::One, FVector2::Zero}, {std::string("armor"), FVector2::One, FVector2::Zero}, {std::string("equip"), FVector2::One, FVector2::Zero}};
const std::tuple<float, float, float> Shop::shopRenderDrawColor = { .0f, .0f, .0f };
void Shop::InitShopData(TextUI *textUI, IntVec2 size, IntVec2 offset, std::string message) {
	textUI->data = Text::TextData(size, Main::halfDisplaySizeI + offset, message, true, true, shopFontColor);
}
void Shop::CreateShopText(TextUI& textUI) {
	textUI.text = new Text(&textUI.data);
}
Shop::Shop(void) {
	auto startPos = Environment::RandOffScreenPos() - FVector2(static_cast<IntVec2>(shopSize)) * .5f;
	backgroundNode = Physics::SubStandaloneEnt(Entity::MakeEntity("shop", { "shop_background" }, FVector2::Zero, shopSize, true, Main::RenderType::renderUnder, startPos));
	foregroundNode = Physics::SubStandaloneEnt(Entity::MakeEntity("shop", { "shop_foreground" }, FVector2::Zero, shopSize, true, Main::RenderType::normal, startPos));
	foregroundEnt = foregroundNode->value;
	if (!promptMessage.empty()) return;
	//have to assign the prompt message in the constructor instead of at compile time to make sure Main::scancode_to_char is initialized first.
	promptMessage = std::string("Press ") + Main::GetScancodeCharacter(enter_shop_key) + " to enter shop.";
	promptData = Text::TextData(promptSize, Main::halfDisplaySizeI + promptOffset, promptMessage);
	auto initTabData = tabDatas[0];
	InitShopData(&tabText, GetTabTextSize(std::get<1>(initTabData)), shopTabOffset + shopTabTextOffset, std::get<std::string>(initTabData));
	promptText = nullptr;
	backDropUINode = nullptr;
	pauseUpdateNode = nullptr;
	promptMessage = std::string();
	currentTabIndex = 0;
	for (int i = 0; i < num_tabs; i++) {
		shopItems[i] = new shopItemBucket[num_shop_items]();
	}
	shopItems[0][0] = new BloodSword();
	shopItems[0][11] = new BloodSword(20);
	currentButtonIndex = 1;
	selectedItemCellPos = IntVec2::Zero;
}
void Shop::DestroyPrompt(void) {
	if (!promptText) return;
	delete promptText;
	promptText = nullptr;
}
Shop::~Shop(void) {
	Environment::OnShopDestroy();
	Physics::UnsubStandaloneEnt(backgroundNode);
	Physics::UnsubStandaloneEnt(foregroundNode);
	if (backDropUINode) {
		Physics::UnsubStandaloneEnt(backDropUINode);
		Physics::UnsubStandaloneEnt(tabUINode);
		delete tabText.text;
		if (buttonUINode) Physics::UnsubStandaloneEnt(buttonUINode);
		delete confirmButtonText.text;
		delete cancelButtonText.text;
		for (int i = 0; i < num_shop_items; i++) {
			if (!shopItemNodes[i]) continue;
			Physics::UnsubStandaloneEnt(shopItemNodes[i]);
		}
	}
	DestroyPrompt();
	if (!pauseUpdateNode) return;
	Main::PauseUpdates -= pauseUpdateNode;
	delete shopTitle.text;
}
Node<Entity*>* Shop::CreateShopUINode(const char* basePaths, CCList animPaths, FVector2 offset, IntVec2 size) {
	return Physics::SubStandaloneEnt(Entity::MakeEntity(basePaths, animPaths, size * -.5f + offset, size, false, Main::RenderType::renderWhenPaused), true);
}
void Shop::SetHorizontalDifference(Entity* entity, IntVec2 offset, IntVec2 size, int index, int difference) {
	entity->SetRectPosition(offset + size / -2 + Main::defaultPlrPos + IntVec2::GetRight() * index * difference);
}
void Shop::SetHorizontalDifference(Entity* entity, IntVec2 offset, IntVec2 size, IntVec2 cellPosition, IntVec2 difference) {
	entity->SetRectPosition(offset + size / -2 + Main::defaultPlrPos + cellPosition * difference);
}
void Shop::SetButtonDifference(void) {
	SetHorizontalDifference(buttonUIEnt, selectedButtonOffset, buttonSize, currentButtonIndex, buttonDifference);
}
//scraps all existing shop items in the page and re-instantiates every shop item entity.
void Shop::RenderItemPage(void) {
	for (int i = 0; i < num_shop_items; i++) {
		auto currentItem = shopItems[currentTabIndex][i];
		auto x = i % num_item_col;
		auto y = i / num_item_col;
		if (shopItemNodes[i]) {
			Physics::UnsubStandaloneEnt(shopItemNodes[i]);
		}
		if (!currentItem) {
			shopItemNodes[i] = nullptr;
			continue;
		}
		shopItemNodes[i] = CreateShopUINode(item_icons_path, { currentItem->GetTexPath() }, static_cast<FVector2>(firstItemOffset) + static_cast<FVector2>(itemDifference) * FVector2(x, y), currentItem->GetSize());
	}
}
void Shop::PauseUpdate(void) {
	bool initialUpdate = !backDropUINode;
	if (initialUpdate) {
		CreateShopText(shopTitle);
		CreateShopText(tabText);
		CreateShopText(confirmButtonText);
		CreateShopText(cancelButtonText);
		backDropUINode = CreateShopUINode(shop_ui_path, { "shop UI" }, backDropOffset, backDropSize);
		tabUINode = CreateShopUINode(shop_ui_path, { "selected tab" }, static_cast<IntVec2>(shopTabOffset), shopTabSize);
		tabUIEnt = tabUINode->value;
	}
	auto keyPressHorizontal = Main::KeyPressed(SDL_SCANCODE_RIGHT) - Main::KeyPressed(SDL_SCANCODE_LEFT);
	auto keyPressVertical = Main::KeyPressed(SDL_SCANCODE_DOWN) - Main::KeyPressed(SDL_SCANCODE_UP);
	if (!itemUINode) {
		currentTabIndex = std::clamp(currentTabIndex + keyPressHorizontal, 0, num_tabs - 1);
		SetHorizontalDifference(tabUIEnt, shopTabOffset, shopTabSize, currentTabIndex, shopTabDifference);
		auto currentTabData = tabDatas[currentTabIndex];
		tabText.text->SetText(std::get<std::string>(currentTabData), GetTabTextSize(std::get<1>(currentTabData)));
		tabText.text->SetPosition(tabUIEnt->GetRectPosition() + shopTabTextOffset + std::get<2>(currentTabData));
	}
	if (keyPressHorizontal || initialUpdate) {
		RenderItemPage();
	}
	/*static int temp4 = 0;
	temp4++;
	if (itemUINode){
		IntVec2 temp = IntVec2(-Main::KeyHeld(SDL_SCANCODE_J) + Main::KeyHeld(SDL_SCANCODE_L), Main::KeyHeld(SDL_SCANCODE_K) - Main::KeyHeld(SDL_SCANCODE_I));
		static IntVec2 temp2 = IntVec2::Zero;
		constexpr int temp3 = 4;
		if ((temp4 & (temp3 - 1)) == 0) temp2 += temp;
	}*/
	auto pressEnterOnFrame = Main::KeyPressed(SDL_SCANCODE_RETURN);
	if (!itemUINode) {
		if (pressEnterOnFrame) {
			tabUIEnt->SetSize(IntVec2::Zero);
			itemUINode = CreateShopUINode(shop_ui_path, { "selected item" }, static_cast<IntVec2>(selectedItemOffset), selectedItemSize);
			itemUIEnt = itemUINode->value;
			RenderItemPage();
		}
	}
	else {
		if (!buttonUINode) {
			if (keyPressHorizontal || keyPressVertical) {
				selectedItemCellPos = IntVec2::Clamp(selectedItemCellPos + IntVec2(keyPressHorizontal, keyPressVertical), IntVec2::Zero, IntVec2(num_item_col, num_item_row) - IntVec2::GetOne());
			}
			SetHorizontalDifference(itemUIEnt, selectedItemOffset, selectedItemSize, selectedItemCellPos, itemDifference);
			if (pressEnterOnFrame) {
				itemUIEnt->SetSize(IntVec2::Zero);
				buttonUINode = CreateShopUINode(shop_ui_path, { "selected button" }, static_cast<IntVec2>(selectedButtonOffset), buttonSize);
				buttonUIEnt = buttonUINode->value;
				SetButtonDifference();
			}
		}
		else {

			currentButtonIndex = std::clamp(currentButtonIndex + keyPressHorizontal, 0, num_button_slots - 1);
			SetButtonDifference();
			if (pressEnterOnFrame) {
				if (currentButtonIndex == confirm_button_index) {
					auto currentShopItem = shopItems[currentTabIndex][selectedItemCellPos.x + selectedItemCellPos.y * num_item_col];
					if (Player::GetCoins() >= currentShopItem->price) {
						currentShopItem->OnPurchase();
						goto resume;
					}
					purchaseAttemptTime.Reset(.0f, false);
				}
				else {
					Physics::UnsubStandaloneEnt(buttonUINode);
					buttonUINode = nullptr;
					itemUIEnt->SetSize(selectedItemSize);
				}
			}
			if (buttonUINode) {
				auto denyEffectTime = confirm_button_size_dilation_add - (std::min(purchaseAttemptTime.GetElapsedSeconds(), purchase_attempt_max_time) / purchase_attempt_max_time) * confirm_button_size_dilation_add;
				buttonUIEnt->SetSize(buttonSize * (1.f + denyEffectTime));
				SetButtonDifference();
				buttonUIEnt->SetRectPosition(buttonUIEnt->GetRectPosition() + buttonDenyDifference * denyEffectTime + IntVec2(std::sinf(denyEffectTime / confirm_button_size_dilation_add * confirm_button_sin_scale) * confirm_button_jiggle, .0f));
			}
		}
	}
	if (!GetShopKey()) return;
resume:
	Main::SetPauseState(false);
	delete this;
}
void Shop::Update(void) {
	if ((Player::GetPosition() - (static_cast<FVector2>(foregroundEnt->GetRenderOffset()) + FVector2(static_cast<IntVec2>(shopSize)) * .5f + shopFrontOffset)).SqrMagnitude() < player_prompt_dst_sqr) {
		if (!promptText) promptText = new Text(&promptData);
		if (GetShopKey()) {
#ifdef DEBUG_BUILD
			if (Main::IsPaused()) throw new exception("timescale was zero when e was pressed");
#endif
			Main::SetBackgroundColor(shopRenderDrawColor);
			Main::SetPauseState(true);
			pauseUpdateNode = (Main::PauseUpdates += [this]() {PauseUpdate();});
		}
		return;
	}
	DestroyPrompt();
}