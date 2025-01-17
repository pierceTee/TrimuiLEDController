#include "led_controller.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    /* Handle program inputs */
    bool verbose_logging_enabled = argc > 1 && strcmp(argv[1], "-v") == 0;

    /* Core SDL components that every application (I write) requires. */
    CoreSDLComponents core_components;

    /* SDL components that this application (I write) requires beyond the core components. */
    AdditionalSDLComponents components;

    /* User state object, contains all relevant user selection information. */
    AppState app_state;

    /* The collection of user interface related objects. */
    SelectableMenuItems config_page_ui;

    /* The collection of user interface related objects. */
    SelectableMenuItems menu_page_ui;

    Sprite brick_sprite;
    /* The last user input converted from SDL_Event. */
    InputType user_input;

    /* SDL event object we read on loop. */
    SDL_Event event;

    /* Initialize onscreen logging message */

    /* Initialize Primary SDL components */
    core_components.window_width = WINDOW_WIDTH;
    core_components.window_height = WINDOW_HEIGHT;

    /* Initialize auxilliary data structures */
    initialize_app_state(&app_state);
    update_leds(&app_state);
    if (initialize_sdl_core(&core_components, WINDOW_TITLE) != 0 ||
        initialize_additional_sdl_components(&core_components, &components) != 0)
    {
        SDL_Log("Failed to initialize SDL, exiting...\n");
        return 1;
    }
    initialize_config_page_ui(&config_page_ui, &core_components, &components);
    initialize_menu_ui(&menu_page_ui, &core_components, &components, &app_state);

    update_config_page_ui_text(&config_page_ui, &core_components, &components, &app_state);
    update_menu_ui_text(&menu_page_ui, &core_components, &components, &app_state);

    /* Initialize sprites */
    initialize_brick_sprite(&brick_sprite, core_components.renderer);
    /* Render the background */
    SDL_RenderCopy(core_components.renderer, components.backgroundTexture, NULL, NULL);
    while (!app_state.should_quit)
    {
        /* Handle events */
        while (SDL_PollEvent(&event) != 0)
        {
            /* Ignore unsupported input events */
            if (!is_supported_input_event(event.type))
            {
                continue;
            }

            if (event.type == SDL_QUIT)
            {
                app_state.should_quit = true;
                break;
            }
            /* Take and process user input. */
            user_input = sdl_event_to_input_type(&event, false);

            handle_event_updates(&app_state, &core_components, &components, &config_page_ui, &menu_page_ui, user_input, event);
        }

        /* Call the render_frame function */
        render_frame(&app_state, &core_components, &components, &brick_sprite, &config_page_ui, &menu_page_ui, verbose_logging_enabled);
    }

    save_settings(&app_state);
    update_leds(&app_state);
    if (app_state.should_install_daemon)
    {
        install_daemon();
    }
    return teardown(&core_components, &components, &config_page_ui, &menu_page_ui, &brick_sprite);
}

