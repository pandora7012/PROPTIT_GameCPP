#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include "Vector2D.h"

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

	void Repeat(); 

	void shadowInit();
	static void playerRewind();
	void kickBox();
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

private:
	
	int cnt = 0;
	SDL_Window* window;
};