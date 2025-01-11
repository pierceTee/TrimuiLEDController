#include "led_controller.h"
#include "sdl_base.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    // Handle program inputs
    bool verbose_logging_enabled = argc > 1 && strcmp(argv[1], "-v") == 0;

    // Core SDL components that every application (I write) requires.
    CoreSDLComponents core_components;

    // SDL components that this application (I write) requires beyond the core components.
    AdditionalSDLComponents components;

    // User state object, contains all relevant user selection information.
    AppState app_state;

    // The collection of user interface related objects.
    UserInterface user_interface;

    // Mapping of LED cluster to sprite sheet indices.
    AnimationInfo animations[LED_COUNT];

    // The last user input converted from SDL_Event.
    InputType user_input;

    // SDL event object we read on loop.
    SDL_Event event;

    // Log message we display in console if verbose logging is enabled.
    char log_message[STRING_LENGTH];

    // Animation variables
    // Index of the current frame in the brick animation.
    int brick_anim_frame_index = 0;

    // Time of the last frame in the brick animation.
    Uint32 last_frame_time_millis = SDL_GetTicks();

    // Initialize onscreen logging message

    // Initialize Primary SDL components
    core_components.window_width = WINDOW_WIDTH;
    core_components.window_height = WINDOW_HEIGHT;

    // Initialize auxilliary data structures
    initialize_app_state(&app_state);
    initialize_animations(animations);
    update_leds(&app_state);
    if (initialize_sdl_core(&core_components, WINDOW_TITLE) != 0 ||
        initialize_additional_sdl_components(&core_components, &components) != 0)
    {
        SDL_Log("Failed to initialize SDL, exiting...\n");
        return 1;
    }
    initialize_user_interface(&user_interface, &core_components, &components);
    update_user_interface_text(&user_interface, &core_components, &components, &app_state);

    // Render the background
    SDL_RenderCopy(core_components.renderer, components.backgroundTexture, NULL, NULL);

    while (!app_state.should_quit)
    {
        // Handle events
        while (SDL_PollEvent(&event) != 0)
        {
            // Ignore unsupported input events
            if (!is_supported_input_event(event.type))
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                app_state.should_quit = true;
                break;
            }
            // Take and process user input.
            user_input = sdl_event_to_input_type(&event, false);

            handle_user_input(user_input, &app_state);

            if (is_supported_input_event(event.type))
            {
                if (app_state.should_update_leds)
                {
                    update_leds(&app_state);
                }
                // Save settings if necessary, live changes to the ini are picked up by settings_daemon.
                if (app_state.should_save_settings)
                {
                    save_settings(&app_state);
                }
                // Update UI text textures
                update_user_interface_text(&user_interface, &core_components, &components, &app_state);
            }
        }

        // Update animation frame
        update_animation_frame_index(&last_frame_time_millis, &brick_anim_frame_index, animations[app_state.selected_led].frame_count);

        // Clear screen
        SDL_RenderClear(core_components.renderer);

        // Render background texture
        SDL_RenderCopy(core_components.renderer, components.backgroundTexture, NULL, NULL);

        // Render the user-selected color in front of a black square
        render_colored_square(&app_state, &core_components);

        // Index on the sheet just cycles between the available indicies for the selected LED.
        int brick_sprite_index = animations[app_state.selected_led].frame_indicies[brick_anim_frame_index];
        snprintf(log_message, sizeof(log_message), "Selected LED: %d, Frame index: %d\n", app_state.selected_led, brick_sprite_index);
        debug_log(log_message, verbose_logging_enabled);

        // Render brick sprite
        render_brick_sprite(core_components.renderer, components.brickSprite, brick_sprite_index, 100, 100);

        // Render the interactable user interface.

        render_user_interface(core_components.renderer, &user_interface, 500, 200);

        // Main render call to update screen
        SDL_RenderPresent(core_components.renderer);
        SDL_Log("Error: %s\n", SDL_GetError());
        // Delay to control frame rate
        //  SDL_Delay(16); // Approximately 60 frames per second
    }

    save_settings(&app_state);
    update_leds(&app_state);
    return teardown(&core_components, &components, &user_interface);
}

