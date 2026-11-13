#include "shop.hpp"
#include "Environment.hpp"
#include "player.hpp"
const std::string Shop::shopTitleMessage = std::string("Shop");
const std::string Shop::tabMessages[num_tabs] = { std::string("buffs"), std::string("weapons"), std::string("armor"), std::string("items")};
const std::tuple<float, float, float> Shop::shopRenderDrawColor = { .0f, .0f, .0f };
Text::TextData Shop::GetShopData(IntVec2 size, IntVec2 offset, std::string message) {
	return Text::TextData(size, Main::halfDisplaySizeI + offset, message, true, true, shopFontColor);
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
	shopTitleData = GetShopData(shopTitleSize, shopTitleOffset, shopTitleMessage);
	tabData = GetShopData(shopTabSize, shopTabOffset, tabMessages[0]);
	tabText = nullptr;
	shopTitle = nullptr;
	promptText = nullptr;
	backDropUINode = nullptr;
	pauseUpdateNode = nullptr;
	promptMessage = std::string();
	currentTabIndex = 0;
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
		//delete tabText;
	}
	DestroyPrompt();
	if (!pauseUpdateNode) return;
	Main::PauseUpdates -= pauseUpdateNode;
	delete shopTitle;
}
Node<Entity*>* Shop::GetShopUINode(const char* basePaths, CCList animPaths, FVector2 offset, IntVec2 size){
	return Physics::SubStandaloneEnt(Entity::MakeEntity(basePaths, animPaths, size * -.5f + offset, size, false, Main::RenderType::renderWhenPaused), true);
}
void Shop::PauseUpdate(void) {
	if (!backDropUINode) {
		shopTitle = new Text(&shopTitleData);
		//tabText = new Text(&tabData);
		backDropUINode = GetShopUINode("UI/shop", { "shop UI" }, backDropOffset, backDropSize);
		tabUINode = GetShopUINode("UI/shop", { "selected tab" }, static_cast<IntVec2>(shopTabOffset), shopTabSize);
		tabUIEnt = tabUINode->value;
	}
	currentTabIndex = std::clamp(currentTabIndex + Main::KeyPressed(SDL_SCANCODE_RIGHT) - Main::KeyPressed(SDL_SCANCODE_LEFT), 0, num_tabs);
	tabUIEnt->SetRenderOffset(shopTabSize / 2 + shopTabOffset + IntVec2::GetRight() * shopTabDifference);
	if (!GetShopKey()) return;
	Main::SetPauseState(false);
	delete this;
}
void Shop::Update(void) {
	if ((Player::GetPosition() - (static_cast<FVector2>(foregroundEnt->GetRenderOffset()) + FVector2(static_cast<IntVec2>(shopSize)) * .5f + shopFrontOffset)).SqrMagnitude() < playerPromptDistSqr) {
		promptText = new Text(&promptData);
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