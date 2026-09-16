#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class SpinningBlade final: public Enemy {
private:
	Node<Entity *> *explosionNode;
	static int numExplosions;
	static constexpr int max_explosions = 15;
	static constexpr IntVec2 explosionSize = { 160, 160 };
	static constexpr float explosionColliderScale = 80.f;
	static constexpr float add_explosion_damage = 3.f;
	static constexpr float toPlrScale = .8f;
	static constexpr float toPlrPerpScale = 1.f - toPlrScale;
	static constexpr float spin_speed_mult = 2.f;
	static const float explosion_damage;
	void TakeDamage(float damageAmount) override;
	void Update(void) override;
	void SetAnim(int);
	enum SpinAnimation {
		idle,
		death,
		hurt,
		attack,
		numSpinAnims,
	};
	static constexpr float attackDistSqr = 12000.f;
	//the chance that one of these enemies will spawn as a boss is (1 / bossChance).
	static int bossChance;
public:
	SpinningBlade(void);
	~SpinningBlade(void) override {
	}
};