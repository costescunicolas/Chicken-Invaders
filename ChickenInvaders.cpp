#include "lab_m1/Tema1/ChickenInvaders.h"

#define BLOCK_SQUARE_SIDE 50
#define ROWS 2
#define COLS 9
#define SPACING_X 130.0f
#define SPACING_Y 100.0f
#define START_X 120.0f
#define START_Y 630.0f
#define SPEED 400.0f

ChickenInvaders::ChickenInvaders()
{
}

ChickenInvaders::ChickenInvaders(const std::vector<BlockInfo>& blocks, float scale)
{	
	
	shipBlocks = blocks;
	scaleFactor = scale;

	eggTimer = 0.0f;
	renderChickensTimer = 0.0f;

	elapsedTime = 0.0f;
	moveSpeedYChicken = 5.0f;     
	amplitude = 60.0f;      
	frequency = 2.0f; 

	maxLives = 3;
	currentLives = 3;
	score = 0;
	round = 1;
	
	motors = 0;
	speedEggs = 200.0f;
	timeBetweenEggs = 3.0f;
	

	if (!shipBlocks.empty()) {
		float minX = FLT_MAX, maxX = FLT_MIN;
		float minY = FLT_MAX, maxY = FLT_MIN;

		for (const auto& block : shipBlocks) {
			minX = std::min(minX, block.position.x);
            maxX = std::max(maxX, block.position.x + BLOCK_SQUARE_SIDE);
            minY = std::min(minY, block.position.y);
            maxY = std::max(maxY, block.position.y + BLOCK_SQUARE_SIDE);
		}
		shipCenter = glm::vec2((minX + maxX) / 2.0f, (minY + maxY) / 2.0f);
	} else {
		shipCenter = glm::vec2(0);
	}

	for (auto& block : shipBlocks) {
		block.position = block.position * scaleFactor - shipCenter * scaleFactor;
		block.position += glm::vec2((1280.0f / 2), 100);
		if (block.meshName == "motor") {
			motors++;
		}
	}

	// --- Initialize chickens (2 rows of 10) ---
	int rows = ROWS;
	int cols = COLS;
	float spacingX = SPACING_X;  // distanța între pui pe orizontală
	float spacingY = SPACING_Y;  // distanța între linii
	float startX = START_X;   // offset inițial pe axa X
	float startY = START_Y;   // poziție inițială sus pe ecran

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			Chicken c;
			c.alive = true;
			c.position = glm::vec2(startX + j * spacingX, startY - i * spacingY);
			chickens.push_back(c);
		}
	}

}

ChickenInvaders::~ChickenInvaders()
{
}

float ChickenInvaders::getMinPosX()
{	
	float Min = FLT_MAX;
	for (const auto& block : shipBlocks) {
		Min = std::min(Min, block.position.x);
	}
	return Min;
}

float ChickenInvaders::getMaxPosX()
{
	float Max = FLT_MIN;
	for (const auto& block : shipBlocks) {
		Max = std::max(Max, block.position.x);
	}
	return Max;	
}

float ChickenInvaders::getMaxPosY()
{
	float Max = FLT_MIN;
	for (const auto& block : shipBlocks) {
		Max = std::max(Max, block.position.y);
	}
	return Max;		
}

float ChickenInvaders::getMinPoxY()
{
	float Min = FLT_MAX;
	for (const auto& block : shipBlocks) {
		Min = std::min(Min, block.position.y);
	}
	return Min;
}

bool ChickenInvaders::chickensKilled()
{
	return chickens.size() == 0;
}

bool ChickenInvaders::noLives()
{
	return currentLives == 0;
}

bool ChickenInvaders::chickensAtBottom()
{
	for (const auto& chicken : chickens) {
		if (chicken.position.y <= 0) {
			return true;
		}
	}
	return false;
}

bool ChickenInvaders::gameEnded()
{
	return noLives() || chickensAtBottom();
}

void ChickenInvaders::shoot()
{
	for (const auto& block : shipBlocks) {
		if (block.meshName == "tun") {
			Bullet b;
			b.position = block.position + glm::vec2(BLOCK_SQUARE_SIDE * scaleFactor / 2.0f, 3 * BLOCK_SQUARE_SIDE * scaleFactor);
			b.active = true;
			bullets.push_back(b);
		}
	}
}



void ChickenInvaders::shootEgg(float deltaTime)
{	
	if (chickens.size() > 0) {
		eggTimer += deltaTime;
		int chickenIndex = rand() % chickens.size();
		if (chickens[chickenIndex].alive && eggTimer > timeBetweenEggs) {
			Egg e;
			e.position = chickens[chickenIndex].position - glm::vec2(0, BLOCK_SQUARE_SIDE * scaleFactor);
			e.active = true;
			eggs.push_back(e);
			eggTimer = 0.0f;
		}
	}
	
}

