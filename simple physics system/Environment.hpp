#pragma once
#include "main.hpp"
#include "physics.hpp"
#include "timer.hpp"
#include "shop.hpp"
//static
class Environment final {
private:
	//number of trees on screen at any given time
	static constexpr int numTrees = 10;
	static constexpr float shopScale = 2.f;
	static constexpr float shop_time = 60.f;
	static constexpr bool spawn_shop_at_start = true;
	static constexpr FVector2 treeSize = FVector2(110.f, 110.f);
	static constexpr FVector2 shopSize = FVector2(191.f * shopScale, 183.f * shopScale);
	static Node<Entity*>* treeEntNodes[numTrees];
	static Shop* shop;
	static Node<std::function<void(void)>> *lateUpdateNode;
	static Node<std::function<void(void)>> *updateNode;
	static void LateUpdate(void);
	static void Update(void);
	static void ResetTreePosExtents(void);
	static IntVec2 GetRandExtentInput(void);
	static FVector2 lastInpVec;
	static IntVec2 minTreePos, maxTreePos;
	static Timer shopTimer;
	~Environment(void);
public:
	static IntVec2 RandOffScreenPos(void);
	static void Init(void);
	static void OnShopDestroy(void);
};