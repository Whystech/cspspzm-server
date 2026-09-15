#pragma once

#include <vector>
#include <math.h>

#define MAX_GUNS 128
#define ZOMBIECLAWS 66
#define FIREMODE_SEMI 0
#define FIREMODE_AUTO 1
#define MAX_PELLETS 16
#define SCOPE_NONE 0
#define SCOPE_LOW 1
#define SCOPE_MEDIUM 2
#define SCOPE_HIGH 3
#define BUY_CATEGORY_NONE 0
#define BUY_CATEGORY_EQUIPMENT 6
#define BUY_TEAM_T 1
#define BUY_TEAM_CT 2
struct Gun {
	int mId;
	int mDelay;
	int mDamage;
	float mSpread;
	int mClip;
	int mNumClips;
	int mReloadDelay;
	float mSpeed;
	float mBulletSpeed;
	float mViewAngle;
	int mCost;
	int mType;
	int mFireMode;
	int mPellets;
	int mScope;
	int mBuyCategory;
	int mBuyTeams;
	char mName[15];
	int mMuzzleFlashType;
};
//------------------------------------------------------------------------------------------------

class GunObject
{
private:

protected:

public:
	int mId;

	float mX;
	float mY;
	float mOldX;
	float mOldY;
	float mSpeed;
	float mAngle;

	Gun *mGun;
	int mClipAmmo;
	int mRemainingAmmo;
	bool mOnGround;
	//bool mSpawned;

	GunObject(Gun *gun, int clipammo, int remainingammo);
	~GunObject();

	void Update(float dt);
	//void Render(float x, float y);

	void SetTotalRotation(float theta);
	void SetPosition(float x, float y);
	//void Reset();
};