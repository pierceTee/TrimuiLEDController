#ifndef SDL_BASE_H
#define SDL_BASE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

/* Struct to hold core SDL components
 *
 * Helps keep all SDL components in one place.
 */
typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_GameController *controller;
    int window_width;
    int window_height;
} CoreSDLComponents;

/* Abstraction of what frames relate to a particular animation.
 *
 * I.E if an animation uses frames 0, 3, 4 of the horizontal sprite sheet,
 * with durations of 1000, 500, 500 milliseconds respectively,
 * it would be stored as:
 *  frame_indicies = [0, 3, 4]
 *  frame_duration_millis = [1000, 500, 500]
 *  frame_count = 3
 *  current_frame_index = 0
 *  last_frame_time_millis = 0
 */
typedef struct
{
    int *frame_indicies;
    int *frame_duration_millis;
    int frame_count;
    int current_frame_index;
    Uint32 last_frame_time_millis;
} AnimationInfo;

/* Sprite object that contains everything necessary to render a sprite. */
typedef struct
{
    SDL_Texture *sprite_texture;
    AnimationInfo *animations;
    int animation_count;
    int current_animation_index;
    int sprite_width;
    int sprite_height;
} Sprite;

/* Used to convert SDL inputs to a common input definition.
 *
 * Useful in the case of accepting both keyboard and controller inputs.
 */
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

/**
 * Handles the initialization of common SDL components necessary for every app
 *
 * Parameters:
 *      core_components - A pre-allocated CoreSDLComponents struct
 *                        to encapsulate common SDL objects
 *      window_title - The title displayed at the top of the window.
 *
 * Returns:
 *      0 on success, 1 on failures
 */
int initialize_sdl_core(CoreSDLComponents *core_components, char *window_title);

/**
 * Frees the core SDL components.
 *
 * Parameters:
 *      core_components - A pre-allocated CoreSDLComponents struct
 *                        to encapsulate common SDL objects
 */
void free_sdl_core(CoreSDLComponents *core_components);

/**
 * Frees the sprite object.
 *
 * Parameters:
 *     sprite - The sprite object to free
 */
void free_sprite(Sprite *sprite);

/**
 * Handles all the logic for preparing the next frame of a sprite animation.
 *
 *  Works by selecting the correct section of the sprite sheet to render and
 *  applying that to a SDL_Rect which is copies to the renderer at the chosen
 *  position. If enough time has elapsed, the frame index is updated to the next
 *  frame in the animation. As with all Sprite Objects, we expect the sprite to be
 *  a horizontal sprite sheet.
 *
 *  Note: Animation index is expected to be set before calling this function.
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      sprite - SDL surface containing the sprite sheet
 *      position_x - x position to render the sprite
 *      position_y - y position to render the sprite
 *
 *  Returns:
 *     void
 */
void update_sprite_render(SDL_Renderer *renderer, Sprite *sprite, int position_x, int position_y);

/**
 * Converts an SDL event to an InputType to simplify input handling
 *
 * Parameters:
 *      event - The SDL event to convert
 *      verbose - If true, prints the input type to the console
 *
 * Returns:
 *      InputType enum value if the event is supported, UNKNOWN otherwise
 */
InputType sdl_event_to_input_type(SDL_Event *event, bool verbose);

/**
 * Create an SDL texture from an image file.
 *
 *  Returned pointer must be freed by the caller
 *
 * Parameters:
 *      renderer - The SDL renderer to create the texture with
 *      full_image_path - The full path to the image file
 *
 * Returns:
 *      SDL_Texture pointer containing the image.
 */
SDL_Texture *create_sdl_texture_from_image(SDL_Renderer *renderer, char *full_image_path);

/**
 * Create a text texture from a string.
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      font - TTF font to use for the text
 *      text_color - color of the text
 *      shadow_color - color of the text drop-shadow (leave null disable drop shadow rendering)
 *      text - string containing the text
 *
 * Returns:
 *      SDL_Texture containing the text
 */
SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const SDL_Color *text_color, const SDL_Color *shadow_color, const char *text);

/**
 * Checks if an SDL event is supported by the input handling system
 *
 * Parameters:
 *      event_type - The SDL event type to check
 *
 * Returns:
 *      true if the event is supported by the input handling system, false otherwise
 */
bool is_supported_input_event(Uint32 event_type);

/**
 * Gets the name of an SDL_Keycode
 *
 * Parameters:
 *      key - The SDL_Keycode to get the name of
 *
 * Returns:
 *      A string containing the name of the key
 */
const char *get_input_type_name(Uint32 inputType);

/**
 * Gets the name of an SDL event type
 *
 * Parameters:
 *      eventType - The SDL event type to get the name of
 *
 * Returns:
 *      A string containing the name of the event type
 */
const char *get_event_name(Uint32 eventType);

/**
 * Gets the name of an SDL_GameControllerButton
 *
 * Parameters:
 *      button - The SDL_GameControllerButton to get the name of
 *
 * Returns:
 *      A string containing the name of the button
 */
const char *get_button_name(SDL_GameControllerButton button);

/**
 * Gets the name of an SDL_Keycode
 *
 * Parameters:
 *      key - The SDL_Keycode to get the name of
 *
 * Returns:
 *      A string containing the name of the key
 */
const char *get_key_name(SDL_Keycode key);

#endif
