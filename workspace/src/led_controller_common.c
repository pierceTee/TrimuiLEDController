#include "led_controller_common.h"

const char *animation_effect_to_string(AnimationEffect effect)
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

const char *color_to_string(uint32_t color)
{
  static char hex_color[STRING_LENGTH];
  switch (color)
  {
  case 0xFF0000:
    return "Red";
  case 0xFF8080:
    return "Light Red";
  case 0x800000:
    return "Maroon";
  case 0xFF0080:
    return "Hot Pink";
  case 0xFF8000:
    return "Orange";
  case 0x00FF00:
    return "Green";
  case 0x00FF80:
    return "Lime";
  case 0xFFFF00:
    return "Yellow";
  case 0x808000:
    return "Olive";
  case 0x0000FF:
    return "Blue";
  case 0x0080FF:
    return "Light Blue";
  case 0x000080:
    return "Navy";
  case 0x00FFFF:
    return "Cyan";
  case 0x008080:
    return "Teal";
  case 0xFF00FF:
    return "Magenta";
  case 0xFF80C0:
    return "Pink";
  case 0xFFFFFF:
    return "White";
  default:
    snprintf(hex_color, sizeof(hex_color), "0x%06X", color);
    return hex_color;
  }
}

uint32_t next_color(uint32_t color, int sign)
{
  int16_t r = (color >> 16) & 0xFF;
  int16_t g = (color >> 8) & 0xFF;
  int16_t b = color & 0xFF;

  printf("==> START r: %d, g: %d, b: %d\n", r, g, b);
  // Red -> Yellow -> Green -> Cyan -> Blue -> Magenta -> Red
  if (r == 255 && g < 255 && b == 0)
  { // Red to Yellow
    g = (g + sign * COLOR_CYLCE_INCREMENT);
    if (g > 255)
      g = 255;
    if (g < 0)
      g = 0;
  }
  else if (r > 0 && g == 255 && b == 0)
  { // Yellow to Green
    r = (r - sign * COLOR_CYLCE_INCREMENT);
    if (r > 255)
      r = 255;
    if (r < 0)
      r = 0;
  }
  else if (g == 255 && r == 0 && b < 255)
  { // Green to Cyan
    b = (b + sign * COLOR_CYLCE_INCREMENT);
    if (b > 255)
      b = 255;
    if (b < 0)
      b = 0;
  }
  else if (g > 0 && b == 255 && r == 0)
  { // Cyan to Blue
    g = (g - sign * COLOR_CYLCE_INCREMENT);
    if (g > 255)
      g = 255;
    if (g < 0)
      g = 0;
  }
  else if (b == 255 && g == 0 && r < 255)
  { // Blue to Magenta
    r = (r + sign * COLOR_CYLCE_INCREMENT);
    if (r > 255)
      r = 255;
    if (r < 0)
      r = 0;
  }
  else if (r == 255 && g == 0 && b > 0)
  { // Magenta to Red
    b = (b - sign * COLOR_CYLCE_INCREMENT);
    if (b > 255)
      b = 255;
    if (b < 0)
      b = 0;
  }
  else
  { // Initialize to red if no valid state
    r = 255;
    g = 0;
    b = 0;
  }
  printf("==> FINISH r: %d, g: %d, b: %d\n", r, g, b);
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

const char *led_setting_option_to_string(LedSettingOption setting)
{
  switch (setting)
  {
  case SELECTED_LED:
    return "LED: ";
    break;
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

const char *menu_option_to_string(MenuOption option, const AppState *app_state)
{
  switch (option)
  {
  case TOGGLE_EXTENDED_COLORS:
    if (app_state->are_extended_colors_enabled)
      return "Disable Extended Colors";
    else
      return "Enable Extended Colors";
  case ENABLE_ALL:
    return "Turn on all LEDs";
  case DISABLE_ALL:
    return "Turn off all LEDs";
  case UNINSTALL:
    return "Uninstall";
  case QUIT:
    return "Quit";
  default:
    return "UNKNOWN";
  }
}

const char *led_to_string(Led led)
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

const char *led_internal_name(Led led)
{
  switch (led)
  {
  case LED_FRONT:
    return "f1f2";
  case LED_TOP:
    return "m";
  case LED_BACK:
    return "lr";
  default:
    return "UNKNOWN";
  }
}

Led internal_led_name_to_led(const char *led_name)
{
  if (strcmp(led_name, "f1f2") == 0)
  {
    return LED_FRONT;
  }
  else if (strcmp(led_name, "m") == 0)
  {
    return LED_TOP;
  }
  else if (strcmp(led_name, "lr") == 0)
  {
    return LED_BACK;
  }
  else
  {
    return -1;
  }
}

void debug_log(const char *message, bool verbose_logging_enabled)
{
  if (verbose_logging_enabled)
  {
    printf("%s\n", message);
  }
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