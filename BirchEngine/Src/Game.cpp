#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "ECS/Components.h"
#include "Vector2D.h"
#include "Collision.h"
#include "AssetManager.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <SDL_mixer.h>
#undef main


Map* map;
Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;

SDL_Rect Game::camera = { 0,0,800,640 };

AssetManager* Game::assets = new AssetManager(&manager);

bool Game::isRunning = false;
bool Game::isCarry = false; 
bool Game::cheat = false; 

auto& player(manager.addEntity());
auto& label(manager.addEntity());
auto& shadow(manager.addEntity());
auto& box1(manager.addEntity());
auto& door(manager.addEntity());
auto& doorOpen(manager.addEntity());
auto& doorFull(manager.addEntity());
auto& textTry(manager.addEntity());
auto& textQuit(manager.addEntity());
auto& hiddenText(manager.addEntity());


Game::Game()
{}

Game::~Game()
{}

void Game::init(const char* title, int width, int height, bool fullscreen)
{
	shaTimer = 0;
	int flags = 0;
	isRewinding = false;
	isOpenDoor = 5; 

	std::fstream f;
	f.open("input.txt", std::ios::in);
	f >> TRY;
	f.close();

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer)
		{
			SDL_SetRenderDrawColor(renderer, 45, 20, 45, 255);
		}
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		}

		gMusic = Mix_LoadMUS("assets/pk.wav");
		if (gMusic == NULL)
		{
			printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
		}
		isRunning = true;
	}
	


	if (TTF_Init() == -1)
	{
		std::cout << "Error : SDL_TTF" << std::endl;
	}

	assets->AddTexture("terrain", "assets/terrain_ss.png");
	assets->AddTexture("player", "assets/player.png");
	assets->AddTexture("projectile", "assets/proj.png");
	assets->AddTexture("idle", "assets/idle.png");
	assets->AddTexture("run0", "assets/run0.png");
	assets->AddTexture("run1", "assets/run1.png");
	assets->AddTexture("run2", "assets/run2.png");
	assets->AddTexture("run3", "assets/run3.png");
	assets->AddTexture("run4", "assets/run4.png");
	assets->AddTexture("run5", "assets/run5.png");
	assets->AddTexture("box", "assets/box.png");
	assets->AddTexture("key", "assets/key.png");
	assets->AddTexture("bullet", "assets/bullet.png");
	assets->AddTexture("door_closed", "assets/door_closed.png");
	assets->AddTexture("door_open", "assets/door_anim.png");
	assets->AddTexture("door_fully", "assets/door_fullyopen.png");

	assets->AddFont("arial", "assets/arial.ttf", 10);
	assets->AddFont("snig", "assets/snig-re.ttf", 24);

	map = new Map("terrain", 2, 16);
	//ecs implementation

	map->LoadMap("assets/map1.map", 40, 30);

	player.addComponent<TransformComponent>(100.0f, 100.0f, 16, 16, 3);
	player.addComponent<SpriteComponent>("player", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayers);

	door.addComponent<TransformComponent>(1050.f, 10.f, 32, 32, 3);
	door.addComponent<SpriteComponent>("door_closed", false);
	door.addComponent<ColliderComponent>("door");
	
	doorFull.addComponent<TransformComponent>(1050.f, 10.f, 32, 32, 3);
	doorFull.addComponent<SpriteComponent>("door_fully", false);
	doorFull.addComponent<ColliderComponent>("door");
	SDL_Color white = { 155, 155, 155, 255 };
	SDL_Color red = { 255,0,17,105 };


	label.addComponent<UILabel>(10, 10, "Test String", "snig", white);
	textQuit.addComponent<UILabel>(700, 600, "Press ESC to Quit", "arial", red);
	textTry.addComponent<UILabel>(700, 10, "", "snig", red);
	hiddenText.addComponent<UILabel>(400,300, "CHEAT MODE", "snig", red);
	

	shadow.addComponent<TransformComponent>(100.0f, 110.0f, 16, 16, 3);
	shadow.addComponent<SpriteComponent>("idle");
	
	box1.addComponent<TransformComponent>(100.0f, 150.f, 16, 16, 3);
	box1.addComponent<SpriteComponent>("box", false);
	box1.addComponent<ColliderComponent>("box");
	box1.addGroup(groupBoxs);

	
	assets->CreateKey(Vector2D(460.f, 500.f), "key");
	assets->CreateKey(Vector2D(100., 750.f), "key");
	assets->CreateKey(Vector2D(740, 400), "key"); 
	assets->CreateKey(Vector2D(610, 255), "key");
	assets->CreateKey(Vector2D(740, 255), "key");
	
	Mix_PlayMusic(gMusic, -1);
}

