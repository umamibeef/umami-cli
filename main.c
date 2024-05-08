/*
 * MIT License
 *
 * Copyright (c) 2022 Michel Kakulphimp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(WIN32)
#include <locale.h>
#include <windows.h>
#endif
#include "console.h"

// This file gives an example of how to use some of the functions in this
// library.

// Define the splash screen
Splash_t splash_screen = {
    ANSI_COLOR_RED,
    "   __  __                          _       ________    ____",
    "  / / / /___ ___  ____ _____ ___  (_)     / ____/ /   /  _/",
    " / / / / __ `__ \\/ __ `/ __ `__ \\/ /_____/ /   / /    / /  ",
    "/ /_/ / / / / / / /_/ / / / / / / /_____/ /___/ /____/ /   ",
    "\\____/_/ /_/ /_/\\__,_/_/ /_/ /_/_/      \\____/_____/___/    ",
    " ",
    ANSI_COLOR_RESET,
    "   Umami-CLI Library Demo",
    "   Build Date: "__DATE__ " " __TIME__
    " ",
    " ", // New line
    "",  // Empty string, end of splash screen indicator
};

// Forward declaration of menus and functions
extern ConsoleMenu_t main_menu;
extern ConsoleMenu_t sub_menu_0;
extern FunctionResult_e ExampleHelloFunc(int argc, char *argv[]);

// Start of main menu definition
ConsoleMenuItem_t main_menu_items[] = {
    {{"One", "The first menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Two", "The second menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Three", "The third menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Four", "The fourth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Five", "The fifth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Six", "The sixth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Seven", "The seventh menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Eight", "The eight menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Nine", "The ninth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Ten", "The tenth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Eleven", "The eleventh menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
    {{"Twelve", "The twelfth menu item"}, &sub_menu_0, NO_FUNCTION_POINTER},
};
ConsoleMenu_t main_menu = {{"Main Menu", "This is the main menu."},
                           main_menu_items,
                           NO_MAIN_MENU,
                           MENU_SIZE(main_menu_items),
                           MENU_DEFAULT,
                           NO_FUNCTION_POINTER};
// End of main menu definition

// Start of a sub menu definition
ConsoleMenuItem_t sub_menu_0_items[] = {
    {{"Hello", "Call the hello function!"}, NO_SUB_MENU, ExampleHelloFunc},
};
ConsoleMenu_t sub_menu_0 = {{"Sub Menu", "Sub menu shared by all."},
                            sub_menu_0_items,
                            &main_menu,
                            MENU_SIZE(sub_menu_0_items),
                            MENU_DEFAULT,
                            NO_FUNCTION_POINTER};
// End of a sub menu definition

// Define these based on the platform you're working on
FunctionResult_e console_os_init(void) {
  FunctionResult_e result = FR_OK;
#if defined(WIN32)
  /* Setting locale for Windows commands prompts */
  setlocale(LC_ALL, ".utf8");

  /* Enabling ANSI colors for Windows commands prompts */
  HANDLE handle;
  DWORD mode = 0;

  handle = GetStdHandle(STD_OUTPUT_HANDLE);

  if (handle == INVALID_HANDLE_VALUE) {
    printf("Warning: Got INVALID_HANDLE_VALUE from "
           "GetStdHandle(STD_OUTPUT_HANDLE)\n");
    return result;
  }

  if (!GetConsoleMode(handle, &mode)) {
    printf("Warning: GetConsoleMode() failed for handle %p. GetLastError() == "
           "%ld\n",
           handle, GetLastError());
    return result;
  }
  +
      // Enable ANSI escape codes
      mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

  if (!SetConsoleMode(handle, mode)) {
    printf("Waning: SetConsoleMode() failed for handle %p. GetLastError() == "
           "%ld\n",
           handle, GetLastError());
    return result;
  }
#endif /* defined(WIN32) */
  return result;
}

// Definitions to enable console.h to to its thing
char console_get_char(void) {
  char c;
  int result = 0;
#if defined(WIN32)
  DWORD mode;
  HANDLE handle;
#endif

/* Enable single character entry */
#if defined(__linux__) || defined(__APPLE__)
  result = system("/bin/stty raw");
  if (result != 0) {
    console_print_error(
        LOGGING_LEVEL_0,
        "%s: Fatal Error: system(\"/bin/stty raw\") returned %d", __FUNCTION__,
        result);
    exit(FR_FAIL);
  }
#elif defined(WIN32)
  handle = GetStdHandle(STD_INPUT_HANDLE);
  if (handle == INVALID_HANDLE_VALUE) {
    exit(GetLastError());
  }
  if (!GetConsoleMode(handle, &mode)) {
    exit(GetLastError());
  }
  SetConsoleMode(handle, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
#endif /* defined(WIN32) */

  do {
    c = getc(stdin);
  } while (!isalpha(c) && !isdigit(c));

/* Disable single character entry */
#if defined(__linux__) || defined(__APPLE__)
  result = system("/bin/stty cooked");
  if (result != 0) {
    console_print_error(
        LOGGING_LEVEL_0,
        "%s: Fatal Error: system(\"/bin/stty cooked\") returned %d",
        __FUNCTION__, result);
    exit(FR_FAIL);
  }
#elif defined(WIN32)
  if (!SetConsoleMode(handle, mode)) {
    exit(GetLastError());
  }
#endif /* defined(WIN32) */

  return c;
}

void console_put_char(char c) { putc(c, stdout); }

void console_put_string(const char *string) { printf("%s", string); }

// An example function
FunctionResult_e ExampleHelloFunc(int argc, char *argv[]) {
  console_print(LOGGING_LEVEL_0, "Hello! How do you do?");

  return FR_OK;
}

int main(int argc, char *argv[]) {
  // Setup console interface
  ConsoleSettings_t console_settings = {
      .splash_screen_pointer = &splash_screen,
      .main_menu_pointer = &main_menu,
      .small_headers = false,
      .logging_level = LOGGING_LEVEL_0,
      .os_init_fn = console_os_init,
      .get_char_fn = console_get_char,
      .put_char_fn = console_put_char,
      .put_string_fn = console_put_string,
  };
  console_init(&console_settings);
  // Erase screen
  console_print(LOGGING_LEVEL_0, ERASE_SCREEN);
  // Start console interface
  console_main(); // Does not return
}