void render_colored_square(AppState *app_state, CoreSDLComponents *core_components)
{
    SDL_Rect black_rect = {100, 96, BRICK_SPRITE_WIDTH, BRICK_SPRITE_HEIGHT + 4}; // Adjusted to add 4 pixels on top
    SDL_SetRenderDrawColor(core_components->renderer, 0, 0, 0, 255);              // Black color
    SDL_RenderFillRect(core_components->renderer, &black_rect);

    SDL_Rect color_rect = {105, 101, BRICK_SPRITE_WIDTH - 10, BRICK_SPRITE_HEIGHT - 6}; // Adjusted to match the new black_rect
    uint32_t color = app_state->led_settings[app_state->selected_led].color;
    SDL_SetRenderDrawColor(core_components->renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderFillRect(core_components->renderer, &color_rect);
}

void handle_user_input(InputType user_input, AppState *app_state)
{
    switch (user_input)
    {
    case POWER:
    case START:
        app_state->should_save_settings = true;
        break;
    case B:
        app_state->should_quit = true;
        break;
    case DPAD_UP:
        app_state->selected_setting = (app_state->selected_setting - 1) % LED_SETTINGS_COUNT; // Switch between settings
        break;
    case DPAD_DOWN:
        app_state->selected_setting = (app_state->selected_setting + 1 + LED_SETTINGS_COUNT) % LED_SETTINGS_COUNT; // Switch between settings
        break;
    case DPAD_RIGHT:
        handle_change_setting(app_state, 1);
        app_state->should_update_leds = true;
        break;
    case DPAD_LEFT:
        handle_change_setting(app_state, -1);
        app_state->should_update_leds = true;
        break;
    case L1:
    case L2:
    case L3:
        app_state->selected_led = (app_state->selected_led - 1 + LED_COUNT) % LED_COUNT;
        break;
    case R1:
    case R2:
    case R3:
        app_state->selected_led = (app_state->selected_led + 1) % LED_COUNT;
        break;
    default:
        break;
    }
}

void handle_change_setting(AppState *app_state, int change)
{
    LedSettings *selected_led_settings = &app_state->led_settings[app_state->selected_led];
    switch (app_state->selected_setting)
    {
    case BRIGHTNESS:
    {
        // Bounds checking because looping around from max brightness to 0 brightness feels bad.
        selected_led_settings->brightness = clamp(selected_led_settings->brightness + (change * BRIGHTNESS_INCREMENT), 0, MAX_BRIGHTNESS);
        break;
    }
    case EFFECT:
        selected_led_settings->effect = clamp(selected_led_settings->effect + change, 0, ANIMATION_EFFECT_COUNT - 1);
        break;
    case DURATION:
        selected_led_settings->duration = clamp(selected_led_settings->duration + DURATION_INCREMENT * change, 0, MAX_DURATION);
        break;
    case COLOR:
    {
        // Find the current color index in bright_colors
        int current_color_index = -1;
        for (int i = 0; i < NUM_COLORS; i++)
        {
            if (bright_colors[i] == selected_led_settings->color)
            {
                current_color_index = i;
                break;
            }
        }

        // If the current color is not found, default to the first color
        if (current_color_index == -1)
        {
            current_color_index = 0;
        }

        // Update the color index based on the change
        current_color_index = (current_color_index + change + NUM_COLORS) % NUM_COLORS;

        // Set the new color
        selected_led_settings->color = bright_colors[current_color_index];
    }
    break;

    default:
        break;
    }
}

int initialize_additional_sdl_components(CoreSDLComponents *core_components, AdditionalSDLComponents *components)
{
    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(core_components->renderer);
        SDL_DestroyWindow(core_components->window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1)
    {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    // Load the background image and sprite sheet.
    SDL_Surface *backgroudImage = load_image("main_menu.png");
    components->brickSprite = load_image("brick_sprite_sheet.png");
    if (!backgroudImage || !components->brickSprite)
    {
        // Logging the error handled by load_image function.
        IMG_Quit();
        SDL_DestroyRenderer(core_components->renderer);
        SDL_DestroyWindow(core_components->window);
        SDL_Quit();
        return 1;
    }

    // Create a texture to render from the background image.
    components->backgroundTexture = SDL_CreateTextureFromSurface(core_components->renderer, backgroudImage);

    SDL_FreeSurface(backgroudImage);
    if (!components->backgroundTexture)
    {
        SDL_Log("Unable to create backgroundTexture! SDL_Error: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_DestroyRenderer(core_components->renderer);
        SDL_DestroyWindow(core_components->window);
        SDL_Quit();
        return 1;
    }

    // Load font
    components->font = TTF_OpenFont("assets/retro_gaming.ttf", 34); // Specify your font path
    if (!components->font)
    {
        SDL_Log("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    return 0;
}

int initialize_app_state(AppState *app_state)
{
    // Begin with first LED selected.
    app_state->selected_led = LED_FRONT;
    app_state->selected_setting = BRIGHTNESS;
    app_state->should_save_settings = false;
    app_state->should_quit = false;

    // Load LED settings from file.
    if (read_settings(app_state) != 0)
    {
        SDL_Log("Failed to read settings");
        return 1;
    }
    return 0;
}

void initialize_user_interface(UserInterface *user_interface, CoreSDLComponents *core_components, AdditionalSDLComponents *components)
{
    snprintf(user_interface->onscreen_log_message, sizeof(user_interface->onscreen_log_message), "Welcome to the LED Controller!");
    user_interface->logging_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->onscreen_log_message);
    user_interface->brightness_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->brightness_text);
    user_interface->effect_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->effect_text);
    user_interface->color_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->color_text);
    user_interface->duration_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->duration_text);
    user_interface->selected_led_texture = create_text_texture(core_components->renderer, components->font, user_interface->selected_led_text);
    user_interface->selected_option_texture = create_text_texture(core_components->renderer, components->font, user_interface->selected_option_text);
}

