#include "EnemySpawner.hpp"
#include "bob.hpp"
#include "bat.hpp"
#include "ghost.hpp"
#include "multicast delegates.hpp"
#include "Item.hpp"
#include "Crystal.hpp"
#include "types.hpp"
#include "spider.hpp"
int EnemySpawner::frameIndex;
int EnemySpawner::numSpawnedEnemies = 0;
bool EnemySpawner::lastFrameEndWave = false;
Node<Text*> *EnemySpawner::waveText = nullptr;
Timer* EnemySpawner::waveTextTimer = nullptr;
Node<Enemy*> *EnemySpawner::enemies = nullptr;
int EnemySpawner::waveIndex = 0;
int EnemySpawner::maxEnemies = 30;
int EnemySpawner::progressionIndex = -1;
Text::TextData EnemySpawner::textData;
float EnemySpawner::minPlrDist = FLT_MAX;
Enemy* EnemySpawner::closestEnemy;
const std::vector<int> EnemySpawner::enemyTypeProgression = { bob, bat, bob | bat, ghost, spider };
void EnemySpawner::DestroyWaveText(void) {
	delete waveTextTimer;
	waveTextTimer = nullptr;
	delete waveText->value;
	Physics::UnSubText(waveText);
}
Enemy *EnemySpawner::SpawnEnemy(int type, FVector2 position) {
	Enemy* returnVal
#ifdef DEBUG_BUILD
		= nullptr
#endif
		;
	switch (type) {
	case bob:
		returnVal = new SwordGuy();
		break;
	case bat:
		returnVal = new Bat();
		break;
	case ghost:
		returnVal = new Ghost();
		break;
	case spider:
		returnVal = new Spider();
		break;
	default:
		ThrowError("unsupported enemy type");
	}
#ifdef DEBUG_BUILD
	if (!returnVal) {
		ThrowError("return val shouldnt' be null");
		return nullptr;
	}
#endif
	if (position != FVector2::Infinity) returnVal->rb->SetPosition(position);
	return returnVal;
}
Enemy* EnemySpawner::AddEnemy(EnemyType enemyType) {
	auto guy = SpawnEnemy(enemyType);
	guy->enemySpawnNode = Node<Enemy*>::AddAtHeadByVal(guy, &enemies);
	numSpawnedEnemies++;
	guy->enemyType = enemyType;
	return guy;
}
void EnemySpawner::Update(void) {
	minPlrDist = FLT_MAX;
	closestEnemy = nullptr;
	if (waveTextTimer && waveTextTimer->GetElapsedSeconds() > waveTextDisappearTime) DestroyWaveText();
	auto bIsEndWave = Enemy::GetNumEnemies() == 0;
	if (bIsEndWave && !lastFrameEndWave) {
		maxEnemies *= wave_num_enemy_mult;
		progressionIndex = (progressionIndex + 1) % enemyTypeProgression.size();
		numSpawnedEnemies = 0;
		if (waveTextTimer) DestroyWaveText();
		textData.SetText("Wave " + std::to_string(++waveIndex));
		waveText = Physics::SubText(new Text(&textData));
		waveTextTimer = new Timer();
	}
	lastFrameEndWave = bIsEndWave;
	if (frameIndex++ % spawnFrameInterval || numSpawnedEnemies == maxEnemies) return;
	int bitIndex = 1;
	while (bitIndex != 0 && numSpawnedEnemies != maxEnemies) {
		if (enemyTypeProgression[progressionIndex] & bitIndex) {
			auto guy = SpawnEnemy(bitIndex);
			guy->enemySpawnNode = Node<Enemy*>::AddAtHeadByVal(guy, &enemies);
			numSpawnedEnemies++;
		}
		bitIndex <<= 1;
	}
}
void EnemySpawner::DestroyEnemy(Node<Enemy*> *guy) {
	auto c = new Crystal(guy->value->GetPosition());
 	delete guy->value;
	Node<Enemy*>::Remove(&enemies, guy);
}
void EnemySpawner::Init(void) {
	textData = Text::TextData(waveTextSizeVec, Main::halfDisplaySizeI + waveTextOffset, "broken");
	Main::EarlyUpdates += EnemySpawner::Update;
}