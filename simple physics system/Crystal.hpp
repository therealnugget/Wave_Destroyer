#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "animations.hpp"
#include "multicast delegates.hpp"
#include "player.hpp"
#include "timer.hpp"
#include "debug.hpp"
class Crystal final : public Behaviour {
private:
	void Update(void);
	typedef enum {
		idle,
		collect,
	} CrytalAnimation;
	constexpr static IntVec2 crystal_size = IntVec2(15, 15);
	constexpr static float crystal_out_force = 3000.f;
	constexpr static float crystal_destroy_dist = 23.f;
	constexpr static float crystal_in_force_add = 200000000.f;
	constexpr static float max_crystal_home_time = 2.5f;
	constexpr static float max_alive_seconds = 20.f;
	constexpr static float start_in_speed = 40000000.f;
	constexpr static const char *blueCrystal = "main/crystal", *greenCrystal = "main/green crystal", * redCrystal = "main/red crystal";
	bool home;
	float crystalInForce;
	float progressAmount;
	Timer *crystalHomeTimer;
	Timer aliveTime;
public:
	typedef enum {
		blue_crystal = 0,
		green_crystal = 1,
		red_crystal = 2,
	} TypeOfCrystal;
	static void SetHomeForce(float destroyDist, float maxHomeTime, float addForce, float& currentForce, RigidBody* rigidBody, std::function<void(void)> onCollect, Timer*);
	static inline const char* GetCrystalPath(TypeOfCrystal typeOfCrystal) {
		switch (typeOfCrystal) {
		case blue_crystal:
			return blueCrystal;
		case green_crystal:
			return greenCrystal;
		case red_crystal:
			return redCrystal;
		}
	}
	static inline int GetProgressAmount(TypeOfCrystal typeOfCrystal) {
		constexpr int blue_crystal_progress = 1;
		constexpr int green_crystal_progress = 2;
		constexpr int red_crystal_progress = 3;
		switch (typeOfCrystal) {
		case blue_crystal:
			return blue_crystal_progress;
		case green_crystal:
			return green_crystal_progress;
		case red_crystal:
			return red_crystal_progress;
		}
	}
	Crystal(FVector2 pos, float _progressAmount = -1.f, TypeOfCrystal typeOfCrystal = blue_crystal): crystalInForce(start_in_speed), home(false), progressAmount(_progressAmount == -1.f ? GetProgressAmount(typeOfCrystal) : _progressAmount), Behaviour(SubRBData(GetCrystalPath(typeOfCrystal), Animations::MakeAnimStrs(2, idle, "crystal_spin", collect, "crystal_collect"), FVector2::GetOne() * 1.f * Physics::DefaultSquareVerticesVec, pos, crystal_size, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2::Zero, Main::Tag::crystal, true, [this](Collision* collision) {
		if (!collision->CompareTag(Main::Tag::playerTrigCrystal) || colOnFrame) return;
		colOnFrame = true;
		home = true;
		crystalHomeTimer = static_cast<Timer *>(_malloca(sizeof(Timer)));
		crystalHomeTimer->Reset();
		rb->SetVelocity(collision->GetNormal().Normalized() * crystal_out_force);
		}, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.0f, false, true, Main::empty_cc_init)) {
		entity->SetNotLoop(collect);
		SetUpdateNode(Main::Updates += [this]() {Update(); });
		rb->SetLayer(Main::Layer::playerLayer);
	}
};