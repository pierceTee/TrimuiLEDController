#include "sdl_base.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

int initialize_sdl_core(CoreSDLComponents *core_components, char *window_title)
{
    if (window_title == NULL)
    {
        window_title = "";
    }
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize window
    core_components->window = SDL_CreateWindow(window_title,
                                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                               core_components->window_width, core_components->window_height,
                                               SDL_WINDOW_SHOWN);
    if (!core_components->window)
    {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize renderer
    core_components->renderer = SDL_CreateRenderer(core_components->window, -1, SDL_RENDERER_ACCELERATED);
    if (!core_components->renderer)
    {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(core_components->window);
        SDL_Quit();
        return 1;
    }

    // Initialize controller
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    core_components->controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        if (SDL_IsGameController(i))
        {
            core_components->controller = SDL_GameControllerOpen(i);
            if (core_components->controller)
            {
                SDL_Log("Game Controller %s connected", SDL_GameControllerName(core_components->controller));
                break;
            }
        }
    }

    if (!core_components->controller)
    {
        SDL_Log("No game controller available");
    }

    // Clear screen
    SDL_RenderClear(core_components->renderer);

    return 0;
}

void free_sdl_core(CoreSDLComponents *core_components)
{
    if (core_components->controller)
    {
        SDL_GameControllerClose(core_components->controller);
    }
    if (core_components->renderer)
    {
        SDL_DestroyRenderer(core_components->renderer);
    }
    if (core_components->window)
    {
        SDL_DestroyWindow(core_components->window);
    }
}

InputType sdl_event_to_input_type(SDL_Event *event, bool verbose)
{
    if (is_supported_input_event(event->type))
    {
        // Handle keyboard events
        if (event->type == SDL_KEYDOWN)
        {
            if (verbose)
            {
                SDL_Log("Keydown event: %d|0x%x \n", event->key.keysym.sym, event->key.keysym.sym);
            }
            switch (event->key.keysym.sym)
            {
            case SDLK_POWER:
                return POWER;
            case SDLK_ESCAPE:
                return START;
            case SDLK_UP:
            case SDLK_w:
                return DPAD_UP;
            case SDLK_LEFT:
            case SDLK_a:
                return DPAD_LEFT;
            case SDLK_DOWN:
            case SDLK_s:
                return DPAD_DOWN;
            case SDLK_RIGHT:
            case SDLK_d:
                return DPAD_RIGHT;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
            case SDLK_SPACE:
            case SDLK_x:
                return A;
            case SDLK_BACKSPACE:
            case SDLK_DELETE:
            case SDLK_z:
                return B;
            case SDLK_c:
                return X;
            case SDLK_v:
                return Y;
            case SDLK_TAB:
                return SELECT;
            case SDLK_PERIOD:
                return R1;
            case SDLK_COMMA:
                return L1;
            case SDLK_k:
                return L2;
            case SDLK_l:
                return R2;
            default:
                return UNKNOWN;
            }
        }
        else if (event->type == SDL_MOUSEBUTTONDOWN)
        {
            if (verbose)
            {
                SDL_Log("Mouse Button down event: %d|0x%x \n", event->button.button, event->button.button);
            }

            switch (event->button.button)
            {
            case SDL_BUTTON_LEFT:
                return A;
            case SDL_BUTTON_RIGHT:
                return B;
            default:
                return UNKNOWN;
            }
        }
        else if (event->type == SDL_CONTROLLERBUTTONDOWN)
        {
            if (verbose)
            {
                SDL_Log("Controller button down event: %d|0x%x \n", event->cbutton.button, event->cbutton.button);
            }
            switch (event->cbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_A: // Trimui Brick buttons are backwards :(
                return B;
            case SDL_CONTROLLER_BUTTON_B: // Trimui Brick buttons are backwards :(
                return A;
            case SDL_CONTROLLER_BUTTON_X:
                return Y;
            case SDL_CONTROLLER_BUTTON_Y: // Trimui Brick buttons are backwards :(
                return X;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                return DPAD_UP;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                return DPAD_DOWN;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                return DPAD_LEFT;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                return DPAD_RIGHT;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                return L1;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                return R1;
            case SDL_CONTROLLER_BUTTON_LEFTSTICK:
                return L3;
            case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
                return R3;
            case SDL_CONTROLLER_BUTTON_START:
                return START;
            case SDL_CONTROLLER_BUTTON_BACK:
                return SELECT;
            case SDL_CONTROLLER_BUTTON_GUIDE:
                return MENU;
            default:
                return UNKNOWN;
            }
        }
    }
    return UNKNOWN;
};

