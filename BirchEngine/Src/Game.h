#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include "Vector2D.h"
#include <SDL_mixer.h>

class AssetManager;
class ColliderComponent;

class Game
{
public:
	Game();
	~Game();

	void init(const char* title, int width, int height, bool fullscreen);

	void handleEvents();
	void update();
	bool running() { return isRunning; }
	void render();
	void clean();

	void Reset();
	void carrying();
	void shadowInit();

	bool isColWithBox(); 

	static bool isCarry;
	static bool cheat;
	static void playerRewind();
	static SDL_Renderer* renderer;
	static SDL_Event event;
	static bool isRunning;
	static SDL_Rect camera;
	static AssetManager* assets;
	enum groupLabels : std::size_t
	{
		groupMap,
		groupPlayers,
		groupColliders,
		groupProjectiles,
		groupBoxs,
		groupKeys,
		groupBullets
	};

	static int ReCount; 

	float shaTimer; 
	bool isRewinding; 
	int isOpenDoor; 
	int TRY; 


private:
	
	int cnt = 0;
	SDL_Window* window;
	Mix_Music* gMusic;
};