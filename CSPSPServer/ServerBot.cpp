#include "ServerBot.h"
#include "Grid.h"
#include "TileMap.h"
#include <cmath>
#include <cstdlib>
#include <limits>

int ServerBot::mDecisionMin = 250;
int ServerBot::mDecisionMax = 450;
float ServerBot::mPathInterval = 600.0f;
float ServerBot::mWaypointTolerance = 32.0f;
float ServerBot::mStopDistance = 150.0f;
float ServerBot::mKnifeDistance = 55.0f;
float ServerBot::mGunDistance = 700.0f;

ServerBot::ServerBot(std::vector<GunObject*>* gunobjects, std::vector<Bullet*>* bullets,
	UdpManager* udpmanager, char* name, std::vector<Person*>* people,
	Grid* grid, AStar* astar, int* gameType, float* flagX, float* flagY)
	: Person(gunobjects,bullets,udpmanager,name), mPeople(people), mGrid(grid),
	  mAStar(astar), mGameType(gameType), mFlagX(flagX), mFlagY(flagY),
	  mTarget(NULL), mPathIndex(0),
	  mDecisionTimer(0.0f), mPathTimer(0.0f), mInputTime(0.0f)
{
	mMovementStyle = ABSOLUTE1;
}

bool ServerBot::IsEnemy(Person* person) const
{
	if (person == NULL || person == this || person->mState == DEAD || person->mTeam == NONE) return false;
	return *mGameType == FFA || person->mTeam != mTeam;
}

Person* ServerBot::FindTarget()
{
	Person* target = NULL;
	float closestDistance = (std::numeric_limits<float>::max)();
	for (unsigned int i=0; i<mPeople->size(); i++) {
		Person* person = (*mPeople)[i];
		if (!IsEnemy(person)) continue;
		float dx = person->mX-mX;
		float dy = person->mY-mY;
		float distance = dx*dx+dy*dy;
		if (distance < closestDistance) {
			target = person;
			closestDistance = distance;
		}
	}
	return target;
}

void ServerBot::RebuildPath(float targetX, float targetY)
{
	mPath.clear();
	mPathIndex = 0;
	Node* start = mAStar->GetClosestNode(this,mGrid);
	Node* end = mAStar->GetClosestNode(targetX,targetY,mGrid);
	mPath = mAStar->GetPath(start,end);
	if (mPath.size() > 1) mPathIndex = 1;
}

void ServerBot::Update(float dt)
{
	if (mState == DEAD || !mIsActive) {
		mIsFiring = false;
		Person::Update(dt);
		return;
	}

	mDecisionTimer -= dt;
	mPathTimer -= dt;
	if (mDecisionTimer <= 0.0f || !IsEnemy(mTarget)) {
		mTarget = FindTarget();
		mDecisionTimer = (float)mDecisionMin;
		if (mDecisionMax > mDecisionMin) mDecisionTimer += rand()%(mDecisionMax-mDecisionMin+1);
	}

	Input input = {0,0,mFacingAngle};
	bool visible = mTarget != NULL && mGrid->LineOfSight(mX,mY,mTarget->mX,mTarget->mY,true,true);
	bool hasDestination = mTarget != NULL;
	float targetX = mTarget == NULL ? mX : mTarget->mX;
	float targetY = mTarget == NULL ? mY : mTarget->mY;
	if (*mGameType == CTF && (mHasFlag || !visible)) {
		int flagTeam = mHasFlag ? mTeam : ((mTeam == CT) ? T : CT);
		targetX = mFlagX[flagTeam];
		targetY = mFlagY[flagTeam];
		hasDestination = true;
		visible = false;
	}

	if (hasDestination) {
		if (!visible) {
			if (mPathTimer <= 0.0f || mPath.empty()) {
				RebuildPath(targetX,targetY);
				mPathTimer = mPathInterval;
			}
			if (mPathIndex < mPath.size()) {
				targetX = (float)mPath[mPathIndex]->mX;
				targetY = (float)mPath[mPathIndex]->mY;
				float nodeX = targetX-mX;
				float nodeY = targetY-mY;
				if (nodeX*nodeX+nodeY*nodeY < mWaypointTolerance*mWaypointTolerance) mPathIndex++;
			}
		}

		float moveAngle = atan2f(targetY-mY,targetX-mX);
		float enemyAngle = mTarget == NULL ? moveAngle : atan2f(mTarget->mY-mY,mTarget->mX-mX);
		float enemyX = mTarget == NULL ? targetX-mX : mTarget->mX-mX;
		float enemyY = mTarget == NULL ? targetY-mY : mTarget->mY-mY;
		float enemyDistance = sqrtf(enemyX*enemyX+enemyY*enemyY);
		input.facingangle = enemyAngle;
		if (!visible || enemyDistance > mStopDistance) {
			input.x = (int)(cosf(moveAngle)*127.0f);
			input.y = (int)(sinf(moveAngle)*127.0f);
		}

		if (visible && !mHasFlag) {
			if (mGunIndex == KNIFE && mGuns[PRIMARY] != NULL) Switch(PRIMARY);
			GunObject* gun = GetCurrentGun();
			if (gun != NULL && gun->mGun->mType != KNIFE && gun->mClipAmmo == 0) Reload();
			mIsFiring = enemyDistance < ((mGunIndex == KNIFE) ? mKnifeDistance : mGunDistance);
		}
		else {
			mIsFiring = false;
		}
	}
	else {
		mIsFiring = false;
	}

	mInputTime += dt;
	ReceiveInput(input,mInputTime);
	Person::Update(dt);
}