bool is_supported_input_event(Uint32 event_type)
{
    const Uint32 supported_events[] = {
        SDL_KEYDOWN,
        SDL_MOUSEBUTTONDOWN,
        SDL_CONTROLLERBUTTONDOWN,
        // TODO: Add control stick support
        // SDL_JOYAXISMOTION,
        // SDL_JOYBALLMOTION,
        // SDL_JOYHATMOTION,
        // SDL_JOYBUTTONDOWN,
        // SDL_JOYBUTTONUP,
        SDL_QUIT};
    const int num_supported_events = sizeof(supported_events) / sizeof(supported_events[0]);

    for (int i = 0; i < num_supported_events; ++i)
    {
        if (event_type == supported_events[i])
        {
            return true;
        }
    }
    return false;
}

const char *get_input_type_name(Uint32 inputType)
{
    switch (inputType)
    {
    case START:
        return "START";
    case SELECT:
        return "SELECT";
    case MENU:
        return "MENU";
    case POWER:
        return "POWER";
    case DPAD_LEFT:
        return "DPAD_LEFT";
    case DPAD_RIGHT:
        return "DPAD_RIGHT";
    case DPAD_UP:
        return "DPAD_UP";
    case DPAD_DOWN:
        return "DPAD_DOWN";
    case A:
        return "A";
    case B:
        return "B";
    case X:
        return "X";
    case Y:
        return "Y";
    case L1:
        return "L1";
    case R1:
        return "R1";
    case L2:
        return "L2";
    case R2:
        return "R2";
    case L3:
        return "L3";
    case R3:
        return "R3";
    default:
        return "Unknown input type";
    }
}

const char *get_event_name(Uint32 eventType)
{
    switch (eventType)
    {
    case SDL_QUIT:
        return "SDL_QUIT";
    case SDL_APP_TERMINATING:
        return "SDL_APP_TERMINATING";
    case SDL_APP_LOWMEMORY:
        return "SDL_APP_LOWMEMORY";
    case SDL_APP_WILLENTERBACKGROUND:
        return "SDL_APP_WILLENTERBACKGROUND";
    case SDL_APP_DIDENTERBACKGROUND:
        return "SDL_APP_DIDENTERBACKGROUND";
    case SDL_APP_WILLENTERFOREGROUND:
        return "SDL_APP_WILLENTERFOREGROUND";
    case SDL_APP_DIDENTERFOREGROUND:
        return "SDL_APP_DIDENTERFOREGROUND";
    case SDL_DISPLAYEVENT:
        return "SDL_DISPLAYEVENT";
    case SDL_WINDOWEVENT:
        return "SDL_WINDOWEVENT";
    case SDL_SYSWMEVENT:
        return "SDL_SYSWMEVENT";
    case SDL_KEYDOWN:
        return "SDL_KEYDOWN";
    case SDL_KEYUP:
        return "SDL_KEYUP";
    case SDL_TEXTEDITING:
        return "SDL_TEXTEDITING";
    case SDL_TEXTINPUT:
        return "SDL_TEXTINPUT";
    case SDL_KEYMAPCHANGED:
        return "SDL_KEYMAPCHANGED";
    case SDL_MOUSEMOTION:
        return "SDL_MOUSEMOTION";
    case SDL_MOUSEBUTTONDOWN:
        return "SDL_MOUSEBUTTONDOWN";
    case SDL_MOUSEBUTTONUP:
        return "SDL_MOUSEBUTTONUP";
    case SDL_MOUSEWHEEL:
        return "SDL_MOUSEWHEEL";
    case SDL_JOYAXISMOTION:
        return "SDL_JOYAXISMOTION";
    case SDL_JOYBALLMOTION:
        return "SDL_JOYBALLMOTION";
    case SDL_JOYHATMOTION:
        return "SDL_JOYHATMOTION";
    case SDL_JOYBUTTONDOWN:
        return "SDL_JOYBUTTONDOWN";
    case SDL_JOYBUTTONUP:
        return "SDL_JOYBUTTONUP";
    case SDL_JOYDEVICEADDED:
        return "SDL_JOYDEVICEADDED";
    case SDL_JOYDEVICEREMOVED:
        return "SDL_JOYDEVICEREMOVED";
    case SDL_CONTROLLERAXISMOTION:
        return "SDL_CONTROLLERAXISMOTION";
    case SDL_CONTROLLERBUTTONDOWN:
        return "SDL_CONTROLLERBUTTONDOWN";
    case SDL_CONTROLLERBUTTONUP:
        return "SDL_CONTROLLERBUTTONUP";
    case SDL_CONTROLLERDEVICEADDED:
        return "SDL_CONTROLLERDEVICEADDED";
    case SDL_CONTROLLERDEVICEREMOVED:
        return "SDL_CONTROLLERDEVICEREMOVED";
    case SDL_CONTROLLERDEVICEREMAPPED:
        return "SDL_CONTROLLERDEVICEREMAPPED";
    case SDL_FINGERDOWN:
        return "SDL_FINGERDOWN";
    case SDL_FINGERUP:
        return "SDL_FINGERUP";
    case SDL_FINGERMOTION:
        return "SDL_FINGERMOTION";
    case SDL_DOLLARGESTURE:
        return "SDL_DOLLARGESTURE";
    case SDL_DOLLARRECORD:
        return "SDL_DOLLARRECORD";
    case SDL_MULTIGESTURE:
        return "SDL_MULTIGESTURE";
    case SDL_CLIPBOARDUPDATE:
        return "SDL_CLIPBOARDUPDATE";
    case SDL_DROPFILE:
        return "SDL_DROPFILE";
    case SDL_DROPTEXT:
        return "SDL_DROPTEXT";
    case SDL_DROPBEGIN:
        return "SDL_DROPBEGIN";
    case SDL_DROPCOMPLETE:
        return "SDL_DROPCOMPLETE";
    case SDL_AUDIODEVICEADDED:
        return "SDL_AUDIODEVICEADDED";
    case SDL_AUDIODEVICEREMOVED:
        return "SDL_AUDIODEVICEREMOVED";
    case SDL_SENSORUPDATE:
        return "SDL_SENSORUPDATE";
    case SDL_RENDER_TARGETS_RESET:
        return "SDL_RENDER_TARGETS_RESET";
    case SDL_RENDER_DEVICE_RESET:
        return "SDL_RENDER_DEVICE_RESET";
    case SDL_USEREVENT:
        return "SDL_USEREVENT";
    case SDL_LASTEVENT:
        return "SDL_LASTEVENT";
    default:
        return "Unknown event type";
    }
}

