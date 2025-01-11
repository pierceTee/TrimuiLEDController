#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "sdl_base.h"

// ** File locations **
#define IMAGE_DIR "assets/images"
#define BACKGROUND_IMAGE_PATH "assets/images/main_menu.png"
#define BRICK_SPRITE_SHEET_PATH "assets/images/brick_sprite_sheet.png"
#define FONT_PATH "assets/retro_gaming.ttf"
#define SETTINGS_FILE "settings.ini"
#define UPDATE_LED_SYS_FILES_SCRIPT "./update_led_sys_files.sh"
// Location of system files we need to edit to change LEDs
#define SYS_FILE_PATH "/sys/class/led_anim"

// ** SDL/Animation Consts **
// TrimUI Brick WxH
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define FONT_SIZE 34
#define WINDOW_TITLE "Led Controller"
#define BRICK_SPRITE_HEIGHT 500
#define BRICK_SPRITE_WIDTH 350
#define BRICK_ANIM_FRAME_DELAY_MS 500
// Indicator to render current user selection (i.e '>>> Brightness: 100')
#define MENU_CARRET ">>> "

// ** Settings Consts **
// FRONT, TOP, BACK
#define LED_COUNT 3
// brightness, effect, color, duration
#define LED_SETTINGS_COUNT 4
// DISABLE, LINEAR, BREATH, SNIFF, STATIC, BLINK1, BLINK2, BLINK3
#define ANIMATION_EFFECT_COUNT 8
// 0xRRGGBBAA
#define COLOR_HEX_LENGTH 8
// Maximum brightness value allowed by trimui firmware
#define MAX_BRIGHTNESS 200
// Maximum duration to cycle a lighting effect
#define MAX_DURATION 5000
// How much to increment duration by when the user increases/decreases
#define DURATION_INCREMENT 500
// How much to increment the brightness by when increasing/decreasing
#define BRIGHTNESS_INCREMENT 10

// How many colors we support.
#define NUM_COLORS 20

// ** Application Consts **
#define STRING_LENGTH 256

// The different Led clusters we support
typedef enum
{
    LED_FRONT,
    LED_TOP,
    LED_BACK,
} Led;

// The different Animation effect options recognized by the TrimUI firmware
// Reference:  "/sys/class/led_anim/help"
typedef enum
{
    DISABLE,
    LINEAR,
    BREATH,
    SNIFF,
    STATIC,
    BLINK1,
    BLINK2,
    BLINK3
} AnimationEffect;

// The different LED functions we support modifying
typedef enum
{
    BRIGHTNESS,
    EFFECT,
    DURATION,
    COLOR,
} LedSettingOption;

// SDL Extended set of components required by this application not covered in sdl_base::CoreSDLComponents
typedef struct
{
    SDL_Texture *brickSpriteTexture;
    SDL_Texture *backgroundTexture;
    TTF_Font *font;
} AdditionalSDLComponents;

// Cluster of all mutable user-interface related objects
typedef struct
{
    // Texture for rendering on screen logging text.
    SDL_Texture *logging_text_texture;
    // Texture for rendering brightness value.
    SDL_Texture *brightness_text_texture;
    // Texture for rendering effect value.
    SDL_Texture *effect_text_texture;
    // Texture for rendering color value.
    SDL_Texture *color_text_texture;
    // Texture for rendering duration value.
    SDL_Texture *duration_text_texture;
    // Texture for rendering selected LED.
    SDL_Texture *selected_led_texture;
    // Texture for rendering selected option.
    SDL_Texture *selected_option_texture;
    // Log message we display in console if verbose logging is enabled.
    char log_message[STRING_LENGTH];
    // Log message we display on screen, gets updated based on user input.
    char onscreen_log_message[STRING_LENGTH];
    // Text that displays the brightness value information.
    char brightness_text[STRING_LENGTH];
    // Text that displays the effect value information.
    char effect_text[STRING_LENGTH];
    // Text that displays the color value information.
    char color_text[STRING_LENGTH];
    // Text that displays the duration value information.
    char duration_text[STRING_LENGTH];
    // Text that displays the selected LED information.
    char selected_led_text[STRING_LENGTH];
    // Text that displays the selected option information.
    char selected_option_text[STRING_LENGTH];
} UserInterface;

// Abstraction of what frames relate to a particular animation.
//
// I.E if an animation uses frames 0, 3, 4 of the horizontal sprite sheet,
// it would be stored as:
//  frame_indicies = [0, 3, 4]
//  frame_count = 3
typedef struct
{
    int *frame_indicies;
    int frame_count;
} AnimationInfo;

