#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include "sdl_base.h"
#include "led_controller_common.h"

/* Eggshell white color for main text */
SDL_Color text_color = {255, 239, 186, 255};

/* Semi-transparent black for shadow */
SDL_Color text_shadow_color = {0, 0, 0, 128};

/* The list of colors we support */
const uint32_t colors[] = {

    // Greens
    0x00FF00, // Green
    0x00FF80, // Aqua

    // Yellows
    0xFFFF00, // Yellow
    0x808000, // Olive

    // Blues
    0x0000FF, // Blue
    0x0080FF, // Sky Blue

    // Cyans
    0x00FFFF, // Cyan
    0x008080, // Teal

    // Oranges
    0xFF8000, // Orange
    // Reds
    0xFF0000, // Red
    // Magentas
    0xFF00FF, // Magenta
    0xFF80C0, // Light Magenta
    // Pinks
    0xFF0080, // Hot Pink

    0xFF8080, // Light Red
};

const int num_color = sizeof(colors) / sizeof(colors[0]);

/* Array of the split suffixs that sont front led files end in*/
const char *front_led_suffix[2] = {"f1", "f2"};

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
 * Control what happens when a LED setting is changed.
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *      change - the amount to change the setting by.
 * Returns:
 *      void
 */
void handle_change_setting(AppState *app_state, int change);

/**
 * Control what happens when a menu option is selected.
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *      selected_menu_option - the menu option selected.
 * Returns:
 *      void
 */
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

/**
 * Initialize the main menu (start screen) user interface.
 *
 * Parameters:
 *    menu_items - user interface object to initialize
 *    core_components - core SDL components
 *    components - SDL components specific to this application
 *
 * Returns:
 *   void
 */
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

/**
 * Update the config page (start menu) user interface text.
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
void update_config_page_ui_text(SelectableMenuItems *menu_items, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state);

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
 * Initialize the Sprite object responsible for rendering the brick animations.
 *
 * Parameters:
 *      brick_sprite - sprite object to initialize
 *      renderer - SDL renderer to draw to
 *
 * Returns:
 *      0 on success, 1 on failure
 */
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
SDL_Surface *load_image(const char *image_name);

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
 *
 * Returns:
 *      void
 */
void write_max_scale_data(FILE *file, const AppState *app_state, const Led led, char *filepath);

/**
 * Write the effect data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *
 * Returns:
 *      void
 */
void write_effect_data(FILE *file, const AppState *app_state, const Led led, char *filepath);

/**
 * Write the effect duration data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *
 * Returns:
 *      void
 */
void write_effect_duration_data(FILE *file, const AppState *app_state, const Led led, char *filepath);

/**
 * Write the color data to a file.
 *
 * Parameters:
 *      file - file to write to
 *      app_state - state object with user information we're updating.
 *      led - LED to write the data for
 *      filepath - path to the file
 *
 * Returns:
 *      void
 */
void write_color_data(FILE *file, const AppState *app_state, const Led led, char *filepath);

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

/**
 * Install the daemon.
 *
 *  Runs the install.sh script to install the daemon.
 *
 * Returns:
 *      void
 */
void install_daemon();

/**
 * Uninstall the daemon.
 *
 *  Runs the uninstall.sh script to uninstall the daemon.
 *
 * Returns:
 *      void
 */
void uninstall_daemon();

/**
 * Quick action that matches all LED colors to the current
 *
 * Parameters:
 *      app_state - state object with user information we're updating.
 *
 * Returns:
 *      void
 */
void color_match_leds(AppState *app_state);
#endif
