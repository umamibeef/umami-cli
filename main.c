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

#include <stdio.h>
#include "console.h"

// This file gives an example of how to use some of the functions in this library.

// Define the splash screen and the amount of lines the splash screen takes
#define NUM_SPLASH_LINES (11)
Splash_t splash_screen =
{
    ANSI_COLOR_RED,
    "   ___                      _                                  ",
    "  / __\\___  _ __  ___  ___ | | ___  /\\/\\   ___ _ __  _   _ ___ ",
    " / /  / _ \\| '_ \\/ __|/ _ \\| |/ _ \\/    \\ / _ \\ '_ \\| | | / __|",
    "/ /__| (_) | | | \\__ \\ (_) | |  __/ /\\/\\ \\  __/ | | | |_| \\__ \\",
    "\\____/\\___/|_| |_|___/\\___/|_|\\___\\/    \\/\\___|_| |_|\\__,_|___/",
    "",
    ANSI_COLOR_RESET,
    "   Console Menus Library",
    "   Build Date: "__DATE__" "__TIME__"",
    "",
};

// Forward declaration of menus and functions
extern ConsoleMenu_t main_menu;
extern ConsoleMenu_t sub_menu_0;
extern FunctionResult_e ExampleHelloFunc(int argc, char *argv[]);

// Start of main menu definition
ConsoleMenuItem_t main_menu_items[] = 
{
    {{"One", "The first menu item"},        &sub_menu_0,    NO_FUNCTION_POINTER},  
    {{"Two", "The second menu item"},       &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Three", "The third menu item"},      &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Four", "The fourth menu item"},      &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Five", "The fifth menu item"},       &sub_menu_0,    NO_FUNCTION_POINTER},  
    {{"Six", "The sixth menu item"},        &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Seven", "The seventh menu item"},    &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Eight", "The eight menu item"},      &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Nine", "The ninth menu item"},       &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Ten", "The tenth menu item"},        &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Eleven", "The eleventh menu item"},  &sub_menu_0,    NO_FUNCTION_POINTER},
    {{"Twelve", "The twelfth menu item"},   &sub_menu_0,    NO_FUNCTION_POINTER},
};
ConsoleMenu_t main_menu = {{"Main Menu", "This is the main menu."}, main_menu_items, NO_TOP_MENU, MENU_SIZE(main_menu_items)};
// End of main menu definition

// Start of a sub menu definition
ConsoleMenuItem_t sub_menu_0_items[] = 
{
    {{"Hello", "Call the hello function!"}, NO_SUB_MENU,    ExampleHelloFunc},  
};
ConsoleMenu_t sub_menu_0 = {{"Sub Menu", "Sub menu shared by all."}, sub_menu_0_items, &main_menu, MENU_SIZE(sub_menu_0_items)};
// End of a sub menu definition

// Define these based on the platform you're working on
char Console_GetChar(void)
{
    char c;
    do
    {
        c = getc(stdin);
    }
    while (c == '\n');

    return c;
}
void Console_PutChar(char c)
{
    putc(c, stdout);
}
void Console_PutString(char * string)
{
    printf("%s", string);
}

// An example function
FunctionResult_e ExampleHelloFunc(int argc, char *argv[])
{
    Console_Print(LOGGING_LEVEL_0, "Hello! How do you do?");

    return SUCCESS;
}

int main(int argc, char *argv[])
{
    // Setup console interface
    ConsoleSettings_t console_settings = 
    {
        &splash_screen,
        NUM_SPLASH_LINES,
        &main_menu,
        LOGGING_LEVEL_0,
    };
    Console_Init(&console_settings);
    // Erase screen
    Console_Print(LOGGING_LEVEL_0, ERASE_SCREEN);
    // Start console interface
    Console_Main(); // Does not return
}