void free_user_interface(UserInterface *user_interface)
{
    if (user_interface == NULL)
    {
        return; // Nothing to clean up if user_interface is NULL
    }

    if (user_interface->logging_text_texture)
    {
        SDL_DestroyTexture(user_interface->logging_text_texture);
        user_interface->logging_text_texture = NULL;
    }
    if (user_interface->brightness_text_texture)
    {
        SDL_DestroyTexture(user_interface->brightness_text_texture);
        user_interface->brightness_text_texture = NULL;
    }
    if (user_interface->effect_text_texture)
    {
        SDL_DestroyTexture(user_interface->effect_text_texture);
        user_interface->effect_text_texture = NULL;
    }
    if (user_interface->color_text_texture)
    {
        SDL_DestroyTexture(user_interface->color_text_texture);
        user_interface->color_text_texture = NULL;
    }
    if (user_interface->duration_text_texture)
    {
        SDL_DestroyTexture(user_interface->duration_text_texture);
        user_interface->duration_text_texture = NULL;
    }
    if (user_interface->selected_led_texture)
    {
        SDL_DestroyTexture(user_interface->selected_led_texture);
        user_interface->selected_led_texture = NULL;
    }
    if (user_interface->selected_option_texture)
    {
        SDL_DestroyTexture(user_interface->selected_option_texture);
        user_interface->selected_option_texture = NULL;
    }
}

void update_user_interface_text(UserInterface *user_interface, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state)
{
    LedSettingOption selected_setting = app_state->selected_setting;
    snprintf(user_interface->brightness_text, sizeof(user_interface->brightness_text), "%sBrightness: %d", selected_setting == BRIGHTNESS ? MENU_CARRET : "", app_state->led_settings[app_state->selected_led].brightness);
    user_interface->brightness_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->brightness_text);

    snprintf(user_interface->effect_text, sizeof(user_interface->effect_text), "%sEffect:     %s", selected_setting == EFFECT ? MENU_CARRET : "", animation_effect_to_string(app_state->led_settings[app_state->selected_led].effect));
    user_interface->effect_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->effect_text);

    snprintf(user_interface->color_text, sizeof(user_interface->color_text), "%sColor:      0x%x", selected_setting == COLOR ? MENU_CARRET : "", app_state->led_settings[app_state->selected_led].color);
    user_interface->color_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->color_text);

    snprintf(user_interface->duration_text, sizeof(user_interface->duration_text), "%sDuration:  %dms", selected_setting == DURATION ? MENU_CARRET : "", app_state->led_settings[app_state->selected_led].duration);
    user_interface->duration_text_texture = create_text_texture(core_components->renderer, components->font, user_interface->duration_text);

    snprintf(user_interface->selected_led_text, sizeof(user_interface->selected_led_text), "[ L1 ] %s [ R1 ]", led_to_string(app_state->selected_led));
    user_interface->selected_led_texture = create_text_texture(core_components->renderer, components->font, user_interface->selected_led_text);

    snprintf(user_interface->selected_option_text, sizeof(user_interface->selected_option_text), "Selected Option: %s", led_setting_option_to_string(app_state->selected_setting));
    user_interface->selected_option_texture = create_text_texture(core_components->renderer, components->font, user_interface->selected_option_text);
}