//float kickTimer = 0; 

auto& tiles(manager.getGroup(Game::groupMap));
auto& players(manager.getGroup(Game::groupPlayers));
auto& colliders(manager.getGroup(Game::groupColliders));
auto& boxs(manager.getGroup(Game::groupBoxs));
auto& keys(manager.getGroup(Game::groupKeys));
auto& bullets(manager.getGroup(Game::groupBullets));

void Game::handleEvents()
{

	SDL_PollEvent(&event);

	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

float kickTimer = 4; 
float bulletRes = 0; 
float stopTimer = 0;
float openAnim = 0; 

void Game::update()
{
	// Timer, respawn 

	carrying();

	bulletRes++;
	if (bulletRes > 20 && stopTimer < 40)
	{
		assets->CreateBullet(Vector2D(350.f, 450.f), Vector2D(0, 2), "bullet");
		assets->CreateBullet(Vector2D(35, 670), Vector2D(2, 0), "bullet");
		assets->CreateBullet(Vector2D(680, 500), Vector2D(2, 0), "bullet");
		assets->CreateBullet(Vector2D(850, 830), Vector2D(0, -2), "bullet");
		assets->CreateBullet(Vector2D(680, 650), Vector2D(2, 0), "bullet");
		
		bulletRes = 0;
	}


	shaTimer++;
	if (shaTimer > 500)
	{
		shadowInit();
		shaTimer = 0; 
		isRewinding = false; 
	}

	
	SDL_Rect playerCol = player.getComponent<ColliderComponent>().collider;
	Vector2D playerPos = player.getComponent<TransformComponent>().position;
	Vector2D playerVel = player.getComponent<TransformComponent>().velocity; 
	Vector2D boxPos = box1.getComponent<TransformComponent>().position;

	//laber update

	std::stringstream ss;
	ss << "Key remaining: " << isOpenDoor;
	label.getComponent<UILabel>().SetLabelText(ss.str(), "snig");


	std::stringstream aa; 
	aa << "Try: " << TRY; 
	textTry.getComponent<UILabel>().SetLabelText(aa.str(), "snig");

	

	if (isOpenDoor == 0)
	{
		if (isOpenDoor == 0 && openAnim ==0 ) {
			doorOpen.addComponent<TransformComponent>(1050.f, 10.f, 32, 32, 3);
			doorOpen.addComponent<SpriteComponent>("door_open", true);
			doorOpen.getComponent<SpriteComponent>().Play("OPEN");
			doorOpen.addComponent<ColliderComponent>("door");
		}
		openAnim++;
	}

	if (isOpenDoor == 0 && openAnim < 115)
	{
		openAnim++; 
		camera.x = static_cast<int>(doorOpen.getComponent<TransformComponent>().position.x - 350);
		camera.y = static_cast<int>(doorOpen.getComponent<TransformComponent>().position.y - 250);
		door.destroy();
	}
	if (openAnim > 115)
	{
		doorOpen.getComponent<SpriteComponent>().setTex("door_fully");
		doorOpen.getComponent<SpriteComponent>().Play("FullOpen");
	}

	manager.refresh();
	manager.update();

	//collider check

	for (auto& c : colliders)
	{
		SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(cCol, playerCol))
		{
			player.getComponent<TransformComponent>().position = playerPos;
			//std::cout << Collision::PosAABB(playerCol, cCol) << std::endl;
		}
	}

	for (auto& b : boxs)
	{
		SDL_Rect bCol = b->getComponent<ColliderComponent>().collider;
		if (Collision::AABB(playerCol, bCol))
		{
			bool e = player.getComponent<KeyboardController>().eIsPress;
			Vector2D boxPos = b->getComponent<TransformComponent>().position; 
			if (playerPos.x <= boxPos.x)
			{
				if (playerVel.x == 0 && playerVel.y == 1 && playerPos.y < boxPos.y)
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.y < boxPos.y + 70)
							b->getComponent<TransformComponent>().position.y++;
					}
						b->getComponent<TransformComponent>().velocity = Vector2D(0, 1);
				}

				else if (playerVel.x == 1 && playerVel.y == 0)
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.x < boxPos.x + 70)
							b->getComponent<TransformComponent>().position.x++;
					}
					b->getComponent<TransformComponent>().velocity = Vector2D(1, 0);
				}

				else if (playerVel.x == 0 && playerVel.y == -1 && playerPos.y > boxPos.y)
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.y > boxPos.y - 70)
							b->getComponent<TransformComponent>().position.y--;
					}
					b->getComponent<TransformComponent>().velocity = Vector2D(0, -1);
				}
			}
			else
			{
				if (playerVel.x == -1 && playerVel.y == 0 )
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.x > boxPos.x - 70)
							b->getComponent<TransformComponent>().position.x--; 
					}
					b->getComponent<TransformComponent>().velocity = Vector2D(-1, 0);
				}
				else if (playerVel.x == 0 && playerVel.y == 1 && playerPos.y < boxPos.y)
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.y < boxPos.y + 70)
							b->getComponent<TransformComponent>().position.y++;
					}
					b->getComponent<TransformComponent>().velocity = Vector2D(0, 1);
				}
				else if (playerVel.x == 0 && playerVel.y == -1 && playerPos.y > boxPos.y)
				{
					if (e)
					{
						while (b->getComponent<TransformComponent>().position.y > boxPos.y - 70)
							b->getComponent<TransformComponent>().position.y--;
					}
					b->getComponent<TransformComponent>().velocity = Vector2D(0, -1);
				}
			}
		}
		else
			b->getComponent<TransformComponent>().velocity = Vector2D(0, 0);
		//std::cout << "Player vel: " << b->getComponent<TransformComponent>().velocity << std::endl;
	}

	for (auto& b : boxs)
	{
		SDL_Rect bCol = b->getComponent<ColliderComponent>().collider;
		for (auto& c : colliders)
		{
			SDL_Rect cCol = c->getComponent<ColliderComponent>().collider;
			Vector2D cPos = c->getComponent<TransformComponent>().position; 
			if (Collision::AABB(cCol, bCol))
			{
				b->getComponent<TransformComponent>().position = boxPos; 
			}
			
		}
		
	}

	

	for (auto& k : keys)
	{
		if (Collision::AABB(k->getComponent<ColliderComponent>().collider, player.getComponent<ColliderComponent>().collider))
		{
			isOpenDoor -= 1;
			k->destroy();
		}
	}


	if (Collision::AABB(playerCol, box1.getComponent<ColliderComponent>().collider))
	{
		player.getComponent<TransformComponent>().position = playerPos;
	}

	for (auto& b : bullets)
	{
		Vector2D bulletPos = b->getComponent<TransformComponent>().position; 
		for (auto& c : colliders)
		{
			if (Collision::AABB(b->getComponent<ColliderComponent>().collider, c->getComponent<ColliderComponent>().collider) )
			{
				b->destroy();
			}
		}
	}

	for (auto& b : bullets)
	{
		for (auto& bo : boxs)
		{
			if (Collision::AABB(b->getComponent<ColliderComponent>().collider, bo->getComponent<ColliderComponent>().collider))
			{
				b->destroy();
			}
		}
	}

	for (auto& b : bullets)
	{
		if (Collision::AABB(b->getComponent<ColliderComponent>().collider, playerCol))
		{
			if (!cheat )
				Reset();
		}
	}
	

	//camera follower 

	camera.x = static_cast<int>(player.getComponent<TransformComponent>().position.x - 400);
	camera.y = static_cast<int>(player.getComponent<TransformComponent>().position.y - 320);

	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > camera.w)
		camera.x = camera.w;
	if (camera.y > camera.h)
		camera.y = camera.h;


	std::fstream f; 
	f.open("input.txt", std::ios::out);
	f << TRY;
	f.close();

	// bebug 

	//std::cout << player.getComponent<KeyboardController>().flipCheck << std::endl;
}

