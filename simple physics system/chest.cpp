#include "chest.hpp"
#include "multicast delegates.hpp"
#include "animations.hpp"
#include "camera.hpp"
#include "player.hpp"
#include "Crystal.hpp"
#include <tuple>
Node<std::function<void(void)>>* ChestSpawner::updateNode;
Timer ChestSpawner::chestSpawnTimer;
void ChestSpawner::Init(void) {
	updateNode = (Main::Updates += Update);
	RandomizeTimer();
}
void ChestSpawner::RandomizeTimer(void) {
	chestSpawnTimer.Reset(Main::GetRandFloat(.0f, chest_rand_time_min));
}
void ChestSpawner::Update(void) {
	if (chestSpawnTimer.GetElapsedSeconds() < chest_rand_time_max) return;
	RandomizeTimer();
	new Chest();
}
Chest::Chest(void) : Behaviour(SubRBData("chest"s, Animations::MakeAnimStrs(numAnims, idle, "idle", opening, "opening"), static_cast<FVector2>(chest_collider_size) * Physics::DefaultSquareVerticesVec, Main::GetRandFVec(Camera::GetCamExtentWorld(-1.f), Camera::GetCamExtentWorld(1.f)), chest_size, std::initializer_list<FVector2>(), FVector2::Zero, -chest_size * .5f, Main::Tag::chest, true, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0f, 1.f, false, false, { "right" }, true, .0f, Main::Layer::all)) {
	rb->SetCollisionCallback([this](Collision* col) {OnCollision(col); });
	SetUpdateNode(Main::Updates += [this]() {Update(); });
	entity->SetNotLoop(opening);
}
void Chest::Update(void) {
	Behaviour::Update();
	if (destroyTimer.GetElapsedSeconds() > max_alive_time) {
		delete this;
		return;
	}
	if (!entity->AnimFinished(opening)) return;
	new Potion(rb->GetPosition(), Main::GetRandFloat(.0f, 1.f) > .5f ? Potion::PotionType::healthPotion : Potion::PotionType::staminaPotion);
	delete this;
}
void Chest::OnCollision(Collision *collision) {
	if (!collision->CompareTag(Main::Tag::playerAttack) && !collision->CompareTag(Main::Tag::player)) return;
	colOnFrame = true;
	entity->SetAnimation(opening);
}
void Potion::OnCollision(Collision* collision) {
	if (colOnFrame || !collision->CompareTag(Main::Tag::player)) return;
	colOnFrame = true;
	switch (potionType) {
	case healthPotion:
		Player::RegenHealth(Player::GetMaxHealth() * .25f);
		return;
	case staminaPotion:
		Player::RegenStamina(stamina_add_amount);
	}
}
void Potion::Update(void) {
	if (destroyTimer.GetElapsedSeconds() > max_alive_time) {
		delete this;
		return;
	}
	Crystal::SetHomeForce(destroy_dist, max_home_time, home_force_add, currentHomeForce, rb, [this]() { collected = true; }, &destroyTimer);
	if (colOnFrame) {
		if (!collected) destroyTimer.Reset();
		collected = true;
		if (destroyTimer.GetElapsedSeconds() > destroy_time) delete this;
		return;
	}
}
const std::unordered_map<Potion::PotionType, std::tuple<std::string, const char*>> Potion::potionTypeToName = { { healthPotion, { std::string(health_potion_path), health_potion_path } }, { staminaPotion, { std::string(stamina_potion_path), stamina_potion_path } } };
const char* Potion::stamina_potion_path = "stamina potion";
const char* Potion::health_potion_path = "health potion";
Potion::Potion(FVector2 position, PotionType type): currentHomeForce(start_in_speed), potionType(type), Behaviour(SubRBData(std::get<0>(potionTypeToName.at(type)), {std::get<1>(potionTypeToName.at(type))}, static_cast<FVector2>(potion_collider_size)* Physics::DefaultSquareVerticesVec, position, potion_size, std::initializer_list<FVector2>(), FVector2::Zero, -potion_size * .5f, Main::Tag::_default, true, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0f, 1.f, false, false, Main::empty_cc_init, true, .0f, Main::Layer::all)), destroyTimer(true, destroy_time), collected(false) {
	rb->SetCollisionCallback([this](Collision* col) {OnCollision(col); });
	SetUpdateNode(Main::Updates += [this]() {Update();});
}