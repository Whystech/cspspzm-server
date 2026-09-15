#ifndef _TILEMAP_H_
#define _TILEMAP_H_

#include "GunObject.h"
#include <vector>
#include "Vector2D.h"
#include "Collision.h"
#include "Node.h"

#define TEAM 0
#define FFA 1
#define CTF 2
#define EXTERMINATION 3
#define INFECTION 4
#define HORDE 5

struct CollisionPoint {
	float x;
	float y;
	bool bullets;
	bool people;
};

struct CollisionLine
{
	Line line;
	bool bullets;
	bool people;

	CollisionLine(Line _line): line(_line) {}
	~CollisionLine() {}
};

struct BuyZone {
	float x1;
	float y1;
	float x2;
	float y2;
};

#define PICKUP_HEALTH 0
#define PICKUP_ARMOR 1
#define PICKUP_AMMO 2

struct MapPickup {
	int type;
	int id;
	float x;
	float y;
	int amount;
	float respawnTime;
	float respawnTimer;
	bool active;
};

//------------------------------------------------------------------------------------------------
class TileMap
{
private:
	//std::vector<JQuad*> mTiles;
	std::vector<int> gMap;
	//JTexture *mTexture;
	bool loaded;

protected:

public:
	char mName[20];
	int mCols;
	int mRows;

	std::vector<CollisionLine> mCollisionLines;
	std::vector<CollisionPoint> mCollisionPoints;

	std::vector<Vector2D*> mCTSpawns;
	std::vector<Vector2D*> mTSpawns;
	std::vector<BuyZone> mCTBuyZones;
	std::vector<BuyZone> mTBuyZones;
	std::vector<Node*> mNodes;
	std::vector<MapPickup> mPickupSpawns;
	std::vector<MapPickup> mPickups;
	int mNumPoints;
	int mNumCTs;
	int mNumTs;
	std::vector<GunObject*>* mGunObjects;
	std::vector<GunObject*> mGunObjectsSpawn;
	Gun mGuns[MAX_GUNS];

	Vector2D mFlagSpawn[2];

	TileMap(Gun guns[], std::vector<GunObject*>* gunobjects);
	~TileMap();
	bool Load(char *mapFile, int &gameType);
	void Unload();
	void Update(float dt);
	//void Render(float x, float y);
	void Reset(int &guncounter);
};

#endif
