#pragma once

#include "../Game.h"
#include "ECS.h"
#include "Components.h"

class KeyboardController : public Component
{
public:
	TransformComponent* transform;
	SpriteComponent* sprite;
	int plaState = 0; 
	bool flipCheck = false; 
	bool isPress = 0; 
	bool eIsPress = 0; 

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
	}


	void update() override
	{
		if (Game::event.type == SDL_KEYDOWN )
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_w:
				transform->velocity.y = -1;
				sprite->Play("Walk");
				plaState = 1; 
				break;
			case SDLK_a:
				transform->velocity.x = -1;
				sprite->Play("Walk");
				plaState = 1;
				if (flipCheck == 0)
				{
					sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
				}
				flipCheck = 1;
				break;
			case SDLK_d:
				transform->velocity.x = 1;
				sprite->Play("Walk");
				plaState = 1; 
				sprite->spriteFlip = SDL_FLIP_NONE; 
				flipCheck = 0; 
				break;
			case SDLK_s:
				transform->velocity.y = 1;
				sprite->Play("Walk");
				plaState = 1; 
				break;
			case SDLK_r:
				if (!isPress )
				{
					Game::playerRewind();
					isPress = true;
				}
			default:
				break;
			}
		}

		if (Game::event.type == SDL_KEYUP)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_w:
				transform->velocity.y = 0;
				sprite->Play("Idle");
				plaState = 0;
				break;
			case SDLK_a:
				transform->velocity.x = 0;
				sprite->Play("Idle");
				plaState = 0; 
				break;
			case SDLK_d:
				transform->velocity.x = 0;
				sprite->Play("Idle");
				plaState = 0;
				break;
			case SDLK_s:
				transform->velocity.y = 0;
				sprite->Play("Idle");
				plaState = 0; 
				break;
			case SDLK_r:
				isPress = false; 
				break; 
			case SDLK_ESCAPE:
				Game::isRunning = false;
			default:
				break;
			}
		}
	}
};