void Game::render()
{
	SDL_RenderClear(renderer);
	for (auto& t : tiles)
	{
		t->draw();
	}

	for (auto& c : colliders)
	{
		c->draw();
	}

	if ( !isRewinding )
		shadow.draw();

	for (auto& p : players)
	{
		p->draw();
	}

	for (auto& b : boxs)
	{

		b->draw();
	}

	for (auto& k : keys)
	{
		k->draw();
	}

	for (auto& b : bullets)
	{
		b->draw();
	}
	if (isOpenDoor > 0 )door.draw();
	
	if (isOpenDoor == 0) doorOpen.draw();

	label.draw();
	textQuit.draw();
	textTry.draw();
	if (cheat)
	{
		hiddenText.draw();
	}
	SDL_RenderPresent(renderer);
}

void Game::clean()
{
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	Mix_Quit();
	SDL_Quit();
}


void Game::shadowInit()
{
	shadow.getComponent<TransformComponent>().position = player.getComponent<TransformComponent>().position; 
	if (player.getComponent<KeyboardController>().plaState == 0) {
		shadow.getComponent<SpriteComponent>().setTex("idle");
	}
	else if (player.getComponent<KeyboardController>().plaState == 1 )
	{
		int p = player.getComponent<SpriteComponent>().getFrame(); 
		if (p == 0)
			shadow.getComponent<SpriteComponent>().setTex("run0");
		else if (p == 16)
			shadow.getComponent<SpriteComponent>().setTex("run1");
		else if (p == 32)
			shadow.getComponent<SpriteComponent>().setTex("run2");
		else if (p == 48)
			shadow.getComponent<SpriteComponent>().setTex("run3");
		else if (p==64)
			shadow.getComponent<SpriteComponent>().setTex("run4");
		else 
			shadow.getComponent<SpriteComponent>().setTex("run5");

		
	}
	shadow.getComponent<SpriteComponent>().spriteFlip = player.getComponent<SpriteComponent>().spriteFlip;
}

