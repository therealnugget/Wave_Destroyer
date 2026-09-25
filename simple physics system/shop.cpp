#include "shop.hpp"
#include "Environment.hpp"
#include "player.hpp"
Text* Shop::promptText = nullptr;
Text* Shop::shopTitle = nullptr;
Text::TextData Shop::promptData;
Text::TextData Shop::shopTitleData;
const std::string Shop::shopTitleMessage = std::string("Shop");
Node<std::function<void(void)>>* Shop::pauseUpdateNode = nullptr;
std::string Shop::promptMessage = std::string();
const std::tuple<float, float, float> Shop::shopRenderDrawColor = { .0f, .0f, .0f };
Shop::Shop(void) {
	auto startPos = Environment::RandOffScreenPos() - FVector2(static_cast<IntVec2>(shopSize)) * .5f;
	backgroundNode = Physics::SubStandaloneEnt(Entity::MakeEntity("shop", { "shop_background" }, FVector2::Zero, shopSize, true, Main::RenderType::renderUnder, startPos));
	foregroundNode = Physics::SubStandaloneEnt(Entity::MakeEntity("shop", { "shop_foreground" }, FVector2::Zero, shopSize, true, Main::RenderType::normal, startPos));
	foregroundEnt = foregroundNode->value;
	if (!promptMessage.empty()) return;
	//have to assign the prompt message in the constructor instead of at compile time to make sure Main::scancode_to_char is initialized first.
	promptMessage = std::string("Press ") + Main::GetScancodeCharacter(enter_shop_key) + " to enter shop.";
	promptData = Text::TextData(promptSize, Main::halfDisplaySizeI + promptOffset, promptMessage);
	shopTitleData = Text::TextData(shopTitleSize, Main::halfDisplaySizeI + shopTitleOffset, shopTitleMessage, true, true, Main::color_white);
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
	if (backDropUINode) Physics::UnsubStandaloneEnt(backDropUINode);
	DestroyPrompt();
	if (!pauseUpdateNode) return;
	Main::PauseUpdates -= pauseUpdateNode;
	delete shopTitle;
}
void Shop::PauseUpdate(void) {
	if (!backDropUINode) {
		backDropUINode = Physics::SubStandaloneEnt(Entity::MakeEntity("UI/shop", { "shop UI" }, backDropSize * -.5f + backDropOffset, backDropSize, false, Main::RenderType::renderWhenPaused));
		shopTitle = new Text(&shopTitleData);
	}
	if (!GetShopKey()) return;
	Main::SetPauseState(false);
	delete this;
}
void Shop::Update(void) {
	if ((Player::GetPosition() - (static_cast<FVector2>(foregroundEnt->GetRenderOffset()) + FVector2(static_cast<IntVec2>(shopSize)) * .5f + shopFrontOffset)).SqrMagnitude() < playerPromptDistSqr) {
		if (!promptText) {
			promptText = new Text(&promptData);
		}
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