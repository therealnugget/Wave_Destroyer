#include "Crystal.hpp"
void Crystal::SetHomeForce(float destoryDist, float maxHomeTime, float forceAdd, float& currentForce, RigidBody *rigidBody, std::function<void(void)> onCollect, Timer *homingTimer) {
	auto dt = Main::DefCapDeltaTime();
	auto toPlr = FVector2::FromTo(rigidBody->GetPosition(), Player::GetPosition());
	rigidBody->AddForce(toPlr.Normalized() * currentForce * dt);
	currentForce += forceAdd * dt;
	if (toPlr.SqrMagnitude() < destoryDist || homingTimer->GetElapsedSeconds() > maxHomeTime) {
		onCollect();
	}
}
void Crystal::Update(void) {
	if (!home) {
		if (aliveTime.GetElapsedSeconds() > max_alive_seconds) goto destroy;
		return;
	}
	SetHomeForce(crystal_destroy_dist, max_crystal_home_time, crystal_in_force_add, crystalInForce, rb, [this]() { entity->SetAnimation(collect); }, crystalHomeTimer);
	if (!entity->AnimFinished(collect)) return;
	Player::IncreaseProgress(progressAmount);
destroy:
	_freea(crystalHomeTimer);
	delete this;
}