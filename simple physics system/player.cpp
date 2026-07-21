#include "player.hpp"
#include "textures.hpp"
#include "winMgr.hpp"
#include <tuple>
#include "create shapes.hpp"
#include "array.hpp"
#include "main.hpp"
#include <algorithm>
#include "animations.hpp"
#include "multicast delegates.hpp"
#include "Item.hpp"
#include "camera.hpp"
#include "PlayerProjectile.hpp"
#define PLAYER_WIDTH 70
#define PLAYER_HEIGHT 70
float Player::accel = 700000.f;
float Player::speed = initSpeed;
float Player::runSpeedMultAdd = 1.75f;
float Player::knockBack = 30.0f;
float Player::plrAttkET = .0f;
float Player::maxHealth = 50.f;
float Player::health = Player::maxHealth;
float Player::crystalColldierSizeMult = 1.4f;
float Player::staminaDecreaseSpeed = 4000.f;
FVector2 Player::mouseDiff;
const FVector2 Player::playerCollider = FVector2(.375f, .5f);
const std::string Player::spear_base_path = "spear";
const std::string Player::sword_base_path = "sword slash";
static constexpr float swordSizeMult = 1.2f;
IntVec2 Player::swordSize = IntVec2(PLAYER_WIDTH * swordSizeMult, PLAYER_HEIGHT * swordSizeMult);
static constexpr float spearSizeMult = .9f;
IntVec2 Player::spearSize = IntVec2(PLAYER_WIDTH * spearSizeMult, PLAYER_HEIGHT * spearSizeMult);
IntVec2 Player::pastInp;
Timer Player::immuneTimer;
Timer Player::sprintCooldown;
bool Player::mouseVertical;
bool Player::colOnFrame = false;
bool Player::enabled = true;
bool Player::pastStaminaPositive = true;
rbList* Player::plrNode;
rbList* Player::plrAttack = nullptr;
RigidBody *Player::player;
Behaviour *Player::plrBehaviour;
Entity* Player::playerEnt;
static FVector2 currentVel;
//#define SHOW_AABB
#ifdef SHOW_AABB
static void SetPositions(rbList** rbs, int i, int j, RigidBody *rb) {
	rbs[i]->value->SetPosition(rb->GetNarrowPhaseVertices()[j] + ((static_cast<FVector2>(FVector2::Left) + FVector2::Down) * 2.5f));
}
#endif
static IntVec2 playerSize = IntVec2(static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT));
static IntVec2 playerSizeFVec = static_cast<FVector2>(playerSize);
FVector2 Player::healthBarOffset = { -21.f, -35.f };
IntVec2 Player::healthBarSize = IntVec2(40, 20);
FVector2 Player::progressBarPos = { 0.0f, 17.f };
IntVec2 Player::progressBarInitSize = IntVec2(2000, 30);
FVector2 Player::staminaBarPos = { 0.0f, .0f };
IntVec2 Player::staminaBarInitSize = IntVec2(2000, 17);
IntVec2 Player::plrAttkPos;
FVector2 Main::defaultPlrPos;
IntVec2 Main::defaultPlrPosI;
float Player::maxProgress = 10.f;
float Player::progressIncrease = 1.5f;
float Player::progressAmount = .0f;
float Player::projectileSpd = 600.f;
float Player::damage = init_damage;
float Player::spearDamageMultiplier = 2.5f;
Node<Entity*> *Player::healthbar;
Entity* Player::healthBarEnt;
Node<Entity*> *Player::progressBar;
Entity* Player::progressBarEnt;
Node<Entity*> *Player::staminaBar;
Entity* Player::staminaBarEnt;
Node<RigidBody*>* Player::crystalCollider;
RigidBody* Player::crystalColliderRb;
void Player::IncreaseProgress(float add) {
	progressAmount += add;
	if (progressAmount > maxProgress) {
		progressAmount -= floorf(progressAmount / maxProgress) * maxProgress;
		for (int i = 0; i < Item::numItems; i++) {
			Item::MakeRandItem(i);
		}
		Main::TogglePauseState();
		Main::canChangePause = false;
		maxProgress *= progressIncrease;
	}
	progressBarEnt->SetSizeX((progressAmount) / maxProgress * static_cast<float>(progressBarInitSize.x));
}
void Player::RegenHealth(float amount) {
	health = min(maxHealth, health + amount);
}
void Player::RegenStamina(float amount) {
	auto maxStamina = staminaBarInitSize.x;
	staminaBarEnt->SetSizeX(std::max(staminaBarEnt->GetSizeX() + static_cast<int>(static_cast<float>(maxStamina) * amount), maxStamina));
}
void Player::TakeDamage(float damage) {
#ifdef IS_DEV
	return;
#endif
	if (immuneTimer.GetElapsedSeconds() < immuneTime) return;
	health -= damage;
	PlayDirAnim(hit, pastInp);
	immuneTimer.Reset();
	if (health > .0f) return;
	Main::SetPauseState(true);
	Main::canChangePause = false;
	enabled = false;
}
void Player::IncreaseHealth(float increaseFactor) {
	maxHealth *= 1.f + increaseFactor;
}
void Player::ReplenishHealth(void) {
	health = maxHealth;
}
void Player::IncreasePickupRange(float increaseFactor) {
	crystalColliderRb->ScaleNarrowPVert(1.f + increaseFactor);
}
void Player::Init(void) {
	Main::defaultPlrPos = Main::halfDisplaySize + (static_cast<FVector2>(FVector2::Down) + FVector2::Left) * playerSizeFVec * .5f;
	Main::defaultPlrPosI = static_cast<IntVec2>(Main::defaultPlrPos);
	auto defPlrPos = Main::defaultPlrPos;
	auto crystalData = SubRBData("", std::vector<const char*>(), playerSizeFVec * crystalColldierSizeMult * Physics::DefaultSquareVerticesVec, defPlrPos);
	crystalData.isTrigger = true;
	crystalData.layer = Main::Layer::crystalLayer;
	crystalData.tag = Main::Tag::playerTrigCrystal;
	crystalData.createEntity = false;
	crystalCollider = Physics::SubscribeEntity(&crystalData);
	crystalColliderRb = crystalCollider->value;
	plrBehaviour = new Behaviour(SubRBData("main/Char_Sprites", Animations::MakeAnimStrs(numAnims, idle, "idle", run, "run", attack, "attack", hit, "hit"), static_cast<FVector2>(playerCollider) * Physics::DefaultSquareVerticesVec, defPlrPos, playerSize, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f, Main::Tag::player, true));
	plrNode = plrBehaviour->rbNode;
	player = plrNode->value;
	immuneTimer = Timer();
	sprintCooldown = Timer(sprint_cooldown_time);
	Camera::cameraPosition = IntVec2(player->GetPosition());
	player->SetTrigger(true);
	player->updateNode = Main::Updates += Player::Update;
	Main::LateUpdates += Player::LateUpdate;
	player->SetCollisionCallback([](Collision *collision) -> void {

		});
	playerEnt = player->GetEntity();
	PlayDirAnim(idle);
	playerEnt->SetNotLoopDirs(Main::GetAnimOffset(hit));
	playerEnt->SetRecordAnim(true);
	healthbar = Physics::SubStandaloneEnt(Entity::MakeEntity("health bar", { "health_bar" }, healthBarOffset, healthBarSize));
	healthBarEnt = healthbar->value;
	healthBarEnt->SetNotLoop(healthBarAnim);
	progressBarPos += -defPlrPos;
	progressBar = Physics::SubStandaloneEnt(Entity::MakeEntity(Main::empty_string, { "progress_bar" }, progressBarPos, progressBarInitSize * IntVec2::GetUp()));
	progressBarEnt = progressBar->value;
	staminaBarPos += -defPlrPos;
	staminaBar = Physics::SubStandaloneEnt(Entity::MakeEntity(Main::empty_string, { "stamina_bar" }, staminaBarPos, staminaBarInitSize));
	staminaBarEnt = staminaBar->value;
	constexpr int numShapes = 0;
	if (!numShapes) return;
	constexpr float scaleFact = .1f;
	FVector2 shapeSize = static_cast<FVector2>(playerSize) * scaleFact;
	constexpr float border = .05f;
	constexpr float invBorder = 1.f - border;
	//don't use createshapes using first param as numshapes here, because we need to randomize the positions.
	for (int i = 0; i < numShapes; i++) Shapes::CreateShapes(1, /*Main::halfDisplaySize + FVector2::GetRight() * (100.f + (static_cast<float>(static_cast<bool>(i & 1)) * 2.f - 1.f) * static_cast<float>(i) * .5f * shapeSize.x)*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize) * border), Main::DisplaySize * invBorder), shapeSize, scaleFact, Shapes::blueSqr, std::initializer_list<FVector2>(), FVector2::Zero, -shapeSize * .5f, false);//nb: this line isn't causing the current error
}
static constexpr float playerAttackSizeMult = 1.2f;
static constexpr float playerAttackOut = .2f;
void Player::LateUpdate(void) {
	auto pos = GetPosition();
	Camera::cameraPosition = IntVec2(pos);
	crystalColliderRb->SetPosition(pos);
	if (Main::clicking) {
		mouseDiff = GetMouseDiff();
		auto mouseRightDot = GetPlayerRightNorm() ^ mouseDiff;
		auto mouseUp = (GetPlayerUpNorm() ^ mouseDiff) >= .0f;
		mouseVertical = mouseRightDot < .5f && mouseRightDot > -.5f;
		auto mouseDirection = IntVec2(!mouseVertical * ((mouseRightDot >= .0f) * 2 - 1), mouseVertical * (mouseUp * 2 - 1));
		PlayDirAnim(attack, mouseDirection);
		pastInp = mouseDirection;
		plrAttkPos = pos + static_cast<FVector2>(mouseDiff * playerSize.x * playerAttackOut);
		if (!plrAttack) {
			RigidBody* attackRB;
			Entity* attackEnt;
			plrAttack = CreatePlayerProjectile(!Main::leftClick ? spear_base_path : sword_base_path, !Main::leftClick ? spear_end_path : sword_end_path, plrAttkPos, spearSize * Main::rightClick + swordSize * Main::leftClick, &attackRB, &attackEnt, mouseDiff.Angle());
			damage = init_damage * spearDamageMultiplier * !Main::leftClick + init_damage * Main::leftClick;
			attackEnt->SetNotLoop(attkSlashAnim);
			attackRB->SetFricCoef(.0f);
			attackEnt->SetAnimSpd(attackSlashAnimSpeed);
			plrAttkET = .0f;
		}
		auto attackEnt = plrAttack->value->GetEntity();
		if (attackEnt->AnimFinished()) {
			attackEnt->ResetAnim(attkSlashAnim);
			plrAttkET = .0f;
		}
		plrAttack->value->SetRotation(static_cast<double>(mouseDiff.Angle()) + 180.f + spearRotationOffset * !Main::leftClick);
		plrAttkET += Main::DeltaTime();
		plrAttack->value->SetPosition(GetProjectilePos(plrAttkET, mouseDiff));
		return;
	}
}
rbList *Player::CreatePlayerProjectile(std::string basePath, const char* endPath, IntVec2 position, IntVec2 size, RigidBody **outRB, Entity ** outEnt, float rotation) {
	auto data = SubRBData(basePath, { endPath }, Physics::DefaultSquareVerticesVec, position, size, std::initializer_list<FVector2>(), FVector2::Zero, size * -.5f, Main::Tag::playerAttack, true, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, rotation, 1.f, true, true, { Main::empty_cc });
	auto node = Physics::SubscribeEntity(&data);
	if (outRB) *outRB = node->value;
	if (outEnt) {
#ifdef DEBUG_BUILD
		if (!outRB) ThrowError("cannot pass outEnt without passing in outRB");
#endif
		*outEnt = (*outRB)->GetEntity();
	}
	return node;
}
IntVec2 Player::GetProjectilePos(float out, FVector2 mouse) {
	return plrAttkPos + mouse * out * projectileSpd;
}
void Player::Update(void) {
	if (!enabled) return;
	colOnFrame = false;
	player->AddForce(Main::fInputVec * accel);
	//player2Rb->AddForce(Main::fInputVec2 * accel);
	currentVel = player->GetVelocity();/*
	cout << "player is located at ";
	player->GetPosition().PrintVec();
	cout << "and player 2 is located at ";
	player2Rb->GetPosition().PrintVec();*/
	if (currentVel.Magnitude() > speed) player->SetVelocity(currentVel.Normalized() * speed);
	//currentVel = player2Rb->GetVelocity();
	//if (currentVel.Magnitude() > speed) player2Rb->SetVelocity(currentVel.Normalized() * speed);
#ifdef SHOW_AABB
	for (int i = 0; i < 2; i++) {
		int j = i ? 2 : 0;
		//the last term is just to correct for the size of the square itself.
		SetPositions(shapesTemp, i, j, player);
		SetPositions(shapesTemp2, i, j, player2Rb);
		/*cout << "vertex at " << std::to_string(j) << " is ";
		player->GetNarrowPhaseVertices()[j].PrintVec();*/
	}
#endif
	healthBarEnt->SetAnimFrame(static_cast<int>(floorf(GetHealthFrac() * static_cast<float>(healthBarEnt->GetNumAnimFrames() - 1))));
	auto stamina = staminaBarEnt->GetSizeX();
	auto staminaPos = !!stamina;
	auto isSprinting = Main::GetKey(SDL_SCANCODE_LSHIFT) && Main::moving && staminaPos;
	auto dt = Main::DeltaTime();
	if (stamina <= 0 && pastStaminaPositive) sprintCooldown.Reset();
	staminaBarEnt->SetSizeX(std::clamp(stamina + static_cast<int>(staminaDecreaseSpeed * dt) * ((sprintCooldown.GetElapsedSeconds() >= sprint_cooldown_time && !isSprinting) * 2 - 1), 0, staminaBarInitSize.x));
	pastStaminaPositive = staminaPos;
	speed = (isSprinting * runSpeedMultAdd + 1.f) * initSpeed;
	if (Main::GetKey(SDL_SCANCODE_O)) player->SetRotation(player->GetRotation() + rotationSpd * dt);
	if (!Main::clicking && plrAttack) {
		Physics::DeleteRB(plrAttack);
		plrAttack = nullptr;
	}
	if (PlayingHurtAnim() || Main::clicking) return;
	if (Main::moving) {
		Player::PlayDirAnim(run, Main::iInputVec);
		pastInp = Main::iInputVec;
		return;
	}
	Player::PlayDirAnim(idle, pastInp);
}
void Player::PlayDirAnim(int animation, IntVec2 direction) {
	bool isHit = animation == hit;
	playerEnt->SetAnimSpd(hurtAnimSpeed * isHit + 1.f * !isHit);
	playerEnt->SetAnimation(IntVec2::VecToDir(direction) + Main::GetAnimOffset(animation));
}