const char *get_button_name(SDL_GameControllerButton button)
{
    switch (button)
    {
    case SDL_CONTROLLER_BUTTON_A:
        return "SDL_CONTROLLER_BUTTON_A";
    case SDL_CONTROLLER_BUTTON_B:
        return "SDL_CONTROLLER_BUTTON_B";
    case SDL_CONTROLLER_BUTTON_X:
        return "SDL_CONTROLLER_BUTTON_X";
    case SDL_CONTROLLER_BUTTON_Y:
        return "SDL_CONTROLLER_BUTTON_Y";
    case SDL_CONTROLLER_BUTTON_BACK:
        return "SDL_CONTROLLER_BUTTON_BACK";
    case SDL_CONTROLLER_BUTTON_GUIDE:
        return "SDL_CONTROLLER_BUTTON_GUIDE";
    case SDL_CONTROLLER_BUTTON_START:
        return "SDL_CONTROLLER_BUTTON_START";
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return "SDL_CONTROLLER_BUTTON_LEFTSTICK";
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return "SDL_CONTROLLER_BUTTON_RIGHTSTICK";
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return "SDL_CONTROLLER_BUTTON_LEFTSHOULDER";
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return "SDL_CONTROLLER_BUTTON_RIGHTSHOULDER";
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return "SDL_CONTROLLER_BUTTON_DPAD_UP";
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return "SDL_CONTROLLER_BUTTON_DPAD_DOWN";
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return "SDL_CONTROLLER_BUTTON_DPAD_LEFT";
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return "SDL_CONTROLLER_BUTTON_DPAD_RIGHT";
    case SDL_CONTROLLER_BUTTON_INVALID:
        return "SDL_CONTROLLER_BUTTON_INVALID";
    case SDL_CONTROLLER_BUTTON_MAX:
        return "SDL_CONTROLLER_BUTTON_MAX";
    default:
        return "Unknown button";
    }
}

