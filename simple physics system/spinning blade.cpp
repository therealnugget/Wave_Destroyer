#include "spinning blade.hpp"
#include "animations.hpp"
#include "player.hpp"
#include "EnemySpawner.hpp"
#include "math.hpp"
#include "camera.hpp"
static constexpr float spinScale = 1.3f;
static constexpr FVector2 spinSize = FVector2(16.f * 2.f * spinScale, 16.f * 2.f * spinScale);
constexpr static float border = .1f;
int SpinningBlade::bossChance = Enemy::GetDefaultBossChance();
int SpinningBlade::numExplosions = 0;
const float SpinningBlade::explosion_damage = Enemy::GetDefaultDamage() + add_explosion_damage;
SpinningBlade::SpinningBlade(void) : explosionNode(nullptr), Enemy(SubRBData("spinning blade", Animations::MakeAnimStrs(numSpinAnims, idle, "idle", death, "death", hurt, "hit", attack, "attack"), FVector2(16.f / 12.f, 12.f / 16.f) * spinScale * Physics::GetDefaultSquareVertVec(), /*Main::halfDisplaySize + FVector2::GetRight() * 350.f*/Main::GetRandFVec(static_cast<FVector2>(Camera::GetCamExtentWorld(1.f)) - Main::halfDisplaySize * border, static_cast<FVector2>(Camera::GetCamExtentWorld(-1.f)) + Main::halfDisplaySize * border), spinSize, std::initializer_list<FVector2>(), FVector2::Zero, IntVec2(-spinSize.x * .5f, -spinSize.y * .5f), Main::Tag::enemy, true, [this](Collision* collision) { CollisionCallback(collision); }, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, .0, 1.f, true, false, {"right"}, true, 32.f / 128.f * spinSize.x, Main::Layer::enemyLayer), IntVec2(7, -40), Enemy::GetDefaultMaxHealth() + 1.5f, Enemy::GetDefaultDamage(), Enemy::GetDefaultSelfDamage(), Enemy::GetDefaultSpeed() * spin_speed_mult, Enemy::GetDefaultNumColsOnFrame(), Enemy::GetIsBoss(bossChance)) {
    entity->SetAnimation(run);
    for (auto& anim : { hurt, death, idle }) {
        entity->SetNotLoop(anim);
    }
    SetUpdateNode(Main::Updates += [this]() {Update();});
    derivedTakeDamage = [this](float damageAmount) { TakeDamage(damageAmount); };
}
void SpinningBlade::TakeDamage(float damageAmount) {
    Enemy::TakeDamage(damageAmount);
    if (!entity) return;
    entity->SetAnimation(hurt);
}
void SpinningBlade::SetAnim(int anim) {
    if (!entity) return;
    entity->SetAnimation(anim);
}
void SpinningBlade::Update(void) {
    Enemy::Update();
    if (health <= .0f) {
        rb->SetVelocity(FVector2::Zero);
        if (entity) {
            entity->SetAnimation(idle);
            if (!entity->AnimFinished()) return;
        }
        if (explosionNode) {
            if (explosionNode->value->AnimFinished()) {
                Physics::UnsubStandaloneEnt(explosionNode);
                EnemySpawner::DestroyEnemy(enemySpawnNode);
                numExplosions--;
            }
            return;
        }
        if (numExplosions >= max_explosions) {
            EnemySpawner::DestroyEnemy(enemySpawnNode);
            return;
        }
        explosionNode = Physics::SubStandaloneEnt(Entity::MakeEntity("explosion/round_vortex", { "explosion" }, FVector2::Zero, explosionSize, true, GetPosition() + explosionSize * -.5f, Main::empty_cc_init, .0f));
        auto animator = explosionNode->value;
        constexpr int explosion_animation = 0;
        if (entity) {
            rb->DestroyEntity();
            entity = nullptr;
        }
        rb->ScaleNarrowPVert(explosionColliderScale);
        rb->SetLayer(Main::Layer::crystalLayer);
        rb->SetTrigger(true);
        damage = explosion_damage;
        animator->SetNotLoop(explosion_animation);
        animator->ResetAnim(explosion_animation);
        //as in the animation was designed to be at this many fps.
        constexpr float designed_for_fps = 30.f;
        animator->SetAnimSpd(designed_for_fps / Animator::default_anim_fps);
        numExplosions++;
        return;
    }
    if (curAnim == hurt && !animFinished) return;
    if (plrDistSqr < attackDistSqr) {
    attack:
        SetAnim(attack);
        goto ret;
    }
    if (touchingEnemy) {
        goto attack;
    }
    SetAnim(idle);
    Move((toPlr * toPlrScale + toPlr.Perpendicular() * toPlrPerpScale).Normalized());
ret:
    touchingEnemy = false;
    if (!entity) return;
    entity->SetFlip(toPlr.x < .0f);
}