// Collection of values for each supported LED setting.
typedef struct
{
    int brightness;
    AnimationEffect effect;
    uint32_t color;
    int duration;
} LedSettings;

typedef struct
{
    bool should_save_settings;
    bool should_update_leds;
    bool should_quit;
    Led selected_led;
    LedSettingOption selected_setting;
    LedSettings led_settings[LED_COUNT];
} AppState;

const uint32_t bright_colors[] = {
    0xFF0000, // Red
    0x00FF00, // Green
    0x0000FF, // Blue
    0xFFFF00, // Yellow
    0x00FFFF, // Cyan
    0xFF00FF, // Magenta
    0xFFFFFF, // White
    0xFF8000, // Orange
    0x8000FF, // Purple
    0x80FF00, // Lime
    0x00FF80, // Aqua
    0x0080FF, // Sky Blue
    0xFF0080, // Hot Pink
    0xFF80FF, // Light Pink
    0x80FF80, // Light Green
    0x8080FF, // Light Blue
    0xFFFF80, // Light Yellow
    0x80FFFF, // Light Cyan
    0xFF8080, // Light Red
    0xFF80C0  // Light Magenta
};

/**
 * Manage what to do with user input.
 *
 *  Shoulder buttons change which LED is selected.
 *  B button quits the application.
 *  DPAD UP/DOWN selects the setting to change
 *  DPAD LEFT/RIGHT changes the setting value
 *  START saves the configuration file.
 * Parameters:
 *      user_input - pre-processed user input converted from SDL_Event
 *      should_quit - loop control flag to exit the main loop.
 *      app_state - state object with user information we're updating.
 * Returns:
 *      void
 */
void handle_user_input(InputType user_input, AppState *app_state);

/**
 * Handle changes to the selected setting.
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *      change - the amount to change the setting by.
 * Returns:
 *      void
 */
void handle_change_setting(AppState *app_state, int change);

/**
 * Initialize any SDL components needed by the application that aren't
 * already initialized by initialize_sdl_core.
 *
 * Parameters:
 *      core_components - core SDL components
 *      components - SDL components specific to this application
 *
 * Returns:
 *      0 on success, 1 on failures
 */
int initialize_additional_sdl_components(CoreSDLComponents *core_components, AdditionalSDLComponents *components);

/**
 * Initialize the user state object.
 *
 * Parameters:
 *    app_state - state object to initialize
 *
 * Returns:
 *   0 on success, 1 on failure
 */
int initialize_app_state(AppState *app_state);

/**
 * Initialize the user interface.
 *
 * Parameters:
 *    user_interface - user interface object to initialize
 *    core_components - core SDL components
 *    components - SDL components specific to this application
 *
 * Returns:
 *   void
 */
void initialize_user_interface(UserInterface *user_interface, CoreSDLComponents *core_components, AdditionalSDLComponents *components);

/**
 * Frees all sub-objects of the user_interface
 *
 * Can be used for cleanup and to clear the text off the screen in
 * preparation for the next frame.
 */
void free_user_interface(UserInterface *user_interface);

/**
 * Update the user interface text.
 *
 * Parameters:
 *    user_interface - user interface object to update
 *    core_components - core SDL components
 *    components - SDL components specific to this application
 *    app_state - state object with user information we're updating.
 *
 * Returns:
 *   void
 */
void update_user_interface_text(UserInterface *user_interface, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state);

/**
 * Clamp a value between a minimum and maximum value.
 *
 * Parameters:
 *      value - the value to clamp
 *      min - the minimum value
 *      max - the maximum value
 *
 * Returns:
 *      the clamped value
 */
int clamp(int value, int min, int max);

/**
 * Read settings from a file.
 *
 * Parameters:
 *      app_state - state object to read settings into
 *
 * Returns:
 *      0 on success, 1 on failure
 */
int read_settings(AppState *app_state);

/**
 * Save settings to a file.
 *
 * Parameters:
 *      app_state - state object to save settings from
 *
 * Returns:
 *      0 on success, 1 on failure
 */
int save_settings(AppState *app_state);

/**
 * Initialize the mapping of LED options to sprite sheet indicies.
 *
 * Parameters:
 *   animations - mapping of LED options to sprite sheet indicies
 *
 * Returns:
 *  void
 */
void initialize_animations(AnimationInfo *animations);