void handle_user_input(InputType user_input, AppState *app_state, SDL_GameController *controller)
{
    switch (app_state->current_page)
    {
    case CONFIG_PAGE:
    {
        switch (user_input)
        {
        case POWER:
            /* power button safely exits app */
            app_state->should_quit = true;
            break;
        case START:
        case SELECT:
            app_state->current_page = MENU_PAGE;
            break;
            break;
        case A:
            handle_change_setting(app_state, 0);
            app_state->should_update_leds = true;
            break;
        case B:
            switch (app_state->current_page)
            {
            case MENU_PAGE:
                /* close menu */
                app_state->current_page = CONFIG_PAGE;
                break;
            case CONFIG_PAGE:
                /* exit app */
                app_state->should_quit = true;
                break;
            }
            break;
        case DPAD_UP:
            /* Switch between settings */
            app_state->selected_setting = (app_state->selected_setting - 1 + LED_SETTINGS_COUNT) % LED_SETTINGS_COUNT;
            break;
        case DPAD_DOWN:
            /* Switch between settings */
            app_state->selected_setting = (app_state->selected_setting + 1 + LED_SETTINGS_COUNT) % LED_SETTINGS_COUNT;
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
    break;
    case MENU_PAGE:
    {
        switch (user_input)
        {
        case START:
        case SELECT:
        case B:
            app_state->current_page = CONFIG_PAGE;
            break;
        case A:
            handle_menu_select(app_state, app_state->selected_menu_option);
            break;
        case DPAD_UP:
            /* Switch between settings */
            app_state->selected_menu_option = (app_state->selected_menu_option - 1 + MENU_OPTION_COUNT) % MENU_OPTION_COUNT;
            break;
        case DPAD_DOWN:
            /* Switch between settings */
            app_state->selected_menu_option = (app_state->selected_menu_option + 1 + MENU_OPTION_COUNT) % MENU_OPTION_COUNT;
            break;
        default:
            break;
        }
    }
    break;
    }
}
void handle_change_setting(AppState *app_state, int change)
{
    LedSettings *selected_led_settings = &app_state->led_settings[app_state->selected_led];
    switch (app_state->selected_setting)
    {
    case SELECTED_LED:
    {
        app_state->selected_led = (app_state->selected_led - change + LED_COUNT) % LED_COUNT;
        break;
    }
    case BRIGHTNESS:
    {
        /* Bounds checking because looping around from max brightness to 0 brightness feels bad. */
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
        if (app_state->are_extended_colors_enabled)
        {
            selected_led_settings->color = next_color(selected_led_settings->color, change);
        }
        else
        {
            /* Find the current color index in colors */
            int current_color_index = -1;
            for (int i = 0; i < num_color; i++)
            {
                if (colors[i] == selected_led_settings->color)
                {
                    current_color_index = i;
                    break;
                }
            }

            /* If the current color is not found, default to the first color */
            if (current_color_index == -1)
            {
                current_color_index = 0;
            }

            /* Update the color index based on the change */
            current_color_index = (current_color_index + change + num_color) % num_color;

            /* Set the new color */
            selected_led_settings->color = colors[current_color_index];
        }
    }
    break;
    case MATCH_SETTINGS:
        /* Change was from the user selecting the A button*/
        if (change == 0)
        {
            color_match_leds(app_state);
        }
        break;
    default:
        break;
    }
}

void handle_menu_select(AppState *app_state, MenuOption selected_menu_option)
{
    switch (selected_menu_option)
    {
    case ENABLE_ALL:
        turn_on_all_leds(app_state);
        break;
    case DISABLE_ALL:
        turn_off_all_leds(app_state);
        break;
    case TOGGLE_EXTENDED_COLORS:
        app_state->are_extended_colors_enabled = !app_state->are_extended_colors_enabled;
        break;
    case UNINSTALL:
        uninstall_daemon();
        turn_off_all_leds(app_state);
        app_state->should_install_daemon = false;
        app_state->should_quit = true;
        break;
    case QUIT:
        app_state->should_quit = true;
        break;
    default:
        break;
    }
}

void handle_event_updates(AppState *app_state, CoreSDLComponents *core_components, AdditionalSDLComponents *components,
                          SelectableMenuItems *config_page_ui, SelectableMenuItems *menu_page_ui,
                          InputType user_input, SDL_Event event)
{
    handle_user_input(user_input, app_state, core_components->controller);

    if (is_supported_input_event(event.type))
    {
        if (app_state->should_update_leds)
        {
            update_leds(app_state);
        }
        if (app_state->should_save_settings)
        {
            save_settings(app_state);
        }
        update_config_page_ui_text(config_page_ui, core_components, components, app_state);
        update_menu_ui_text(menu_page_ui, core_components, components, app_state);
    }
}
int initialize_additional_sdl_components(CoreSDLComponents *core_components, AdditionalSDLComponents *components)
{
    /* Initialize SDL_image */
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(core_components->renderer);
        SDL_DestroyWindow(core_components->window);
        SDL_Quit();
        return 1;
    }

    /* Initialize SDL_ttf */
    if (TTF_Init() == -1)
    {
        SDL_Log("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    /* Create a texture to render from the background image. */
    components->backgroundTexture = create_sdl_texture_from_image(core_components->renderer, BACKGROUND_IMAGE_PATH);

    components->menuTexture = create_sdl_texture_from_image(core_components->renderer, MENU_IMAGE_PATH);

    if (!components->backgroundTexture || !components->menuTexture)
    {

        return 1;
    }

    /* Load font */
    components->font = TTF_OpenFont("assets/retro_gaming.ttf", 32); /* Specify your font path */
    if (!components->font)
    {
        SDL_Log("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        return 1;
    }

    return 0;
}

int initialize_app_state(AppState *app_state)
{
    /* Begin with first LED selected. */
    app_state->selected_led = LED_FRONT;
    app_state->selected_setting = BRIGHTNESS;
    app_state->should_save_settings = false;
    app_state->should_quit = false;
    app_state->should_install_daemon = true;
    app_state->are_extended_colors_enabled = false;
    app_state->current_page = CONFIG_PAGE;
    app_state->selected_menu_option = ENABLE_ALL;

    /* Load LED settings from file. */
    if (read_settings(app_state) != 0)
    {
        SDL_Log("Failed to read settings");
        return 1;
    }
    return 0;
}

void initialize_config_page_ui(SelectableMenuItems *menu_items, CoreSDLComponents *core_components, AdditionalSDLComponents *components)
{
    menu_items->text_color = (SDL_Color){255, 239, 186, 255};
    menu_items->text_shadow_color = (SDL_Color){0, 0, 0, 128};
    menu_items->text_highlight_color = (SDL_Color){255, 173, 99, 255};
    menu_items->item_count = LED_SETTINGS_COUNT;
    menu_items->string_length = STRING_LENGTH;
    menu_items->menu_text = malloc(menu_items->item_count * sizeof(char *));
    menu_items->menu_text_textures = malloc(menu_items->item_count * sizeof(SDL_Texture *));

    for (int setting_index = 0; setting_index < menu_items->item_count; setting_index++)
    {
        menu_items->menu_text[setting_index] = malloc(menu_items->string_length * sizeof(char));
        /* preload default strings */
        snprintf(menu_items->menu_text[setting_index], menu_items->string_length, "%s", led_setting_option_to_string(setting_index));

        /* create text textures from strings */
        menu_items->menu_text_textures[setting_index] = create_text_texture(core_components->renderer, components->font, &menu_items->text_color, &menu_items->text_shadow_color, menu_items->menu_text[setting_index]);
    }
}

void initialize_menu_ui(SelectableMenuItems *menu_items, CoreSDLComponents *core_components, AdditionalSDLComponents *components, AppState *app_state)
{
    menu_items->text_color = (SDL_Color){255, 239, 186, 255};
    menu_items->text_shadow_color = (SDL_Color){0, 0, 0, 128};
    menu_items->text_highlight_color = (SDL_Color){255, 173, 99, 255};
    menu_items->item_count = MENU_OPTION_COUNT;
    menu_items->string_length = STRING_LENGTH;
    menu_items->menu_text = malloc(menu_items->item_count * sizeof(char *));
    menu_items->menu_text_textures = malloc(menu_items->item_count * sizeof(SDL_Texture *));

    for (int setting_index = 0; setting_index < menu_items->item_count; setting_index++)
    {
        menu_items->menu_text[setting_index] = malloc(menu_items->string_length * sizeof(char));
        /* preload default strings */
        snprintf(menu_items->menu_text[setting_index], menu_items->string_length, "%s", menu_option_to_string(setting_index, app_state));

        /* create text textures from strings */
        menu_items->menu_text_textures[setting_index] = create_text_texture(core_components->renderer, components->font, &menu_items->text_color, &menu_items->text_shadow_color, menu_items->menu_text[setting_index]);
    }
}

void free_menu_items(SelectableMenuItems *menu_items)
{
    if (!menu_items)
        return;
    if (menu_items->menu_text_textures)
    {
        for (int item_index = 0; item_index < menu_items->item_count; item_index++)
        {
            if (menu_items->menu_text_textures[item_index])
            {
                SDL_DestroyTexture(menu_items->menu_text_textures[item_index]);
            }
        }
        free(menu_items->menu_text_textures);
    }
    if (menu_items->menu_text)
    {
        for (int item_index = 0; item_index < menu_items->item_count; item_index++)
        {
            if (menu_items->menu_text[item_index])
            {
                free(menu_items->menu_text[item_index]);
            }
        }
        free(menu_items->menu_text);
    }
}

void update_config_page_ui_text(SelectableMenuItems *menu_items, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state)
{
    LedSettingOption selected_setting = app_state->selected_setting;
    for (LedSettingOption setting_index = 0; setting_index < LED_SETTINGS_COUNT; setting_index++)
    {
        switch (setting_index)
        {
        case SELECTED_LED:
            snprintf(menu_items->menu_text[setting_index], menu_items->string_length, "%sLED: %s%s",
                     selected_setting == SELECTED_LED ? MENU_CARRET_LEFT : "",
                     led_to_string(app_state->selected_led),
                     selected_setting == SELECTED_LED ? MENU_CARRET_RIGHT : "");
            break;
        case BRIGHTNESS:
            /* Write menu text to string */
            snprintf(menu_items->menu_text[setting_index],
                     menu_items->string_length, "%sBrightness: %d%s", selected_setting == BRIGHTNESS ? MENU_CARRET_LEFT : "",
                     app_state->led_settings[app_state->selected_led].brightness / BRIGHTNESS_INCREMENT,
                     selected_setting == BRIGHTNESS ? MENU_CARRET_RIGHT : "");
            break;
        case EFFECT:
            snprintf(menu_items->menu_text[setting_index],
                     menu_items->string_length,
                     "%sEffect:     %s%s",
                     selected_setting == EFFECT ? MENU_CARRET_LEFT : "",
                     animation_effect_to_string(app_state->led_settings[app_state->selected_led].effect),
                     selected_setting == EFFECT ? MENU_CARRET_RIGHT : "");
            break;
        case COLOR:
            snprintf(menu_items->menu_text[setting_index],
                     menu_items->string_length, "%sColor:   %s%s",
                     selected_setting == COLOR ? MENU_CARRET_LEFT : "",
                     color_to_string(app_state->led_settings[app_state->selected_led].color),
                     selected_setting == COLOR ? MENU_CARRET_RIGHT : "");
            break;
        case DURATION:
            snprintf(menu_items->menu_text[setting_index],
                     menu_items->string_length,
                     "%sDuration: %dms%s",
                     selected_setting == DURATION ? MENU_CARRET_LEFT : "",
                     app_state->led_settings[app_state->selected_led].duration,
                     selected_setting == DURATION ? MENU_CARRET_RIGHT : "");
            break;
        case MATCH_SETTINGS:
            snprintf(menu_items->menu_text[setting_index],
                     menu_items->string_length,
                     "Sync LED colors");
            break;
        }

        /* Create texture from string */
        menu_items->menu_text_textures[setting_index] = create_text_texture(core_components->renderer, components->font, selected_setting == setting_index ? &menu_items->text_highlight_color : &menu_items->text_color, &menu_items->text_shadow_color, menu_items->menu_text[setting_index]);
    }
}

void update_menu_ui_text(SelectableMenuItems *menu_items, const CoreSDLComponents *core_components, const AdditionalSDLComponents *components, const AppState *app_state)
{
    MenuOption selected_menu_option = app_state->selected_menu_option;

    for (int menu_index = 0; menu_index < menu_items->item_count; menu_index++)
    {
        snprintf(menu_items->menu_text[menu_index], menu_items->string_length, "%s%s", selected_menu_option == menu_index ? ">>> " : "",
                 menu_option_to_string(menu_index, app_state));

        menu_items->menu_text_textures[menu_index] = create_text_texture(core_components->renderer, components->font, selected_menu_option == menu_index ? &menu_items->text_highlight_color : &menu_items->text_color, &menu_items->text_shadow_color, menu_items->menu_text[menu_index]);
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
        char led_name[STRING_LENGTH];
        if (sscanf(line, "[%[^]]]", led_name) == 1)
        {
            led_index = internal_led_name_to_led(led_name);
            continue;
        }
        if (led_index >= 0 && led_index < LED_COUNT)
        {
            sscanf(line, "brightness=%d", &app_state->led_settings[led_index].brightness);
            sscanf(line, "color=%x", &app_state->led_settings[led_index].color);
            sscanf(line, "duration=%d", &app_state->led_settings[led_index].duration);
            sscanf(line, "effect=%d", (int *)&app_state->led_settings[led_index].effect);

            /* Bounds checking */
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
        fprintf(file, "[%s]\n", led_internal_name(led_index));
        fprintf(file, "brightness=%d\n", app_state->led_settings[led_index].brightness);
        fprintf(file, "color=0x%06X\n", app_state->led_settings[led_index].color);
        fprintf(file, "duration=%d\n", app_state->led_settings[led_index].duration);
        fprintf(file, "effect=%d\n\n", app_state->led_settings[led_index].effect);
    }
    SDL_Log("Settings saved successfully");

    fclose(file);
    return 0;
}

int initialize_brick_sprite(Sprite *brick_sprite, SDL_Renderer *renderer)
{
    /* Load the brick sprite image to a texture */

    brick_sprite->sprite_texture = create_sdl_texture_from_image(renderer, BRICK_SPRITE_SHEET_PATH);
    if (!brick_sprite->sprite_texture)
    {
        SDL_Log("Failed to load brick sprite texture from %s", BRICK_SPRITE_SHEET_PATH);
        return 1;
    }

    /* We have one animation for each LED. */
    brick_sprite->animations = (AnimationInfo *)malloc(sizeof(AnimationInfo) * LED_COUNT);
    brick_sprite->animation_count = LED_COUNT;
    brick_sprite->sprite_width = BRICK_SPRITE_WIDTH;
    brick_sprite->sprite_height = BRICK_SPRITE_HEIGHT;
    brick_sprite->current_animation_index = 0;

    /* Define what frames to display for each LED option. */
    static int front_frames[] = {0, 1, 2, 3, 2, 1, 0};
    static int top_frames[] = {4, 5, 6, 8, 6, 5, 4};
    static int back_frames[] = {4, 5, 6, 7, 6, 5, 4};
    static int frame_durations[] = {150, 150, 150, 500, 150, 150, 150};

    brick_sprite->animations[LED_FRONT] = (AnimationInfo){front_frames, frame_durations, 7, 0, 0};
    brick_sprite->animations[LED_TOP] = (AnimationInfo){top_frames, frame_durations, 7, 0, 0};
    brick_sprite->animations[LED_BACK] = (AnimationInfo){back_frames, frame_durations, 7, 0, 0};
    return 0;
}

void render_frame(AppState *app_state, CoreSDLComponents *core_components, AdditionalSDLComponents *components, Sprite *brick_sprite, SelectableMenuItems *config_page_ui, SelectableMenuItems *menu_page_ui, bool verbose_logging_enabled)
{
    /* Clear screen */
    SDL_RenderClear(core_components->renderer);

    /* Render background texture */
    SDL_RenderCopy(core_components->renderer, components->backgroundTexture, NULL, NULL);

    /* Render the user-selected color in front of a black square */
    render_colored_square(app_state, core_components);

    char log_message[STRING_LENGTH];
    snprintf(log_message, sizeof(log_message), "Selected LED: %d\n", app_state->selected_led);
    debug_log(log_message, verbose_logging_enabled);

    /* Render brick sprite */
    brick_sprite->current_animation_index = app_state->selected_led;
    update_sprite_render(core_components->renderer, brick_sprite, 10, 99);

    /* Render the interactable user interface. */
    render_menu_items(core_components->renderer, config_page_ui, 550, 150);

    if (app_state->current_page == MENU_PAGE)
    { /* Render menu last on stack if it needs to show*/
        SDL_RenderCopy(core_components->renderer, components->menuTexture, NULL, NULL);
        render_menu_items(core_components->renderer, menu_page_ui, 250, 250);
    }
    /* Main render call to update screen */
    SDL_RenderPresent(core_components->renderer);

    /* Delay to control frame rate Approximately 60 frames per second */
    SDL_Delay(16);
}

void render_colored_square(AppState *app_state, CoreSDLComponents *core_components)
{
    /* Adjusted to add 4 pixels on top */
    SDL_Rect black_rect = {20, 96, BRICK_SPRITE_WIDTH, BRICK_SPRITE_HEIGHT + 4};
    /* Black color */
    SDL_SetRenderDrawColor(core_components->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(core_components->renderer, &black_rect);
    /* Adjusted to match the new black_rect */

    SDL_Rect color_rect = {25, 101, BRICK_SPRITE_WIDTH - 10, BRICK_SPRITE_HEIGHT - 6};
    int32_t color = app_state->led_settings[app_state->selected_led].color;
    SDL_SetRenderDrawColor(core_components->renderer, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF, 255);
    SDL_RenderFillRect(core_components->renderer, &color_rect);
}

void render_text_texture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y)
{
    int text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect dstrect = {x, y, text_width, text_height};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
}

void render_menu_items(SDL_Renderer *renderer, SelectableMenuItems *menu_items, int start_x, int start_y)
{
    /* x and y UI element spacing. */
    const int x_offset = 20;
    const int y_offset = 50;
    for (int item_index = 0; item_index < menu_items->item_count; item_index++)
    {
        render_text_texture(renderer, menu_items->menu_text_textures[item_index], start_x + x_offset, start_y + y_offset * item_index);
    }
}

SDL_Surface *load_image(const char *image_name)
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

void write_max_scale_data(FILE *file, const AppState *app_state, const Led led, char *filepath)
{
    if (led == LED_TOP)
    {
        snprintf(filepath, STRING_LENGTH, "%s/max_scale", SYS_FILE_PATH);
    }
    else
    {
        snprintf(filepath, STRING_LENGTH, "%s/max_scale_%s", SYS_FILE_PATH, led_internal_name(led));
    }

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

void write_effect_data(FILE *file, const AppState *app_state, const Led led, char *filepath)
{
    const char *led_suffix[1] = {led_internal_name(led)};
    const char **suffix_array = (led == LED_FRONT) ? front_led_suffix : led_suffix;

    for (int suffix_index = 0; suffix_index < (led == LED_FRONT ? 2 : 1); suffix_index++)
    {
        snprintf(filepath, STRING_LENGTH, "%s/effect_%s", SYS_FILE_PATH, suffix_array[suffix_index]);
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
}

void write_effect_duration_data(FILE *file, const AppState *app_state, const Led led, char *filepath)
{
    const char *led_suffix[1] = {led_internal_name(led)};
    const char **suffix_array = (led == LED_FRONT) ? front_led_suffix : led_suffix;

    for (int suffix_index = 0; suffix_index < (led == LED_FRONT ? 2 : 1); suffix_index++)
    {
        snprintf(filepath, STRING_LENGTH, "%s/effect_duration_%s", SYS_FILE_PATH, suffix_array[suffix_index]);
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
}

void write_color_data(FILE *file, const AppState *app_state, const Led led, char *filepath)
{
    const char *led_suffix[1] = {led_internal_name(led)};
    const char **suffix_array = (led == LED_FRONT) ? front_led_suffix : led_suffix;

    for (int suffix_index = 0; suffix_index < (led == LED_FRONT ? 2 : 1); suffix_index++)
    {
        snprintf(filepath, STRING_LENGTH, "%s/effect_rgb_hex_%s", SYS_FILE_PATH, suffix_array[suffix_index]);
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
}

void update_leds(AppState *app_state)
{
    char filepath[STRING_LENGTH];
    FILE *file = NULL;

    app_state->should_update_leds = false;
    for (Led led = 0; led < LED_COUNT; led++)
    {
        write_max_scale_data(file, app_state, led, filepath);
        write_color_data(file, app_state, led, filepath);
        write_effect_duration_data(file, app_state, led, filepath);
        write_effect_data(file, app_state, led, filepath);
    }
}

void install_daemon()
{
    system("sh scripts/install.sh");
}

void uninstall_daemon()
{
    system("sh scripts/uninstall.sh");
}

void color_match_leds(AppState *app_state)
{
    for (Led led = 0; led < LED_COUNT; led++)
    {
        app_state->led_settings[led].color = app_state->led_settings[app_state->selected_led].color;
    }
}

void turn_off_all_leds(AppState *app_state)
{
    for (Led led = 0; led < LED_COUNT; led++)
    {
        app_state->led_settings[led].brightness = 0;
        app_state->led_settings[led].effect = DISABLE;
    }
    update_leds(app_state);
    system("sh scripts/turn_off_all_leds.sh");
}

void turn_on_all_leds(AppState *app_state)
{
    for (Led led = 0; led < LED_COUNT; led++)
    {
        app_state->led_settings[led].brightness = MAX_BRIGHTNESS;
        app_state->led_settings[led].effect = STATIC;
    }
    update_leds(app_state);
    system("sh scripts/turn_on_all_leds.sh");
}

int teardown(CoreSDLComponents *core_components, AdditionalSDLComponents *components,
             SelectableMenuItems *config_menu_items, SelectableMenuItems *main_menu_items, Sprite *brick_sprite)
{
    free_menu_items(config_menu_items);
    free_menu_items(main_menu_items);
    free_sprite(brick_sprite);
    free_sdl_core(core_components);
    SDL_DestroyTexture(components->backgroundTexture);
    SDL_DestroyTexture(components->menuTexture);
    TTF_CloseFont(components->font);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