int clamp(int value, int min, int max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

int read_settings(AppState *app_state)
{
    FILE *file = fopen(SETTINGS_FILE, "r");
    if (!file)
    {
        perror("fopen");
        SDL_Log("Failed to open %s for reading", SETTINGS_FILE);
        return 1;
    }

    char line[STRING_LENGTH];
    int led_index = -1;
    SDL_Log("Reading settings from %s ...", SETTINGS_FILE);

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "[LED_%d", &led_index) == 1)
        {
            continue;
        }
        if (led_index >= 0 && led_index < LED_COUNT)
        {
            sscanf(line, "brightness=%d", &app_state->led_settings[led_index].brightness);
            sscanf(line, "color=%x", &app_state->led_settings[led_index].color);
            sscanf(line, "duration=%d", &app_state->led_settings[led_index].duration);
            sscanf(line, "effect=%d", (int *)&app_state->led_settings[led_index].effect);

            // Bounds checking
            app_state->led_settings[led_index].brightness =
                clamp(app_state->led_settings[led_index].brightness, 0, MAX_BRIGHTNESS);
            app_state->led_settings[led_index].color =
                clamp(app_state->led_settings[led_index].color, 0, 0xFFFFFF);
            app_state->led_settings[led_index].duration =
                clamp(app_state->led_settings[led_index].duration, 0, MAX_DURATION);
            app_state->led_settings[led_index].effect =
                clamp(app_state->led_settings[led_index].effect, 0, ANIMATION_EFFECT_COUNT - 1);
        }
    }
    SDL_Log("Settings read successfully");

    fclose(file);
    return 0;
}

int save_settings(AppState *app_state)
{

    app_state->should_save_settings = false;
    FILE *file = fopen(SETTINGS_FILE, "w");
    if (!file)
    {
        perror("fopen");
        SDL_Log("Failed to open %s for writing", SETTINGS_FILE);
        return 1;
    }

    SDL_Log("Saving settings to %s ...", SETTINGS_FILE);
    for (int led_index = 0; led_index < LED_COUNT; led_index++)
    {
        fprintf(file, "[LED_%d_%s]\n", led_index, led_internal_name(led_index));
        fprintf(file, "brightness=%d\n", app_state->led_settings[led_index].brightness);
        fprintf(file, "color=0x%06X\n", app_state->led_settings[led_index].color);
        fprintf(file, "duration=%d\n", app_state->led_settings[led_index].duration);
        fprintf(file, "effect=%d\n\n", app_state->led_settings[led_index].effect);
    }
    SDL_Log("Settings saved successfully");

    fclose(file);
    return 0;
}

void initialize_animations(AnimationInfo *animations)
{
    // Define what frames to display for each LED option.
    static int front_frames[] = {0, 1};
    static int top_frames[] = {2, 3};
    static int back_frames[] = {2, 4};

    animations[LED_FRONT] = (AnimationInfo){front_frames, 2};
    animations[LED_TOP] = (AnimationInfo){top_frames, 2};
    animations[LED_BACK] = (AnimationInfo){back_frames, 2};
}

void render_brick_sprite(SDL_Renderer *renderer, SDL_Surface *sprite, int frame_index, int position_x, int position_y)
{
    // Offset the frame index by the width of the sprite to display the next frame
    SDL_Rect src_rect = {frame_index * BRICK_SPRITE_WIDTH, 0, BRICK_SPRITE_WIDTH, BRICK_SPRITE_HEIGHT};
    SDL_Rect dst_rect = {position_x, position_y, BRICK_SPRITE_WIDTH, BRICK_SPRITE_HEIGHT}; // Position the sprite at (100, 100)
    SDL_Texture *brick_texture = SDL_CreateTextureFromSurface(renderer, sprite);
    SDL_RenderCopy(renderer, brick_texture, &src_rect, &dst_rect);
    SDL_DestroyTexture(brick_texture);

    // Delay to control frame rate
    SDL_Delay(16); // Approximately 60 frames per second
}

