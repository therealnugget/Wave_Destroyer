#include "Environment.hpp"
#include "multicast delegates.hpp"
#include "camera.hpp"
Node<std::function<void(void)>>* Environment::lateUpdateNode = nullptr;
Node<std::function<void(void)>>* Environment::updateNode = nullptr;
Node<Entity*>* Environment::treeEntNodes[Environment::numTrees];
Shop* Environment::shop = nullptr;
Timer Environment::shopTimer;
FVector2 Environment::lastInpVec = FVector2::Zero;
IntVec2 Environment::minTreePos;
IntVec2 Environment::maxTreePos;
IntVec2 Environment::RandOffScreenPos(void) {
	auto randPos = Main::GetRandIVec(Camera::GetCamExtentWorld(-1.f), Camera::GetCamExtentWorld(1.f)) + GetRandExtentInput();
	Main::AssignIfLess(randPos, minTreePos);
	Main::AssignIfMore(randPos, maxTreePos);
	return randPos;
}
IntVec2 Environment::GetRandExtentInput(void) {
	return Main::halfDisplaySizeI * lastInpVec * 2;
}
void Environment::OnShopDestroy(void) {
	shop = nullptr;
}
void Environment::Update(void) {
	//if (shopTimer.GetElapsedSeconds() > shop_time) {
	//	shopTimer.Reset();
	//	if (shop) {
	//		//shop assigned to nullptr in destructor
	//		delete shop;
	//	}
	//	shop = new Shop();
	//}
	//if (shop) shop->Update();
}
void Environment::Init(void) {
	shopTimer.Reset(shop_time * spawn_shop_at_start);
	lateUpdateNode = (Main::LateUpdates += []() {LateUpdate(); });
	updateNode = (Main::Updates += Update);
	for (auto &tree: treeEntNodes) {
		tree = Physics::SubStandaloneEnt(Entity::MakeEntity("environment", { "green tree" }, FVector2::Zero, treeSize, true, Main::RenderType::renderUnder, RandOffScreenPos() - static_cast<FVector2>(treeSize) * .5f));
	}
}
void Environment::ResetTreePosExtents(void) {
	minTreePos = FVector2::Infinity;
	maxTreePos = FVector2::NegInfinity;
}
void Environment::LateUpdate(void) {
	auto randExtentInpPos = GetRandExtentInput();
	auto isPosX = lastInpVec.x > .0f;
	auto isPosY = lastInpVec.y > .0f;
	auto addNeg = IntVec2(randExtentInpPos.x * !isPosX, randExtentInpPos.y * !isPosY);
	auto addPos = IntVec2(randExtentInpPos.x * isPosX, randExtentInpPos.y * isPosY);
	auto minRangeCheck = Camera::GetCamExtentWorld(-1.f) + addNeg;
	auto maxRangeCheck = Camera::GetCamExtentWorld(1.f) + addPos;
	if (minTreePos.InRange(minRangeCheck, maxRangeCheck) && maxTreePos.InRange(minRangeCheck, maxRangeCheck) || !Main::moving) {
		ResetTreePosExtents();
		return;
	}
	ResetTreePosExtents();
	for (auto& tree : treeEntNodes) {
		if (tree->value->GetRenderOffset().InRange(minRangeCheck, maxRangeCheck)) continue;
		tree->value->SetRenderOffset(RandOffScreenPos());
	}
	if (Main::moving) lastInpVec = Main::fInputVec;
}
Environment::~Environment(void) {
	for (auto tree : treeEntNodes) {
		Physics::UnsubStandaloneEnt(tree);
	}
#ifdef DEBUG_BUILD
	if (!lateUpdateNode) ThrowError("update node should not be nullptr, because it should be assigned on main(int, char*[]).");
#endif
	Main::Updates -= lateUpdateNode;
}