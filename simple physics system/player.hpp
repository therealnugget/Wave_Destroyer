#pragma once
#include "debug.hpp"
#include "physics.hpp"
#include <SDL.h>
#include "timer.hpp"
#define IS_DEV
//static, therefore can't inherit from behaviour.
class Player final {
private:
	static constexpr float rotationSpd = 500.f;
	static constexpr float attackSlashAnimSpeed = 1.2f;
	static constexpr float hurtAnimSpeed = 1.f;
	static constexpr float spearRotationOffset = 225.f;
	static constexpr float immuneTime = .13f;
	static constexpr float initSpeed = 650.f;
	static constexpr float sprint_cooldown_time = 1.4f;
	static constexpr float init_damage =
#ifdef IS_DEV
		1000.f
#else
		1.f
#endif
		;
	static const std::string sword_base_path;
	static const std::string spear_base_path;
	static constexpr const char* spear_end_path = "spear";
	static constexpr const char* sword_end_path = "sword_slash";
	static float projectileSpd;
	static constexpr int attkSlashAnim = 0;
	static constexpr int healthBarAnim = 0;
	static const FVector2 playerCollider;
	static float crystalColldierSizeMult;
	static float plrAttkET;
	static float maxHealth;
	static float health;
	static float accel, speed;
	static float runSpeedMultAdd;
	static float staminaDecreaseSpeed;
	static float knockBack;
	static float damage;
	static float spearDamageMultiplier;
	static float progressAmount;
	static float maxProgress;
	static float progressIncrease;
	static bool mouseVertical;
	static bool colOnFrame;
	static bool enabled;
	static bool pastStaminaPositive;
	static Node<std::function<void(void)>>* updateNode;
	static Timer immuneTimer;
	static Timer sprintCooldown;
	static Behaviour* plrBehaviour;
	static rbList* plrNode;
	static rbList* plrAttack;
	static RigidBody* player;
	static Entity* playerEnt;
	static Node<Entity*> *healthbar;
	static Entity* healthBarEnt;
	static Node<Entity*> *progressBar;
	static Entity* progressBarEnt;
	static Node<Entity*> *staminaBar;
	static Entity* staminaBarEnt;
	static Node<RigidBody*> *crystalCollider;
	static RigidBody* crystalColliderRb;
	static FVector2 mouseDiff;
	static FVector2 healthBarOffset;
	static FVector2 progressBarPos;
	static FVector2 staminaBarPos;
	static IntVec2 healthBarSize;
	static IntVec2 progressBarInitSize;
	static IntVec2 staminaBarInitSize;
	static IntVec2 pastInp;
	static IntVec2 swordSize, spearSize;
	static IntVec2 plrAttkPos;
	static void PlayDirAnim(int animation = -1, IntVec2 = IntVec2(-1, 0));
	typedef int (*limitFuncDel)(int, int);
public:
	static inline float GetMaxHealth(void) {
		return maxHealth;
	}
	static inline float GetDamage(void) {
		return damage;
	}
	static IntVec2 GetProjectilePos(float, FVector2);
	static rbList *CreatePlayerProjectile(std::string basePath, const char *endPath, IntVec2 position, IntVec2 size, RigidBody ** outRb = nullptr, Entity ** outEnt = nullptr, float rotation = .0f);
	static inline float GetHealthFrac(void) {
		return health / maxHealth;
	}
	//normal of mouse pos from plr pos
	static inline FVector2 GetMouseDiff(void) {
		return (static_cast<FVector2>(Main::GetMousePosition()) - GetPosition()).Normalized();
	}
	static inline float GetKnockBack(void) {
		return knockBack;
	}
	static inline FVector2 GetPlayerRightNorm(void) {
		return FVector2(cosf(player->GetRotation()), sinf(player->GetRotation()));
	}
	static inline FVector2 GetPlayerUpNorm(void) {
		auto upRotation = player->GetRotation() + 90.f;
		return FVector2(cosf(upRotation), sinf(upRotation));
	}
	static inline FVector2 GetPosition(void) {
		return player->GetPosition();
	}
	static void TakeDamage(float);
	static void RegenHealth(float);
	//.0f to 1.f
	static void RegenStamina(float);
	static void IncreaseProgress(float);
	static void IncreasePickupRange(float);
	static void IncreaseHealth(float);
	static void ReplenishHealth(void);
	static void Init(void);
	static void Finalize(void);
	static void Update(void);
	static void LateUpdate(void);
	inline static bool PlayingHurtAnim() {
		return playerEnt->GetCurAnim() == Main::GetAnimOffset(hit) + IntVec2::VecToDir(pastInp) && !playerEnt->AnimFinished();
	}
	friend class Main;
	static enum PlrAnim {
		idle,
		run,
		attack,
		hit,
		numAnims,
	};
};