void ChickenInvaders::updateBullets(float deltaTime, float viewHeight) 
{
	float speed = SPEED;
	float bulletRadius = (BLOCK_SQUARE_SIDE * scaleFactor) / 2.0f;
	float chickenRadius = 0.5 * BLOCK_SQUARE_SIDE * scaleFactor;
	float eggRadius = (BLOCK_SQUARE_SIDE * scaleFactor) / 2.0f;
	for (auto& bullet : bullets) {
		if (!bullet.active) continue;

		bullet.position.y += speed * deltaTime;
		if (bullet.position.y > viewHeight - BLOCK_SQUARE_SIDE * scaleFactor / 2.0f) {
			bullet.active = false;
		} else {
			bool bulletDeleted = false;
			for (auto& chicken : chickens) {
				if (!chicken.alive) continue;

				float distChicken = glm::distance(bullet.position, chicken.position);
				if (distChicken < bulletRadius + chickenRadius) {
					chicken.alive = false;
					bullet.active = false;
					bulletDeleted = true;
					score++;
					chickens.erase(
						std::remove_if(chickens.begin(), chickens.end(), 
										[](const Chicken& c) { return !c.alive; }),
						chickens.end()
					);
					break;
				}
			}
			if (!bulletDeleted) {
				for (auto& egg : eggs) {
					if (!egg.active) continue;
					float distEgg = glm::distance(bullet.position, egg.position);

					if (distEgg < bulletRadius + eggRadius) {
						bullet.active = false;
						egg.active = false;
						eggs.erase(
							std::remove_if(eggs.begin(), eggs.end(),
										[](const Egg& e) { return !e.active; }),
							eggs.end()
						);
					}
				}
			}
		}
	}
	bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
                       [](const Bullet& b) { return !b.active; }),
        bullets.end()
    );
}

void ChickenInvaders::resetChickens(float updateTime) 
{
	
	if (chickensKilled()) {
		// --- Initialize chickens (2 rows of 10) ---
		renderChickensTimer += updateTime;
		if (renderChickensTimer > 5.0f) {
			int rows = ROWS;
			int cols = COLS;
			float spacingX = SPACING_X;  // distanța între pui pe orizontală
			float spacingY = SPACING_Y;  // distanța între linii
			float startX = START_X;   // offset inițial pe axa X
			float startY = START_Y;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < cols; j++) {
					Chicken c;
					c.alive = true;
					c.position = glm::vec2(startX + j * spacingX, startY - i * spacingY);
					chickens.push_back(c);
				}
			}
			round++;
			speedEggs += 50.0f;
			if (timeBetweenEggs > 1.0f) timeBetweenEggs -= 0.5f;
			moveSpeedYChicken += 5.0f;
			frequency += 0.5f;
			renderChickensTimer = 0.0f;
		}	
		
	}
}

void ChickenInvaders::updateEggs(float deltaTime)
{

	shootEgg(deltaTime);
	for (auto& egg : eggs) {
		if (!egg.active) continue;

		egg.position.y -= speedEggs * deltaTime;
		if (egg.position.y < -BLOCK_SQUARE_SIDE * scaleFactor / 2.0f) {
			egg.active = false;
		} else {
			for (auto& block : shipBlocks) {
				int levels = block.meshName == "tun" ? 3 : 1;
				if (egg.position.x - 0.7 * BLOCK_SQUARE_SIDE * scaleFactor / 2.0f <= block.position.x + BLOCK_SQUARE_SIDE * scaleFactor 
					&& egg.position.x + 0.7 * BLOCK_SQUARE_SIDE * scaleFactor / 2.0f >= block.position.x - BLOCK_SQUARE_SIDE * scaleFactor
					&& egg.position.y - BLOCK_SQUARE_SIDE * scaleFactor / 2.0f <= block.position.y + levels * BLOCK_SQUARE_SIDE * scaleFactor
					&& egg.position.y - BLOCK_SQUARE_SIDE * scaleFactor / 2.0f >= block.position.y) {
					
					egg.active = false;
					if (currentLives > 0) {
						currentLives--;
					}
				}
			}
		}
	}
	eggs.erase(
		std::remove_if(eggs.begin(), eggs.end(),
					[](const Egg& e) { return !e.active; }),
		eggs.end()
    );
}


void ChickenInvaders::updateChickens(float deltaTime) 
{
	elapsedTime += deltaTime;

	float offsetX = amplitude * sin(frequency * elapsedTime) * deltaTime;
	float offsetY = -moveSpeedYChicken * deltaTime;

	for (auto& c : chickens) {
		if (!c.alive) continue;

		c.position.x += offsetX;
		c.position.y += offsetY;
	}
}



void ChickenInvaders::moveShip(float deltaTime, int key, float viewWidth, float viewHeight)
{
    float speed = 100.0f * motors; 
    float leftBound = 0.0f;
    float rightBound = viewWidth - BLOCK_SQUARE_SIDE * scaleFactor;
	float lowerBound = 0.0f;
	float upperBound = viewHeight / 2 - BLOCK_SQUARE_SIDE * scaleFactor;
    float minX = getMinPosX(); 
    float maxX = getMaxPosX();
	float minY = getMinPoxY();
	float maxY = getMaxPosY(); 

	std::cout << minX << " " << maxX << std::endl;

    if (key == GLFW_KEY_LEFT) {
        if (minX > leftBound) { 
            for (auto& block : shipBlocks) {
                block.position.x -= speed * deltaTime;
            }
        }
    }


    else if (key == GLFW_KEY_RIGHT) {
        if (maxX < rightBound) { 
            for (auto& block : shipBlocks) {
                block.position.x += speed * deltaTime;
            }
        }
    } else if (key == GLFW_KEY_UP) {
		if (maxY < upperBound) {
			for (auto& block : shipBlocks) {
				block.position.y += speed * deltaTime;
			}
		}
	} else if (key == GLFW_KEY_DOWN) {
		if (minY > lowerBound) {
			for (auto& block : shipBlocks) {
				block.position.y -= speed * deltaTime;
			}
		}
	}
}