void Game::playerRewind()
{
	Vector2D pos = shadow.getComponent<TransformComponent>().position; 
	player.getComponent<TransformComponent>().position = pos; 
	player.getComponent<SpriteComponent>().spriteFlip = shadow.getComponent<SpriteComponent>().spriteFlip; 
}


void Game::Reset()
{
	player.getComponent<TransformComponent>().position = Vector2D(100.0f, 100.0f);
	box1.getComponent<TransformComponent>().position = Vector2D(100, 150);
	shadow.getComponent<TransformComponent>().position = Vector2D(100.0f, 100.0f);
	for (auto& b : keys)
	{
		b->destroy();
	}

	isOpenDoor = 5;
	assets->CreateKey(Vector2D(460.f, 500.f), "key");
	assets->CreateKey(Vector2D(100., 750.f), "key");
	assets->CreateKey(Vector2D(740, 400), "key");
	assets->CreateKey(Vector2D(610, 255), "key");
	assets->CreateKey(Vector2D(740, 255), "key");
	TRY++; 
}


void Game::carrying()
{
	if (isCarry)
	{
		box1.getComponent<TransformComponent>().position = player.getComponent<TransformComponent>().position + Vector2D(5, 5);
	}
}

bool Game::isColWithBox()
{
	if (Collision::AABB(player.getComponent<ColliderComponent>().collider, box1.getComponent<ColliderComponent>().collider))
	{
		return true;
	}
	else return false;
}