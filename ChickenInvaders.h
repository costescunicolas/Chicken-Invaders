#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>


#include "lab_m1/Tema1/ShipEditor.h"

struct Chicken {
	glm::vec2 position;
	bool alive;
};

struct Bullet {
	glm::vec2 position;
	bool active;
};

struct Egg {
	glm::vec2 position;
	bool active;
};

class ChickenInvaders {
public:
	ChickenInvaders();
	ChickenInvaders(const std::vector<BlockInfo>& blocks, float scale);
	~ChickenInvaders();

	glm::vec2 fromPixelToWorld(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight);
    glm::ivec2 fromPixelToGridPos(glm::ivec2 pixel, int resX, int resY, float viewWidth, float viewHeight);
    glm::ivec2 fromGridPosToPixel(glm::ivec2 pos);


	void moveShip(float deltaTime, int key, float viewWidth, float viewHeight);
	void shoot();
	void updateBullets(float deltaTime, float viewHeight);
	void updateChickens(float deltaTime);
	void shootEgg(float deltaTime);
	void updateEggs(float deltaTime);
	void resetChickens(float updateTime);

	float getMinPosX();
	float getMaxPosX();
	float getMinPoxY();
	float getMaxPosY();

	bool chickensKilled();
	bool noLives();
	bool chickensAtBottom();
	bool gameEnded();

	std::vector<BlockInfo> shipBlocks;
	glm::vec2 shipCenter;
	glm::vec2 shipPosition;



	std::vector<Chicken> chickens;
	std::vector<Bullet> bullets;
	std::vector<Egg> eggs;

	float eggTimer;
	float renderChickensTimer;

	float scaleFactor;
	float elapsedTime;
	float moveSpeedYChicken;
	float amplitude;
	float frequency;
	int maxLives;
	int currentLives;
	int score;
	int round;
	int motors;
	float speedEggs;
	float timeBetweenEggs;
};
