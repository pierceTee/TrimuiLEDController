#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "sdl_base.h"

// ** File locations **
#define IMAGE_DIR "assets/images"
#define BACKGROUND_IMAGE_PATH "assets/images/background.png"
#define MENU_IMAGE_PATH "assets/images/main_menu.png"
#define BRICK_SPRITE_SHEET_PATH "assets/images/brick_sprite_sheet.png"
#define FONT_PATH "assets/retro_gaming.ttf"
#define SETTINGS_FILE "settings.ini"
#define UPDATE_LED_SYS_FILES_SCRIPT "./update_led_sys_files.sh"
/* Location of system files we need to edit to change LEDs */
#define SYS_FILE_PATH "/sys/class/led_anim"

// ** SDL/Animation Consts **
/* TrimUI Brick WxH */
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define FONT_SIZE 34
#define WINDOW_TITLE "Led Controller"
#define BRICK_SPRITE_HEIGHT 512
#define BRICK_SPRITE_WIDTH 512

/* Indicator to render current user selection (i.e '>>> Brightness: 100') */
#define MENU_CARRET_LEFT "<  "
#define MENU_CARRET_RIGHT "  >"

//  ** Settings Consts **
/* FRONT, TOP, BACK */
#define LED_COUNT 3
/* brightness, effect, color, duration */
#define LED_SETTINGS_COUNT 5

/* enable all, disable all, uninstall, quit*/
#define MENU_OPTION_COUNT 4

/* DISABLE, LINEAR, BREATH, SNIFF, STATIC, BLINK1, BLINK2, BLINK3 */
#define ANIMATION_EFFECT_COUNT 8
/* 0xRRGGBBAA */
#define COLOR_HEX_LENGTH 8
/* Maximum brightness value allowed by trimui firmware */
#define MAX_BRIGHTNESS 200
/* Maximum duration to cycle a lighting effect */
#define MAX_DURATION 5000
/* How much to increment duration by when the user increases/decreases */
#define DURATION_INCREMENT 500
/* How much to increment the brightness by when increasing/decreasing */
#define BRIGHTNESS_INCREMENT 10
/* How many colors we support */
#define NUM_COLORS 17
/* Application Consts */
#define STRING_LENGTH 256

/* Eggshell white color for main text */
const SDL_Color text_color = {255, 239, 186, 255};

/* Semi-transparent black for shadow */
const SDL_Color text_shadow_color = {0, 0, 0, 128};

/* The different Led clusters we support */
typedef enum
{
    LED_FRONT,
    LED_TOP,
    LED_BACK,
} Led;

/* The different Animation effect options recognized by the TrimUI firmware */
/* Reference:  "/sys/class/led_anim/help" */
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

/* The different LED functions we support modifying */
typedef enum
{
    SELECTED_LED,
    BRIGHTNESS,
    EFFECT,
    DURATION,
    COLOR,
} LedSettingOption;

/* The different menu options we provide */
typedef enum
{
    ENABLE_ALL,
    DISABLE_ALL,
    UNINSTALL,
    QUIT,
} MenuOption;

/* SDL Extended set of components required by this application not covered in sdl_base::CoreSDLComponents */
typedef struct
{
    SDL_Texture *backgroundTexture;
    SDL_Texture *menuTexture;
    TTF_Font *font;
} AdditionalSDLComponents;

/* Cluster of all mutable user-interface related objects */
typedef struct
{
    SDL_Color text_shadow_color;
    SDL_Color text_color;
    SDL_Color text_highlight_color;
    SDL_Texture **menu_text_textures;
    char **menu_text;
    int item_count;
    int string_length;

} SelectableMenuItems;

/* Collection of values for each supported LED setting. */
typedef struct
{
    int brightness;
    AnimationEffect effect;
    uint32_t color;
    int duration;
} LedSettings;

typedef enum
{
    CONFIG_PAGE,
    MENU_PAGE,
} ApplicationPage;

typedef struct
{
    bool should_save_settings;
    bool should_update_leds;
    bool should_quit;
    bool should_install_daemon;
    ApplicationPage current_page;
    Led selected_led;
    LedSettingOption selected_setting;
    MenuOption selected_menu_option;
    LedSettings led_settings[LED_COUNT];
} AppState;

const uint32_t colors[] = {
    // Reds
    0xFF0000, // Red
    0xFF8080, // Light Red
    0x800000, // Maroon
    // Pinks
    0xFF0080, // Hot Pink
    // Oranges
    0xFF8000, // Orange

    // Greens
    0x00FF00, // Green
    0x00FF80, // Aqua

    // Yellows
    0xFFFF00, // Yellow
    0x808000, // Olive

    // Blues
    0x0000FF, // Blue
    0x0080FF, // Sky Blue
    0x000080, // Navy

    // Cyans
    0x00FFFF, // Cyan
    0x008080, // Teal

    // Magentas
    0xFF00FF, // Magenta
    0xFF80C0, // Light Magenta

    // Whites
    0xFFFFFF, // White

};

char *color_to_string(uint32_t color);

const int num_color = sizeof(colors) / sizeof(colors[0]);
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

void handle_menu_select(AppState *app_state, MenuOption selected_menu_option);
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
 *    menu_items - user interface object to initialize
 *    core_components - core SDL components
 *    components - SDL components specific to this application
 *
 * Returns:
 *   void
 */
void initialize_config_page_ui(SelectableMenuItems *menu_items, CoreSDLComponents *core_components, AdditionalSDLComponents *components);

void initialize_menu_ui(SelectableMenuItems *menu_items, CoreSDLComponents *core_components, AdditionalSDLComponents *components);

/**
 * Frees all sub-objects of the menu_items
 *
 * Can be used for cleanup and to clear the text off the screen in
 * preparation for the next frame.
 */
void free_menu_items(SelectableMenuItems *menu_items);

/**
 * Update the user interface text.
 *
 * Parameters:
 *    menu_items - user interface object to update
 *    core_components - core SDL components
 *    components - SDL components specific to this application
 *    app_state - state object with user information we're updating.
 *
 * Returns:
 *   void
 */
void update_menu_ui_text(SelectableMenuItems *menu_items, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state);

void update_config_page_ui_text(SelectableMenuItems *menu_items, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state);
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

int initialize_brick_sprite(Sprite *brick_sprite, SDL_Renderer *renderer);

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
 *      menu_items - user interface object to render
 *      start_x - x position to start rendering
 *      start_y - y position to start rendering
 *
 * Returns:
 *      void
 */
void render_menu_items(SDL_Renderer *renderer, SelectableMenuItems *menu_items, int start_x, int start_y);

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

char *menu_option_to_string(MenuOption option);

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
 * Convert an internal LED name to an index.
 *
 * Parameters:
 *      led_name - internal name of the LED
 *
 * Returns:
 *     Led object
 */
Led internal_led_name_to_led(char *led_name);

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
 *      menu_items - user interface object to teardown
 *
 * Returns:
 *      0 on success, 1 on failure
 */
int teardown(CoreSDLComponents *core_components, AdditionalSDLComponents *components,
             SelectableMenuItems *config_menu_items, SelectableMenuItems *main_menu_items, Sprite *brick_sprite);
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

void install_daemon();

void uninstall_daemon();
#endif
