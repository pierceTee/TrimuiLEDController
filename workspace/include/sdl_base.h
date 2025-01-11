#ifndef SDL_BASE_H
#define SDL_BASE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// Struct to hold core SDL components
//
// Helps keep all SDL components in one place.
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GameController *controller;
    int window_width;
    int window_height;
} CoreSDLComponents;

// Used to convert SDL inputs to a common input definition.
//
// Useful in the case of accepting both keyboard and controller inputs.
typedef enum
{
    UNKNOWN,
    START,
    SELECT,
    MENU,
    POWER,
    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_UP,
    DPAD_DOWN,
    A,
    B,
    X,
    Y,
    L1,
    R1,
    L2,
    R2,
    L3,
    R3,
} InputType;

int initialize_sdl_core(CoreSDLComponents *core_components, char *window_title);
void free_sdl_core(CoreSDLComponents *core_components);
bool is_supported_input_event(Uint32 event_type);
const char *get_event_name(Uint32 event_type);
const char *get_button_name(SDL_GameControllerButton button);
const char *get_key_name(SDL_Keycode key);
const char *get_input_type_name(Uint32 inputType);
InputType sdl_event_to_input_type(SDL_Event *event, bool verbose);
#endif
