#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

void render(SDL_Renderer *renderer, SDL_Texture *texture)
{
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255); // Sky blue background
    SDL_RenderClear(renderer);

    // Get texture width and height
    int texture_width, texture_height;
    SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

    // Calculate centered position
    SDL_Rect dstrect = {
        (WINDOW_WIDTH - texture_width) / 2,
        (WINDOW_HEIGHT - texture_height) / 2,
        texture_width,
        texture_height};

    // Render texture
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);
    SDL_Delay(16); // Cap at roughly 60 FPS
}

void handle_input(bool *running)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            *running = false;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            // Quit on any key press
            *running = false;
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *log_file = fopen("test.log", "w");
    if (!log_file)
    {
        fprintf(stderr, "Failed to open log file\n");
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
    {
        fprintf(log_file, "SDL initialization failed: %s\n", SDL_GetError());
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "SDL initialized successfully\n");

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        fprintf(log_file, "SDL_image initialization failed: %s\n", IMG_GetError());
        SDL_Quit();
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "SDL_image initialized successfully\n");

    SDL_Window *window = SDL_CreateWindow(
        "Main Menu",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(log_file, "Window creation failed: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "Window created successfully\n");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        fprintf(log_file, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "Renderer created successfully\n");

    SDL_Surface *surface = IMG_Load("menu_images/main_menu.png"); // Load your image
    if (!surface)
    {
        fprintf(log_file, "Image loading failed: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "Image loaded successfully\n");

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture)
    {
        fprintf(log_file, "Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        fclose(log_file);
        return 1;
    }
    fprintf(log_file, "Texture created successfully\n");

    SDL_GameController *controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if (SDL_IsGameController(i))
        {
            controller = SDL_GameControllerOpen(i);
            if (controller)
            {
                fprintf(log_file, "Game Controller %s connected\n", SDL_GameControllerName(controller));
                break;
            }
        }
    }

    if (!controller)
    {
        fprintf(log_file, "No game controller available\n");
    }

    bool running = true;
    while (running)
    {
        handle_input(&running);
        render(renderer, texture);
    }

    if (controller)
    {
        SDL_GameControllerClose(controller);
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    fprintf(log_file, "SDL cleaned up and exited successfully\n");
    fclose(log_file);
    return 0;
}