/**
 * Render the sprite to the screen starting at a given x/y coordinate.
 *
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      sprite - SDL surface containing the sprite sheet
 *      frame_index - index of the frame to render
 *      position_x - x position to render the sprite
 *      position_y - y position to render the sprite
 *
 *  Returns:
 *     void
 */
void render_sprite(SDL_Renderer *renderer, SDL_Texture *sprite_texture, int frame_index, int position_x, int position_y);

/**
 * Render a text texture to the screen.
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      texture - SDL texture containing the text
 *      x - x position to render the text
 *      y - y position to render the text
 *
 * Returns:
 *      void
 */
void render_text_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y);

/**
 * Render the user interface to the screen.
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      user_interface - user interface object to render
 *      start_x - x position to start rendering
 *      start_y - y position to start rendering
 *
 * Returns:
 *      void
 */
void render_user_interface(SDL_Renderer *renderer, UserInterface *user_interface, int start_x, int start_y);

/**
 * Update the animation frame index.
 *
 * Parameters:
 *      last_frame_time_millis - time of the last frame in milliseconds
 *      brick_anim_frame_index - index of the current frame
 *      frame_count - total number of frames in the animation
 *
 * Returns:
 *      void
 */
void update_animation_frame_index(Uint32 *last_frame_time_millis, int *brick_anim_frame_index, int frame_count);

/**
 * Load an image from a file.
 *
 * Parameters:
 *      image_name - name of the image file to load
 *
 * Returns:
 *      SDL_Surface containing the image
 */
SDL_Surface *load_image(char *image_name);

/**
 * Create a text texture from a string.
 *
 * Parameters:
 *      renderer - SDL renderer to draw to.
 *      font - TTF font to use for the text
 *      text - string containing the text
 *
 * Returns:
 *      SDL_Texture containing the text
 */
SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text);

/**
 * Convert an animation effect to a string.
 *
 * Parameters:
 *      effect - animation effect to convert
 *
 * Returns:
 *      string containing the animation effect
 */
char *animation_effect_to_string(AnimationEffect effect);

/**
 * Convert a LED setting option to a string.
 *
 * Parameters:
 *      setting - LED setting option to convert
 *
 * Returns:
 *      string containing the LED setting option
 */
char *led_setting_option_to_string(LedSettingOption setting);

/**
 * Convert a LED to a string.
 *
 * Parameters:
 *      led - LED to convert
 *
 * Returns:
 *      string containing the LED
 */
char *led_to_string(Led led);

/**
 * Get the internal name of a LED.
 *
 * Parameters:
 *      led - LED to get the internal name of
 *
 * Returns:
 *      string containing the internal name of the LED
 */
char *led_internal_name(Led led);

/**
 * Log a debug message.
 *
 * Parameters:
 *      message - message to log
 *      verbose_logging_enabled - flag to enable verbose logging
 *
 * Returns:
 *      void
 */
void debug_log(char *message, bool verbose_logging_enabled);

/**
 * Update the LED system files.
 *
 * Returns:
 *      void
 */
void update_led_sys_files();

/**
 * Render a colored square to the screen.
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *      core_components - core SDL components
 *
 * Returns:
 *      void
 */
void render_colored_square(AppState *app_state, CoreSDLComponents *core_components);

/**
 * Teardown the application.
 *
 * Parameters:
 *      core_components - core SDL components
 *      components - SDL components specific to this application
 *      user_interface - user interface object to teardown
 *
 * Returns:
 *      0 on success, 1 on failure
 */
int teardown(CoreSDLComponents *core_components, AdditionalSDLComponents *components, UserInterface *user_interface);

/**
 * Write the max scale data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *      suffix - suffix to add to the file path
 *
 * Returns:
 *      void
 */
void write_max_scale_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);

/**
 * Write the effect data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *      suffix - suffix to add to the file path
 *
 * Returns:
 *      void
 */
void write_effect_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);

/**
 * Write the effect duration data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *      suffix - suffix to add to the file path
 *
 * Returns:
 *      void
 */
void write_effect_duration_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);

/**
 * Write the color data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *      suffix - suffix to add to the file path
 *
 * Returns:
 *      void
 */
void write_color_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);

/**
 * Write user selection data to the associated LED files.
 *
 *  Opens several files in the /sys/class/led_anim directory and writes
 *  the user selected values to the files. The firmware continually
 *  checks the values in this directory and updates the LEDs accordingly.
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *
 * Returns:
 *      void
 */
void update_leds(AppState *app_state);

#endif
