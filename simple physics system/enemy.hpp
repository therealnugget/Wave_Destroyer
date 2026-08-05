#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
#include <unordered_map>
class SwordGuy;
class Enemy : public Behaviour {
private:
	static constexpr IntVec2 debuffDefaultOffset = IntVec2(155, 20);
	static constexpr int default_boss_chance = 30;
	static constexpr int default_num_minions = 5;
	static constexpr float boss_health_multiplier = 3.f;
	static constexpr float boss_damage_multiplier = 2.f;
	static constexpr float boss_speed_multiplier = 1.5f;
	static constexpr float default_max_health = 3.f;
	static constexpr float default_damage = 1.f;
	static constexpr float default_self_damage = .5f;
	static constexpr float default_speed = 19000000.f;
	static constexpr float default_minion_spawn_time = 5.f;
	static constexpr float default_minion_time_var = 3.f;
	static const std::unordered_map<int, const char*> debuffPaths;
	struct Debuff {
	public:
		Debuff(Textures::TextureRect _texture, int _index) : texture(_texture), index(_index) {}
		Textures::TextureRect texture;
		int index;
	};
	std::unordered_map<int, Debuff> debuffTexes;
	IntVec2 GetDebuffPos(int index);
	void AddDebuffTex(int debuff);
	IntVec2 debuffImgOffset;
	Timer *minionSpawnTimer;
	float minionSpawnTime;
	float minionSpawnTimeVariance;
	//don't access this directly. use the "GetDebuffActive(int)" and "SetDebuffActive(int)" functions.
	int _debuffActive;
	int frameIndex;
	int numMinions;
	int enemyType;
	bool bIsBoss;
	static std::vector<int> insigniaTagList;
protected:
	bool animFinished;
	static constexpr IntVec2 debuffSize = { 40, 40 };
	static constexpr int debuffSeparation = 25;
	static constexpr int default_num_cols_on_frame = 5;
	static constexpr float boss_size_increase = 2.f;
	static float knockBack;
	float health;
	enum debuffType {
		confused = 1,
		poisoned = 2,
	};
	static inline bool GetIsBoss(int chance) {
		return Main::GetRandInt(0, chance + 1) == 0;
	}
	static inline const int GetDefaultNumMinions(void) {
		return default_num_minions;
	}
	static inline const int GetDefaultBossChance(void) {
		return default_boss_chance;
	}
	static inline const int GetDefaultNumColsOnFrame(void) {
		return default_num_cols_on_frame;
	}
	static inline const float GetDefaultMinionTime(void) {
		return default_minion_spawn_time;
	}
	static inline const float GetDefaultMinionTimeVar(void) {
		return default_minion_time_var;
	}
	static inline const float GetDefaultMaxHealth(void) {
		return default_max_health;
	}
	static inline const float GetDefaultDamage(void) {
		return default_damage;
	}
	static inline const float GetDefaultSelfDamage(void) {
		return default_self_damage;
	}
	//W seed - ur unc if u dont get the reference
	static inline const  float GetDefaultSpeed(void) {
		return default_speed;
	}
	static inline const IntVec2 GetDefaultDebugOffset(void) {
		return debuffDefaultOffset;
	}
	inline bool GetDebuffActive(int debuff) {
		return _debuffActive & debuff;
	}
	inline void SetDebuffActive(int bitIndex, bool value = true) {
		_debuffActive = (_debuffActive | bitIndex) * value + (_debuffActive & ~bitIndex) * !value;
	}
	bool touchingEnemy;
	int numColsOnFrame;
	std::unordered_map<int, bool> colsOnFrame;
	float speed;
	float damage;
	float plrDistSqr;
	float selfDamage;
	FVector2 toPlr;
	int curAnim;
	static int numEnemies;
	static bool isSingleEnemy;
	void SetPlayerDist(void);
	void EnactDamage(void);
	virtual void CollisionCallback(Collision*);
	Enemy(SubRBData, IntVec2 = debuffDefaultOffset, float max_health = default_max_health, float damage = default_damage, float selfDamage = default_self_damage, float speed = default_speed, int numColsOnFrame = default_num_cols_on_frame, bool isBoss = false, float _minionSpawnTime = default_minion_spawn_time, float _minionSpawnTimeVar = default_minion_time_var, int _numMinions = default_num_minions);
	virtual void Update(void);
	virtual void LateUpdate(void);
	Node<std::function<void(void)>>* lateUpdateNode;
	enum EnemyAnimation {
		idle,
		death,
		hurt,
		run,
		walk,
		jump,
		numEnemyAnims,
	};
	//strange, isn't it? a function that calls from the most derived to the base class. eù_e
	std::function<void(float)> derivedTakeDamage;
public:
	inline static int GetNumEnemies(void){
		return numEnemies;
	}
	void UndoBoss(void);
	virtual ~Enemy(void);
	virtual void TakeDamage(float damageAmount);
	void OnDamaged(float, FVector2);
	Node<Enemy*>* enemySpawnNode;
	inline FVector2 GetPosition(void) {
		return rb->newPosition;
	}
	inline static void AddInsigniaTag(int tag) {
		insigniaTagList.emplace(insigniaTagList.begin(), tag);
	}
	friend class SubRBData;
	friend class EnemySpawner;
};