const char *get_key_name(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_RETURN:
        return "Return";
    case SDLK_ESCAPE:
        return "Escape";
    case SDLK_BACKSPACE:
        return "Backspace";
    case SDLK_TAB:
        return "Tab";
    case SDLK_SPACE:
        return "Space";
    case SDLK_EXCLAIM:
        return "!";
    case SDLK_QUOTEDBL:
        return "\"";
    case SDLK_HASH:
        return "#";
    case SDLK_PERCENT:
        return "%";
    case SDLK_DOLLAR:
        return "$";
    case SDLK_AMPERSAND:
        return "&";
    case SDLK_QUOTE:
        return "'";
    case SDLK_LEFTPAREN:
        return "(";
    case SDLK_RIGHTPAREN:
        return ")";
    case SDLK_ASTERISK:
        return "*";
    case SDLK_PLUS:
        return "+";
    case SDLK_COMMA:
        return ",";
    case SDLK_MINUS:
        return "-";
    case SDLK_PERIOD:
        return ".";
    case SDLK_SLASH:
        return "/";
    case SDLK_0:
        return "0";
    case SDLK_1:
        return "1";
    case SDLK_2:
        return "2";
    case SDLK_3:
        return "3";
    case SDLK_4:
        return "4";
    case SDLK_5:
        return "5";
    case SDLK_6:
        return "6";
    case SDLK_7:
        return "7";
    case SDLK_8:
        return "8";
    case SDLK_9:
        return "9";
    case SDLK_a:
        return "a";
    case SDLK_b:
        return "b";
    case SDLK_c:
        return "c";
    case SDLK_d:
        return "d";
    case SDLK_e:
        return "e";
    case SDLK_f:
        return "f";
    case SDLK_g:
        return "g";
    case SDLK_h:
        return "h";
    case SDLK_i:
        return "i";
    case SDLK_j:
        return "j";
    case SDLK_k:
        return "k";
    case SDLK_l:
        return "l";
    case SDLK_m:
        return "m";
    case SDLK_n:
        return "n";
    case SDLK_o:
        return "o";
    case SDLK_p:
        return "p";
    case SDLK_q:
        return "q";
    case SDLK_r:
        return "r";
    case SDLK_s:
        return "s";
    case SDLK_t:
        return "t";
    case SDLK_u:
        return "u";
    case SDLK_v:
        return "v";
    case SDLK_w:
        return "w";
    case SDLK_x:
        return "x";
    case SDLK_y:
        return "y";
    case SDLK_z:
        return "z";
    case SDLK_F1:
        return "F1";
    case SDLK_F2:
        return "F2";
    case SDLK_F3:
        return "F3";
    case SDLK_F4:
        return "F4";
    case SDLK_F5:
        return "F5";
    case SDLK_F6:
        return "F6";
    case SDLK_F7:
        return "F7";
    case SDLK_F8:
        return "F8";
    case SDLK_F9:
        return "F9";
    case SDLK_F10:
        return "F10";
    case SDLK_F11:
        return "F11";
    case SDLK_F12:
        return "F12";
    case SDLK_UP:
        return "Up Arrow";
    case SDLK_DOWN:
        return "Down Arrow";
    case SDLK_LEFT:
        return "Left Arrow";
    case SDLK_RIGHT:
        return "Right Arrow";
    case SDLK_INSERT:
        return "Insert";
    case SDLK_DELETE:
        return "Delete";
    case SDLK_HOME:
        return "Home";
    case SDLK_END:
        return "End";
    case SDLK_PAGEUP:
        return "Page Up";
    case SDLK_PAGEDOWN:
        return "Page Down";
    case SDLK_CAPSLOCK:
        return "Caps Lock";
    case SDLK_LCTRL:
        return "Left Ctrl";
    case SDLK_RCTRL:
        return "Right Ctrl";
    case SDLK_LSHIFT:
        return "Left Shift";
    case SDLK_RSHIFT:
        return "Right Shift";
    case SDLK_LALT:
        return "Left Alt";
    case SDLK_RALT:
        return "Right Alt";
    case SDLK_LGUI:
        return "Left GUI (Windows/Command)";
    case SDLK_RGUI:
        return "Right GUI (Windows/Command)";
    case SDLK_PRINTSCREEN:
        return "Print Screen";
    case SDLK_SCROLLLOCK:
        return "Scroll Lock";
    case SDLK_PAUSE:
        return "Pause";
    case SDLK_NUMLOCKCLEAR:
        return "Num Lock";
    default:
        return "Unknown Key";
    }
}
