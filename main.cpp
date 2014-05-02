#include "Omega2D-PSP.h"
#include <unistd.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

const int frameTime = 10; // fixed frame time in ms

// SDL_Surface *playerShip = NULL;
// SDL_Surface *background = NULL;
// SDL_Surface *screen = NULL;

extern "C"

int main(int argc, char *argv[])
{
	gameStart: int gameLoop = 1;

	Omega2D::InitGraphics(480, 272, 16);
	Omega2D::InitInput();

	SDL_Surface *playerShip = Omega2D::LoadImage("playerShip.png");
	TTF_Font *hudFont = Omega2D::LoadFont("kenpixel_high_square.ttf", 12);
	SDL_Color whiteColor = {255, 255, 255, 0};
	// SDL_Surface *spaceBackground = Omega2D::LoadImage("spaceBackground.png");
	SDL_Surface *playerLaser = Omega2D::LoadImage("playerLaser.png");
	SDL_Surface *enemyOne = Omega2D::LoadImage("enemyOne.png");
	SDL_Surface *enemyExplosion = Omega2D::LoadImage("enemyExplosion.png");
	SDL_Surface *tiledBackground = Omega2D::LoadImage("tiledBackground.png");

	Omega2D::Vector2 playerPos(100.0f, 100.0f);
	Omega2D::Vector2 playerVelocity(0.0f, 0.0f);
	Omega2D::Vector2 maxVelocity(2.0f, 2.0f);

	std::vector<Omega2D::Vector2> playerBullets;

	Omega2D::Vector2 testBullet(200.0f, 100.0f);
	playerBullets.push_back(testBullet);

	float movementSpeed = 0.2f;
	int frameTime = 20;

	float laserSpeed = 0.5f;

	unsigned int frameStartTime = SDL_GetTicks();
	unsigned int frameEndTime = SDL_GetTicks();
	unsigned int currentFrameTime = 0;

	char frameDelayString [50];

	int laserAutoDelay = 300; // laser fire delay in ms
	int lastLaserFireTime = SDL_GetTicks(); // also in ms

	int fpsMeterInterval = 500; // fps delay in ms
	int lastFpsDraw = SDL_GetTicks();

	std::vector<int> bulletsToRemove;

	int enemySpawnInterval = 200; // in ms
	int lastEnemySpawn = SDL_GetTicks();

	int enemySpeed = 4.0f;

	std::vector<Omega2D::Vector2> enemies;
	std::map<Omega2D::Vector2*, int> enemyHealth;

	std::vector<Omega2D::Vector2> bgTiles;
	bgTiles.push_back(Omega2D::Vector2(0, 0));
	bgTiles.push_back(Omega2D::Vector2(272, 0));

	float tileSpeed = 0.03f;

	for (int i = 0; i < bgTiles.size(); i++)
	{
		bgTiles.at(i).x = i * 272.0f;
		bgTiles.at(i).y = 0.0f;
	}

	while (gameLoop)
	{		
		currentFrameTime = frameEndTime - frameStartTime;

		frameStartTime = SDL_GetTicks();		

		if (SDL_GetTicks() > lastFpsDraw + fpsMeterInterval)
		{
			lastFpsDraw = SDL_GetTicks();
			sprintf(frameDelayString, "FPS: %3d (%d ms)", 1000 / currentFrameTime, currentFrameTime);
		}

		Omega2D::ClearScreen(0x000000);
		// Omega2D::DrawImage(0, 0, spaceBackground);
		// Omega2D::DrawImage(0, 0, tiledBackground);
		// Omega2D::DrawImage(272, 0, tiledBackground);

		if (bgTiles.back().x <= 272.0f)
		{
			bgTiles.push_back(Omega2D::Vector2(bgTiles.back().x + 272.0f, 0.0f));
			
			if (bgTiles.size() > 3) bgTiles.erase(bgTiles.begin());
		}

		for (int i = 0; i < bgTiles.size(); i++)
		{
			Omega2D::DrawImage(bgTiles.at(i).x, bgTiles.at(i).y, tiledBackground);
			bgTiles.at(i).x -= tileSpeed * 15;
		}

		for (int i = 0; i < bgTiles.size(); i++)
		{
			char debugString [50];
			sprintf(debugString, "Drawing background tile at %.2f, %.2f", bgTiles.at(i).x, bgTiles.at(i).y);

			Omega2D::DrawText(hudFont, debugString, whiteColor, 10, 35 + i * 15);			
		}

		Omega2D::DrawText(hudFont, frameDelayString, whiteColor, 10, 10);

		//////////////
		// Player logic/input
		//////////////

		playerPos.x += playerVelocity.x * currentFrameTime / 10;
		playerPos.y += playerVelocity.y * currentFrameTime / 10;


		if (Omega2D::GetButton(PSP_CTRL_UP))
		{
			if (playerVelocity.y - movementSpeed >= -maxVelocity.y)
				playerVelocity.y -= movementSpeed;
		}
		else if (Omega2D::GetButton(PSP_CTRL_DOWN))
		{
			if (playerVelocity.y + movementSpeed <= maxVelocity.y)
				playerVelocity.y += movementSpeed;	
		}
		else
		{
			playerVelocity.y = Omega2D::Lerp(playerVelocity.y, 0.0f, 0.1f);
		}

		if (Omega2D::GetButton(PSP_CTRL_RIGHT))
		{
			if (playerVelocity.x + movementSpeed <= maxVelocity.x)
				playerVelocity.x += movementSpeed;
		}
		else if (Omega2D::GetButton(PSP_CTRL_LEFT))
		{
			if (playerVelocity.x - movementSpeed >= -maxVelocity.x)
				playerVelocity.x -= movementSpeed;
		}
		else
		{
			playerVelocity.x = Omega2D::Lerp(playerVelocity.x, 0.0f, 0.2f);
		}

		// check if colliding with an enemy

		for (int i = 0; i < enemies.size(); i++)
		{
			if (Omega2D::CircleCollision
				(Omega2D::Vector2(playerPos.x + 25, playerPos.y + 37),
				 Omega2D::Vector2(enemies.at(i).x + 25, enemies.at(i).y + 30),
				 25, 25))
			{
				// collided with enemy!
				SDL_Delay(3000);
				return 0;			}
		}

		// playerVelocity.x = Omega2D::Clamp(playerVelocity.x, -maxVelocity.x, maxVelocity.x);
		// playerVelocity.y = Omega2D::Clamp(playerVelocity.y, -maxVelocity.y, maxVelocity.y);

		playerPos.x = Omega2D::Clamp(playerPos.x, 0.0f, 430.0f);
		playerPos.y = Omega2D::Clamp(playerPos.y, 0.0f, 197.0f);

		// Omega2D::DrawCircle(Omega2D::Vector2(0, 0), 50);
		// Omega2D::DrawCircle(Omega2D::Vector2(playerPos.x + 25, playerPos.y + 37), 25);

		// if (Omega2D::CircleCollision(playerPos, Omega2D::Vector2(0, 0), 25, 50))
		// {			
		// 	Omega2D::DrawText(hudFont, "Player is colliding with 0, 0!", whiteColor, 10, 25);
		// }

		////////////////
		// Player bullet logic
		////////////////

		// create new bullet on button press
		if (Omega2D::GetButton(PSP_CTRL_CROSS))
		{
			if (SDL_GetTicks() > lastLaserFireTime + laserAutoDelay)
			{
				lastLaserFireTime = SDL_GetTicks();

				Omega2D::Vector2 newBullet(playerPos.x + 37, playerPos.y + 21);
				playerBullets.push_back(newBullet);

				Omega2D::Vector2 newBullet2(playerPos.x + 37, playerPos.y + 45);
				playerBullets.push_back(newBullet2);
			}
		}

		for (int i = 0; i < playerBullets.size(); i++)
		{
			playerBullets.at(i).x += laserSpeed * currentFrameTime; // move laser equally each frame
		}

		// limit number of bullets to render
		while (playerBullets.size() > 12)
		{
			playerBullets.erase(playerBullets.begin());
		}

		std::vector<int> enemyIndexesToRemove;
		std::vector<int> laserIndexesToRemove;

		for (int i = 0; i < playerBullets.size(); i++)
		{
			bool collisionCheck = false;

			for (int j = 0; j < enemies.size(); j++)
			{
				if (playerBullets.at(i).x < 480 && enemies.at(j).x < 480)
				{
					collisionCheck = Omega2D::CircleCollision(
						Omega2D::Vector2(playerBullets.at(i).x + 18, playerBullets.at(i).y + 4),
						Omega2D::Vector2(enemies.at(j).x + 25, enemies.at(j).y + 30),
						18, 25);

					if (collisionCheck)
					{
						if (enemyHealth[&enemies.at(j)] <= 0)
						{
							// Omega2D::DrawText(hudFont, "Bullet colliding with enemy!", whiteColor, 10, 25);

							playerBullets.erase(playerBullets.begin() + i);
							
							Omega2D::DrawImage(enemies.at(j).x, enemies.at(j).y + 5, enemyExplosion);

							// SDL_Delay(500); // sleep when enemy is hit

							enemies.erase(enemies.begin() + j);

							break;
						}
						else
						{							
							enemyHealth[&enemies.at(j)] -= 1;
						}
					}
				}
			}

			if (collisionCheck) break;
		}

		// for (int i = 0; i < enemyIndexesToRemove.size(); i++)
		// {
		// 	enemies.erase(enemies.begin() + enemyIndexesToRemove.at(i));
		// }
		// enemyIndexesToRemove.clear();

		// for (int i = 0; i < laserIndexesToRemove.size(); i++)
		// {
		// 	playerBullets.erase(playerBullets.begin() + laserIndexesToRemove.at(i));
		// }
		// laserIndexesToRemove.clear();

		////////////
		// Enemy spawning / logic
		////////////

		if (SDL_GetTicks() > lastEnemySpawn + enemySpawnInterval)
		{
			lastEnemySpawn = SDL_GetTicks();

			// spawn an enemy
			Omega2D::Vector2 newEnemy(480, Omega2D::RandomInt(50, 150));
			enemies.push_back(newEnemy);
			enemyHealth.insert(std::make_pair(&newEnemy, 2));
		}

		for(int i = 0; i < enemies.size(); i++)
		{
			enemies.at(i).x -= enemySpeed * currentFrameTime / 10; // move enemy to the left each frame
		}


		// limit enemy vector to 5 enemies
		while (enemies.size() > 10)
		{
			enemies.erase(enemies.begin());
		}

		////////////
		// Drawing code
		////////////

		for (int i = 0; i < playerBullets.size(); i++)
		{
			Omega2D::Vector2 currentBulletVector = playerBullets.at(i);
			Omega2D::DrawImage(currentBulletVector.x, currentBulletVector.y, playerLaser);
		}		

		for (int i = 0; i < enemies.size(); i++)
		{
			Omega2D::Vector2 currentEnemyPos = enemies.at(i);
			Omega2D::DrawImage(currentEnemyPos.x, currentEnemyPos.y, enemyOne);
		}

		Omega2D::DrawImage(playerPos.x, playerPos.y, playerShip);

		// for(int i = 0; i < enemies.size(); i++)
		// {
		// 	Omega2D::DrawCircle(Omega2D::Vector2(enemies.at(i).x + 25, enemies.at(i).y + 30), 25);
		// }

		// for (int i = 0; i < playerBullets.size(); i++)
		// {
		// 	Omega2D::DrawCircle(Omega2D::Vector2(playerBullets.at(i).x + 18, playerBullets.at(i).y + 4), 18);
		// }

		if (currentFrameTime < 16)
			frameTime = 16 - currentFrameTime;
		else
			frameTime = 0;

		Omega2D::UpdateScreen(frameTime, false);

		frameEndTime = SDL_GetTicks();

		//////////////
		// End-of-frame input code
		//////////////

		// xPressedLastFrame = Omega2D::GetButton(PSP_CTRL_CROSS);
	}

	SDL_FreeSurface(playerShip);
	// SDL_FreeSurface(background);

	Omega2D::Quit();

	return 0;
}