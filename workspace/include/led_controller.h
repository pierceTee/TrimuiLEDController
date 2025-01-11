#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "sdl_base.h"

// ** File locations **
#define IMAGE_DIR "menu_images"
#define SETTINGS_FILE "settings.ini"
#define UPDATE_LED_SYS_FILES_SCRIPT "./update_led_sys_files.sh"
#define SETTINGS_DAEMON_SCRIPT "./settings_daemon"
// Location of system files we need to edit to change LEDs
#define SYS_FILE_PATH "/sys/class/led_anim"

// ** SDL/Animation Consts **
// TrimUI Brick WxH
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define FONT_SIZE 24
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
    SDL_Surface *brickSprite;
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

void handle_user_input(InputType user_input, AppState *app_state);
void handle_change_setting(AppState *app_state, int change);
int initialize_additional_sdl_components(CoreSDLComponents *core_components, AdditionalSDLComponents *components);
int initialize_app_state(AppState *app_state);
void initialize_user_interface(UserInterface *user_interface, CoreSDLComponents *core_components, AdditionalSDLComponents *components);
void free_user_interface(UserInterface *user_interface);
void update_user_interface_text(UserInterface *user_interface, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state);
int clamp(int value, int min, int max);
int read_settings(AppState *app_state);
int save_settings(AppState *app_state);
void initialize_animations(AnimationInfo *animations);
void render_brick_sprite(SDL_Renderer *renderer, SDL_Surface *sprite, int frame_index, int position_x, int position_y);
void render_text_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y);
void render_user_interface(SDL_Renderer *renderer, UserInterface *user_interface, int start_x, int start_y);

void update_animation_frame_index(Uint32 *last_frame_time_millis, int *brick_anim_frame_index, int frame_count);
SDL_Surface *load_image(char *image_name);
SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text);
char *animation_effect_to_string(AnimationEffect effect);
char *led_setting_option_to_string(LedSettingOption setting);
char *led_to_string(Led led);
char *led_internal_name(Led led);
void debug_log(char *message, bool verbose_logging_enabled);
void update_led_sys_files();
void render_colored_square(AppState *app_state, CoreSDLComponents *core_components);
void update_leds(AppState *app_state);
int teardown(CoreSDLComponents *core_components, AdditionalSDLComponents *components, UserInterface *user_interface);
void write_max_scale_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);
void write_effect_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);
void write_effect_duration_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);
void write_color_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix);
#endif
