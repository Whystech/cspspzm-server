#pragma once

#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <errno.h>
#include <math.h>
#include <sstream>

#ifdef _WIN32
#include <winsock.h>

#define socklen_t int
#else
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "Packet.h"
#include "Person.h"
#include "Vector2D.h"
#include "TileMap.h"
#include "UdpManager.h"
#include "Collision.h"
#include "Bullet.h"
#include "HttpManager.h"
#include "Grid.h"
#include "AStar.h"
#include "ServerBot.h"

#define VERSION 1.51f
#define NETVERSION 16

#define NONE -1
#define T 0
#define CT 1
#define TIE 2
#define ON 0
#define OFF 1

#define FREEZETIME 0
#define STARTED 1

#define MAXFILESIZE 400

class UdpManager;

struct MapInfo {
	char name[256];
	int type;
};

class GameServer
{
public:
	// Callback functions invoked when mPeople or mBannedPeople is updated, respectively
	std::function<void()> mOnPlayerListUpdate;
	std::function<void()> mOnBanListUpdate;

	bool mHasError;
	//string mOutput;
	std::stringstream mOutStream;

#ifdef _WIN32
	WSADATA wsaData;	// Windows socket
#endif
	int sock, length;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buffer[4096];

	//char mHTTPBuffer[4096];

	HttpManager* mHttpManager;

	//int websock;
	//struct sockaddr_in webserver;
	std::string ipaddress;
	int mPort;
	
	std::string mInput;
	float mCursorTimer;

	std::vector<MapInfo> mMapCycle;
	int mMapIndex;
	float mMapTime;
	float mMapTimer;
	int mMapTextSize;
	int mMapImageSize;
	int mMapOverviewSize;
	FILE *mMapTextFile;
	FILE *mMapImageFile;
	FILE *mMapOverviewFile;

	int mGameType;

	int mRoundFreezeTime;
	int mRoundTime;
	int mRoundEndTime;
	int mBuyTime;
	int mRespawnTime;
	int mInfectionRespawnDelay;
	int mOnlineInactivityTimeout;
	int mHordeWave;
	int mHordeWaveDelay;
	int mHordeWaveReward;
	int mHordeRegroupStyle;
	bool mHordeReviveSurvivors;
	float mHordeHealthMultiplierPerWave;
	float mHordeDamageMultiplierPerWave;
	float mHordeSpeedMultiplierPerWave;
	float mHordeHealthMultiplier;
	float mHordeDamageMultiplier;
	float mHordeSpeedMultiplier;

	int mSpawnGunIndex;
	int mInvincibleTime;

	float mLastRoundTime;

	float mRoundTimer;
	float mRoundEndTimer;
	float mBuyTimer;
	float mHordeSurvivalTime;
	int mRoundState;

	int mNumRounds;
	int mNumCTWins;
	int mNumTWins;

	int mNumFlags[2];
	float mFlagX[2];
	float mFlagY[2];
	bool mIsFlagHome[2];
	Person* mFlagHolder[2];

	int mRoundBit;

	int mNumGuns;

	UdpManager* mUdpManager;

	std::vector<Person*> mPeople;
	std::vector<GunObject*> mGunObjects;
	std::vector<Bullet*> mBullets;

	//Person* mPlayer;
	TileMap* mMap;
	Grid* mGrid;
	AStar* mAStar;
	Gun mGuns[MAX_GUNS];

	int mNumCTs;
	int mNumTs;

	int mNumRemainingCTs;
	int mNumRemainingTs;

	int mNumPlayers;
	int mWinner;

	float mTime;

	int mPlayerCounter;
	int mGunCounter;
	int mBulletCounter;
	int mChatCounter;
	int ackcounter;

	float mPingTimer;

	bool mUpdating;
	float mUpdateTimer;

	float mSendMovementTimer;

	char* mName;
	char mMapName[32];
	int mNumMaxPlayers;
	bool mBotsEnabled;
	int mBotCount;
	int mInfectedBotCount;
	int mCTBotCount;
	int mFriendlyFire;
	int mAutoBalance;
	int mAllTalk;
	bool mAllowRemoteAdmin;

	float mTimeMultiplier;

	std::vector<char*> mBannedPeople;
	std::vector<char*> mAdmins;
	std::vector<std::string> mBotNames;

	GameServer();
	~GameServer();
	void Init();

	void Update(float dt);
	void CheckCollisions();
	void CheckPlayerCollisions(Person* player);
	void HandlePacket(Packet &packet, Connection* connection, sockaddr_in from, bool sendack = true);
	void ResetRound(bool fullreset = false);
	void ResetHordeWave();
	void AdvanceHordeScaling();
	int GetHordeMeleeDamage(Person* attacker) const;
	void RespawnPlayer(Person* player, int x, int y);
	void UpdateScores(Person* attacker, Person* victim, Gun* weapon);

	void CleanUp();
	int Register();

	char* GetConfig(const char *location, char searchstr[], int length = 32);
	int LoadServerConfigInt(const char* file, char* key, int defaultValue, int minimum, int maximum);
	float LoadServerConfigFloat(const char* file, char* key, float defaultValue, float minimum, float maximum);

	Person* GetPerson(int id);
	int GetPlayerId();
	int GetGunId();

	void Buy(Person* player, int index);
	void Explode(Grenade* grenade);
	void HandleInput(char* input, bool remote = false);
	bool LoadMap(char* mapname, int maptype);
	void RemovePerson(Person* player);
	void AddConfiguredBots();
	void AddBot(char* name, int team);
	void EquipMeleeWeapon(Person* player);
	void GiveBotLoadout(Person* player);
	void SendNewPlayer(Person* player);
	void SendPickupState(MapPickup& pickup, int playerId = -1, Connection* target = NULL);
	void SendHordeState();
	void SendAmmoUpdate(Person* player, int slot);
	void ConfigureModeTeams();

	std::string GetPersonName(Person* player);
	//bool ReadHTTP(char* string);

	char* DecodeText(char* buffer, char* text);

	void Kick(char* name);
	void Ban(char* name);
	void Unban(char* name);

	void Hash();
};
