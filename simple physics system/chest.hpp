#pragma once
#include "main.hpp"
#include "timer.hpp"
#include "physics.hpp"
//static
class ChestSpawner final {
private:
	static Node<std::function<void(void)>>* updateNode;
	static void Update(void);
	static void RandomizeTimer(void);
	static Timer chestSpawnTimer;
	constexpr static float chest_rand_time_min = 10.f;
	constexpr static float chest_rand_time_max = 14.f;
public:
	static void Init(void);
};
class Chest final : public Behaviour {
private:
	constexpr static FVector2 chest_collider_size = FVector2(1.f, 13.f / 16.f);
	constexpr static FVector2 chest_size = FVector2(30.f, 30.f * 13.f / 16.f);
	static constexpr float max_alive_time = 30.f;
	Timer destroyTimer;
	enum chestAnims {
		idle = 0,
		opening = 1,
		numAnims = 2,
	};
	void OnCollision(Collision*);
	void Update(void) override;
public:
	Chest(void);
};
class HealthPotion final : public Behaviour {
private:
	constexpr static FVector2 health_potion_collider_size = FVector2(1.f, 1.f);
	constexpr static FVector2 health_potion_size = FVector2(30.f, 30.f * 13.f / 16.f);
	constexpr static float home_force_add = 400000000.f;
	constexpr static float max_home_time = 1.f;
	constexpr static float destroy_dist = 30.f;
	constexpr static float start_in_speed = 50000000.f;
	static constexpr float destroy_time = .15f;
	static constexpr float max_alive_time = 10.f;
	float currentHomeForce;
	void OnCollision(Collision*);
	void Update(void) override;
	Timer destroyTimer;
	bool collected;
public:
	HealthPotion(FVector2);
};