void render_text_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y)
{
    int text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect dstrect = {x, y, text_width, text_height};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}

void render_user_interface(SDL_Renderer *renderer, UserInterface *user_interface, int start_x, int start_y)
{
    // x and y UI element spacing.
    const int x_offset = 20;
    const int y_offset = 50;
    render_text_texture(renderer, user_interface->selected_led_texture, start_x, start_y);
    render_text_texture(renderer, user_interface->brightness_text_texture, start_x + x_offset, start_y + y_offset * 2);
    render_text_texture(renderer, user_interface->effect_text_texture, start_x + x_offset, start_y + y_offset * 3);
    render_text_texture(renderer, user_interface->duration_text_texture, start_x + x_offset, start_y + y_offset * 4);
    render_text_texture(renderer, user_interface->color_text_texture, start_x + x_offset, start_y + y_offset * 5);
}

void update_animation_frame_index(Uint32 *last_frame_time_millis, int *brick_anim_frame_index, int frame_count)
{
    Uint32 current_time_millis = SDL_GetTicks();
    if (current_time_millis - *last_frame_time_millis >= BRICK_ANIM_FRAME_DELAY_MS)
    {
        *brick_anim_frame_index = (*brick_anim_frame_index + 1) % frame_count;
        *last_frame_time_millis = current_time_millis;
    }
}

SDL_Surface *load_image(char *image_name)
{
    char image_path[STRING_LENGTH];
    snprintf(image_path, sizeof(image_path), "%s/%s", IMAGE_DIR, image_name);
    SDL_Surface *surface = IMG_Load(image_path);
    if (!surface)
    {
        SDL_Log("Unable to load image %s ! IMG_Error: %s\n", image_name, IMG_GetError());
    }
    return surface;
}

SDL_Texture *create_text_texture(SDL_Renderer *renderer, TTF_Font *font, const char *text)
{
    // Define colors for shadow and main text
    SDL_Color shadow_color = {0, 0, 0, 128};     // Semi-transparent black for shadow
    SDL_Color text_color = {255, 239, 186, 255}; // Eggshell white color for main text

    // Render shadow surface
    SDL_Surface *shadow_surface = TTF_RenderText_Solid(font, text, shadow_color);
    if (!shadow_surface)
        return NULL;

    // Render main text surface
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, text_color);
    if (!text_surface)
    {
        SDL_FreeSurface(shadow_surface);
        return NULL;
    }

    // Create a larger surface to combine shadow and main text
    int shadow_offset = 4; // Offset for shadow
    int width = text_surface->w + shadow_offset;
    int height = text_surface->h + shadow_offset;
    SDL_Surface *combined_surface = SDL_CreateRGBSurface(0, width, height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    if (!combined_surface)
    {
        SDL_FreeSurface(shadow_surface);
        SDL_FreeSurface(text_surface);
        return NULL;
    }

    // Blit the shadow first (offset by shadow_offset)
    SDL_Rect shadow_rect = {shadow_offset, shadow_offset, shadow_surface->w, shadow_surface->h};
    SDL_BlitSurface(shadow_surface, NULL, combined_surface, &shadow_rect);

    // Blit the main text on top (without offset)
    SDL_Rect text_rect = {0, 0, text_surface->w, text_surface->h};
    SDL_BlitSurface(text_surface, NULL, combined_surface, &text_rect);

    // Create texture from combined surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, combined_surface);

    // Free the surfaces
    SDL_FreeSurface(shadow_surface);
    SDL_FreeSurface(text_surface);
    SDL_FreeSurface(combined_surface);

    return texture;
}

char *animation_effect_to_string(AnimationEffect effect)
{
    switch (effect)
    {
    case DISABLE:
        return "Disabled";
    case LINEAR:
        return "Linear";
    case BREATH:
        return "Breath";
    case SNIFF:
        return "Sniff";
    case STATIC:
        return "Static";
    case BLINK1:
        return "Blink 1";
    case BLINK2:
        return "Blink 2";
    case BLINK3:
        return "Blink 3";
    default:
        return "UNKNOWN";
    }
}

char *led_setting_option_to_string(LedSettingOption setting)
{
    switch (setting)
    {
    case BRIGHTNESS:
        return "Brightness: ";
    case EFFECT:
        return "Effect:     ";
    case COLOR:
        return "Color       ";
    case DURATION:
        return "Duration:   ";
    default:
        return "UNKNOWN     ";
    }
}

