#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class SwordGuy final: public Enemy {
private:
	void CheckResetAttackDecision(void);
	void TakeDamage(float damageAmount) override;
	void Update(void) override;
	enum SGAnimation {
		idle,
		death,
		hurt,
		walk,
		run,
		jump,
		overhead,
		swing,
		dash,
		numSGAnims,
	};
	static constexpr float dashPositionAdd = 200.f;
	static constexpr float attackDistSqr = 10000.f;
	static constexpr float dashDistSqr = 17000.f;
	//the chance that one of these enemies will spawn as a boss is (1 / bossChance).
	static int bossChance;
	//dash + 1 for yet to decide
	int attackDecision;
	bool attackDecide;
	float signAtDash;
	void ResetIfAttkFin(int animation);
public:
	SwordGuy();
	~SwordGuy(void) override {
	}
};