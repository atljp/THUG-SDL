#include <SDL2/SDL.h>

typedef void (*eventHandler)(SDL_Event*);

void initEvents();
void handleEvents();
void registerEventHandler(eventHandler handler);