char *led_to_string(Led led)
{
    switch (led)
    {
    case LED_FRONT:
        return " Front LED ";
    case LED_TOP:
        return " Top LED ";
    case LED_BACK:
        return " Back LED ";
    default:
        return "UNKNOWN";
    }
}

char *led_internal_name(Led led)
{
    switch (led)
    {
    case LED_FRONT:
        return "f";
    case LED_TOP:
        return "m";
    case LED_BACK:
        return "b";
    default:
        return "UNKNOWN";
    }
}

void debug_log(char *message, bool verbose_logging_enabled)
{
    if (verbose_logging_enabled)
    {
        printf("%s\n", message);
    }
}

void write_max_scale_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix)
{
    snprintf(filepath, STRING_LENGTH, "%s/max_scale_%s", SYS_FILE_PATH, suffix);
    file = fopen(filepath, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", app_state->led_settings[led].brightness);
        fclose(file);
    }
    else
    {
        SDL_Log("Failed to open file: %s", filepath);
    }
}

void write_effect_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix)
{
    snprintf(filepath, STRING_LENGTH, "%s/effect_%s", SYS_FILE_PATH, suffix);
    SDL_Log("Opening file: %s", filepath);
    file = fopen(filepath, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", app_state->led_settings[led].effect);
        fclose(file);
    }
    else
    {
        SDL_Log("Failed to open file: %s", filepath);
    }
}

void write_effect_duration_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix)
{
    snprintf(filepath, STRING_LENGTH, "%s/effect_duration_%s", SYS_FILE_PATH, suffix);
    SDL_Log("Opening file: %s", filepath);
    file = fopen(filepath, "w");
    if (file != NULL)
    {
        fprintf(file, "%d\n", app_state->led_settings[led].duration);
        fclose(file);
    }
    else
    {
        SDL_Log("Failed to open file: %s", filepath);
    }
}

void write_color_data(FILE *file, const AppState *app_state, const Led led, char *filepath, const char *suffix)
{
    snprintf(filepath, STRING_LENGTH, "%s/effect_rgb_hex_%s", SYS_FILE_PATH, suffix);
    SDL_Log("Opening file: %s", filepath);
    file = fopen(filepath, "w");
    if (file != NULL)
    {

        fprintf(file, "%06X\n", app_state->led_settings[led].color);

        fclose(file);
    }
    else
    {
        SDL_Log("Failed to open file: %s", filepath);
    }
}

void update_leds(AppState *app_state)
{
    char filepath[STRING_LENGTH];
    FILE *file = NULL;

    const char *led_names[LED_COUNT] = {"f1f2", "m", "lr"};
    const char *front_led_name_spit[2] = {"f1", "f2"};

    app_state->should_update_leds = false;
    for (Led led = 0; led < LED_COUNT; led++)
    {
        const char *name = led_names[led];

        SDL_Log(">>> Updating LED %d: %s", led, led_to_string(led));

        // Update everything else
        if (led == LED_FRONT)
        {
            // Front LED filenames are split into f1 & f2
            for (int name_index = 0; name_index < 2; name_index++)
            {
                // Order for these write calls actually matters.
                // My theory is that the OS listens for changes on the effect data path and
                // updates updates everything but I haven't fully tested this.
                write_color_data(file, app_state, led, filepath, front_led_name_spit[name_index]);
                write_effect_duration_data(file, app_state, led, filepath, front_led_name_spit[name_index]);
                write_effect_data(file, app_state, led, filepath, front_led_name_spit[name_index]);
            }
        }
        else
        {
            write_color_data(file, app_state, led, filepath, name);
            write_effect_duration_data(file, app_state, led, filepath, name);
            write_effect_data(file, app_state, led, filepath, name);
        }

        // Update brightness - Consider updating max_scale generally(?)
        // Only FRONT and BACK have max_scale files
        if (led != LED_FRONT || led != LED_BACK)
        {
            write_max_scale_data(file, app_state, led, filepath, name);
        }
    }
}

int teardown(CoreSDLComponents *core_components, AdditionalSDLComponents *components, UserInterface *user_interface)
{
    free_user_interface(user_interface);
    free_sdl_core(core_components);
    SDL_DestroyTexture(components->backgroundTexture);
    TTF_CloseFont(components->font);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
