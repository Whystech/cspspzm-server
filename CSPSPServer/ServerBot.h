#ifndef _SERVERBOT_H_
#define _SERVERBOT_H_

#include "Person.h"
#include "AStar.h"

class Grid;

class ServerBot : public Person
{
private:
	std::vector<Person*>* mPeople;
	Grid* mGrid;
	AStar* mAStar;
	int* mGameType;
	float* mFlagX;
	float* mFlagY;
	Person* mTarget;
	std::vector<Node*> mPath;
	unsigned int mPathIndex;
	float mDecisionTimer;
	float mPathTimer;
	float mInputTime;

	Person* FindTarget();
	bool IsEnemy(Person* person) const;
	void RebuildPath(float targetX, float targetY);

public:
	static int mDecisionMin;
	static int mDecisionMax;
	static float mPathInterval;
	static float mWaypointTolerance;
	static float mStopDistance;
	static float mKnifeDistance;
	static float mGunDistance;
	ServerBot(std::vector<GunObject*>* gunobjects, std::vector<Bullet*>* bullets,
		UdpManager* udpmanager, char* name, std::vector<Person*>* people,
		Grid* grid, AStar* astar, int* gameType, float* flagX, float* flagY);
	virtual void Update(float dt);
	virtual bool IsBot() const { return true; }
};

#endif