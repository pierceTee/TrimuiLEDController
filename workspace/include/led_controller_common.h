#ifndef LED_CONTROLLER_COMMON_H
#define LED_CONTROLLER_COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

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
#define LED_SETTINGS_COUNT 6

/* enable all, disable all, uninstall, quit*/
#define MENU_OPTION_COUNT 6

/* DISABLE, LINEAR, BREATH, SNIFF, STATIC, BLINK1, BLINK2, BLINK3 */
#define ANIMATION_EFFECT_COUNT 8
/* 0xRRGGBBAA */
#define COLOR_HEX_LENGTH 8
/* Maximum brightness value allowed by trimui firmware */
#define MAX_BRIGHTNESS 100
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
/*How much to increment/decrement the color when the user changes the value in extened color mode*/
#define COLOR_CYLCE_INCREMENT 16

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
  MATCH_SETTINGS
} LedSettingOption;

/* The different menu options we provide */
typedef enum
{
  ENABLE_ALL,
  DISABLE_ALL,
  TOGGLE_EXTENDED_COLORS,
  TOGGLE_LOW_BATTERY_INDICATION,
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
  bool are_extended_colors_enabled;
  bool should_enable_low_battery_indication;
  ApplicationPage current_page;
  Led selected_led;
  LedSettingOption selected_setting;
  MenuOption selected_menu_option;
  LedSettings led_settings[LED_COUNT];
} AppState;

/**
 * Convert a color to a string.
 *
 * Parameters:
 *      color - 0x8 RGB hex value of the color to convert
 *
 * Returns:
 *      string containing the color
 */
const char *color_to_string(uint32_t color);

/**
 * Cycles to the next color in the color wheel.
 *
 * This is used to cycle colors in the extended color mode.
 * Parameters:
 *    color - the current 0xRRGGBBAA color to cycle from
 *   sign - direction to cycle the color
 */
uint32_t next_color(uint32_t color, int sign);
/**
 * Convert an animation effect to a string.
 *
 * Parameters:
 *      effect - animation effect to convert
 *
 * Returns:
 *      string containing the animation effect
 */
const char *animation_effect_to_string(AnimationEffect effect);

/**
 * Convert a LED setting option to a string.
 *
 * Parameters:
 *      setting - LED setting option to convert
 *
 * Returns:
 *      string containing the LED setting option
 */
const char *led_setting_option_to_string(LedSettingOption setting);

/**
 * Convert a menu option to a string.
 *
 * Parameters:
 *    option - menu option to convert
 *    app_state - application state to reference for conditional strings
 *
 * Returns:
 *   string containing the menu option
 */

const char *menu_option_to_string(MenuOption option, const AppState *app_state);

/**
 * Convert a LED to a string.
 *
 * Parameters:
 *      led - LED to convert
 *
 * Returns:
 *      string containing the LED
 */
const char *led_to_string(Led led);

/**
 * Get the internal name of a LED.
 *
 * Parameters:
 *      led - LED to get the internal name of
 *
 * Returns:
 *      string containing the internal name of the LED
 */
const char *led_internal_name(Led led);

/**
 * Convert an internal LED name to an index.
 *
 * Parameters:
 *      led_name - internal name of the LED
 *
 * Returns:
 *     Led object
 */
Led internal_led_name_to_led(const char *led_name);

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
void debug_log(const char *message, bool verbose_logging_enabled);

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
#endif
