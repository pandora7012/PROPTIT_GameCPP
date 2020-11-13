#include "Game.h"
#include <SDL_ttf.h>

typedef struct {
    SDL_Rect draw_rect;    // dimensions of button
    struct {
        Uint8 r, g, b, a;
    } colour;

    bool pressed;
} button_t;

static void button_process_event(button_t* btn, const SDL_Event* ev) {
    // react on mouse click within button rectangle by setting 'pressed'
    if (ev->type == SDL_MOUSEBUTTONDOWN) {
        if (ev->button.button == SDL_BUTTON_LEFT &&
            ev->button.x >= btn->draw_rect.x &&
            ev->button.x <= (btn->draw_rect.x + btn->draw_rect.w) &&
            ev->button.y >= btn->draw_rect.y &&
            ev->button.y <= (btn->draw_rect.y + btn->draw_rect.h)) {
            btn->pressed = true;
        }
    }
}

static bool button(SDL_Renderer* r, button_t* btn) {
    // draw button
    SDL_SetRenderDrawColor(r, btn->colour.r, btn->colour.g, btn->colour.b, btn->colour.a);
    SDL_RenderFillRect(r, &btn->draw_rect);

    // if button press detected - reset it so it wouldn't trigger twice
    if (btn->pressed) {
        btn->pressed = false;
        return true;
    }
    return false;
}

Game *game = nullptr;

int main(int argc, char *argv[])
{
    SDL_Surface* surf;
    TTF_Font* font ;

	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	game = new Game();
	game->init("Game", 800, 640, false);
	
	while (game->running())
	{

		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		frameTime = SDL_GetTicks() - frameStart;
		
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
		
	}

	game->clean();

	return 0;
}