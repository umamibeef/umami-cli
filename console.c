/*
 * MIT License
 *
 * Copyright (c) 2021 Michel Kakulphimp
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
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "console.h"

static consoleSettings_t *consoleSettings;

static const consoleSelection_t splashOptions[] = {{'m',"menus"},{'o',"options"}};
static const consoleSelection_t menuOptions[] = {{'t',"top"},{'u',"up"},{'n',"next"},{'p',"prev"},{'q',"quit"}};

char stringBuffer[STRING_BUFFER_SIZE];

void Console_Init(consoleSettings_t *settings)
{
    memset(stringBuffer, 0, STRING_BUFFER_SIZE);
    consoleSettings = settings;
}

void Console_Main(void)
{
    char selection;

    for (;;)
    {
        Console_PrintNewLine(LOGGING_LEVEL_0);
        Console_PrintNewLine(LOGGING_LEVEL_0);
        Console_PrintHeader(LOGGING_LEVEL_0, "Welcome");
        for (unsigned int line = 0; line < consoleSettings->numSplashLines; line++)
        {
            Console_Print(LOGGING_LEVEL_0, "%s", (*(consoleSettings->splashScreenPointer))[line]);
        }
        selection = Console_PrintOptionsAndGetResponse(splashOptions, SELECTION_SIZE(splashOptions), 0);
        
        switch(selection)
        {
            case 'm':
                Console_TraverseMenus(consoleSettings->mainMenuPointer);
                break;
            case 'o':
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" Options not implemented." ANSI_COLOR_RESET);
                break;
            default:
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" Something went wrong..." ANSI_COLOR_RESET);
                for (;;);
                break;
        };
    }
}

unsigned int Console_PromptForInt(const char *prompt)
{
    unsigned int input;

    Console_PrintNoEol(LOGGING_LEVEL_0, "%s ", prompt);
    scanf("%d", &input);
    Console_PrintNewLine(LOGGING_LEVEL_0);

    return input;
}

void Console_PromptForAnyKeyBlocking(void)
{
    Console_Print(LOGGING_LEVEL_0, "Press any key to continue");
    Console_CheckForKeyBlocking();
}

char Console_CheckForKeyBlocking(void)
{
    char c = 0;

    while (c == 0)
    {
        c = Console_GetCharInternal(LOGGING_LEVEL_0);
    }

    return c;
}

char Console_CheckForKey(void)
{
    return Console_GetCharInternal(LOGGING_LEVEL_0);
}

void Console_TraverseMenus(consoleMenu_t *menu)
{
    bool stayPut = true;
    consoleMenu_t *currentMenu = menu;
    char selection;
    char *charPointer;
    unsigned int totalPages;
    unsigned int numSelections;
    unsigned int selectedIndex;

    do
    {
        // Mutable menus need a function to update them, call it
        // prior to printing the menu out so it's updated.
        if (currentMenu->mode == MENU_MUTABLE)
        {
            // ToDo: Assert on null pointer.
            currentMenu->updater();
        }

        // Determine maximum selection is for this page
        if (((currentMenu->currentPage * PAGE_LENGTH) + PAGE_LENGTH) > currentMenu->menuLength)
        {
            numSelections = currentMenu->menuLength - (currentMenu->currentPage * PAGE_LENGTH);
        }
        else
        {
            numSelections = PAGE_LENGTH;
        }

        // Determine total pages for current menu (do this after a potential menu update)
        totalPages = TOTAL_PAGES(currentMenu->menuLength);
        
        Console_PrintMenu(currentMenu);
        selection = Console_PrintOptionsAndGetResponse(menuOptions, SELECTION_SIZE(menuOptions), numSelections);

        // Check the menu mode to see if the menu is mutable or static
        if (currentMenu->mode == MENU_MUTABLE)
        {
            // Menu is mutable, so accept the selection as-is (menu items are populated dynamically)
            selectedIndex = (selection - '0');
        }
        else
        {
            // Normal behaviour page 
            selectedIndex = ((currentMenu->currentPage * PAGE_LENGTH) + selection - '0');
        }

        // First check if it's a menu selection (selection should be valid)
        if (selection < (numSelections + '0'))
        {
            // Error out if we have neither
            if ((currentMenu->menuItems[selectedIndex].functionPointer == NO_FUNCTION_POINTER) &&
                (currentMenu->menuItems[selectedIndex].subMenu == NO_SUB_MENU))
            {
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" No submenu or function pointer!!!"ANSI_COLOR_RESET);
                // for (;;);
            }
            
            // Note:    A menu item can have both a submenu as well as a function pointer.
            //          Useful if a function needs to be called prior to entering a new menu.
            // Check if we have a function pointer
            if (currentMenu->menuItems[selectedIndex].functionPointer != NO_FUNCTION_POINTER)
            {
                // ToDo: Handle arguments.
                if (MENU_MUTABLE)
                {
                    // Pass menu name to first argument of function if menu is mutable
                    // ToDo: Either insert name at beginning or the end of the argument list when arguments are supported
                    charPointer = currentMenu->menuItems[selectedIndex].id.name;
                    currentMenu->menuItems[selectedIndex].functionPointer(1, &charPointer);
                }
                else
                {
                    currentMenu->menuItems[selectedIndex].functionPointer(NO_ARGS, NO_ARGS);
                }
                // We stay put after executing a function
                // ToDo: Print function return status
            }

            // Check if we have a submenu
            if (currentMenu->menuItems[selectedIndex].subMenu != NO_SUB_MENU)
            {
                currentMenu = currentMenu->menuItems[selectedIndex].subMenu;
                // Note! After this point, we've changed menus
            }
        }
        // Check if we're traversing up
        else if (selection == 'u')
        {
            // Go up if we can
            if (currentMenu->topMenu != NO_TOP_MENU)
            {
                // Reset page
                currentMenu->currentPage = 0;
                currentMenu = currentMenu->topMenu;
            }
        }
        // Check if we're traversing to top
        else if (selection == 't')
        {
            // Go up until we hit top menu
            while (currentMenu->topMenu != NO_TOP_MENU)
            {
                // Reset page
                currentMenu->currentPage = 0;
                currentMenu = currentMenu->topMenu;
            };
            
        }
        // Check if we're going to the previous page
        else if (selection == 'p')
        {
            // Only valid if we have more than one page and we aren't on the first page
            if ((totalPages > 1) && (currentMenu->currentPage > 0))
            {
                currentMenu->currentPage--;
            }
        }
        // Check if we're going to the next page
        else if (selection == 'n')
        {
            // Only valid if we have more than one page and we aren't on the last page
            if ((totalPages > 1) && (currentMenu->currentPage < (totalPages - 1)))
            {
                currentMenu->currentPage++;
            }
        }
        // Check if we're quitting
        else if (selection == 'q')
        {
            stayPut = false;
        }
        else
        {
            Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" Something went wrong..."ANSI_COLOR_RESET);
            for (;;);
        }
    }
    while (stayPut);
}

char Console_PrintOptionsAndGetResponse(const consoleSelection_t selections[], unsigned int numSelections, unsigned int numMenuSelections)
{
    // ToDo: Assert on number of menu selections greater than 10
    char c;
    bool valid = false;

    do
    {
        Console_PrintDivider(LOGGING_LEVEL_0);
        // Print menu selections (these will override any conflicting passed in selections)
        if (numMenuSelections != 0)
        {
            Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"0"ANSI_COLOR_RESET"-"ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-item ", ('0' + (numMenuSelections - 1)));
        }
        // Print passed in selections
        for (unsigned int i = 0; i < numSelections; i++)
        {
            Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-%s ", selections[i].key, selections[i].description);
        }
        Console_PrintNewLine(LOGGING_LEVEL_0);
        Console_PrintDivider(LOGGING_LEVEL_0);
        Console_PrintNoEol(LOGGING_LEVEL_0, " Selection > ");
        c = Console_CheckForKeyBlocking();

        // If we have menu selections, check for those first
        if (numMenuSelections != 0)
        {
            // Check if it's a valid menu selection
            if ((unsigned int)(c - '0') < numMenuSelections)
            {
                valid = true;
            }
        }
        
        // We didn't get a valid value yet
        if (!valid)
        {
            for (unsigned int i = 0; i < numSelections; i++)
            {
                if (c == selections[i].key)
                {
                    valid = true;
                    break;
                }
            }
        }
        
        if (!valid)
        {
            Console_PrintNewLine(LOGGING_LEVEL_0);
            Console_Print(LOGGING_LEVEL_0, " Bad selection %c! ", c);
        }
    }
    while (!valid);
    
    Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_GREEN" Selecting %c!"ANSI_COLOR_RESET, c);
    Console_PrintDivider(LOGGING_LEVEL_0);
    Console_PrintNewLine(LOGGING_LEVEL_0);
    
    return c;
}

void Console_Print(loggingLevel_e loggingLevel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(stringBuffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    Console_PutStringInternal(loggingLevel, stringBuffer);
    Console_PrintNewLine(loggingLevel);
}

void Console_PrintNoEol(loggingLevel_e loggingLevel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(stringBuffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    Console_PutStringInternal(loggingLevel, stringBuffer);
}

void Console_PrintNewLine(loggingLevel_e loggingLevel)
{
    Console_PutCharInternal(loggingLevel, '\r');
    Console_PutCharInternal(loggingLevel, '\n');
}

void Console_PrintHeader(loggingLevel_e loggingLevel, char *headerString)
{
    unsigned int stringLength = strlen(headerString);

    if (stringLength > MAX_HEADER_TITLE_WIDTH)
    {
        stringLength = MAX_HEADER_TITLE_WIDTH;
    }

    Console_PrintNoEol(loggingLevel, DBL_LINE_CHAR"["ANSI_COLOR_YELLOW" %s "ANSI_COLOR_RESET"]"DBL_LINE_CHAR, headerString);
    // Fill the rest of the line with '='
    for (unsigned int i = 0; i < (CONSOLE_WIDTH - stringLength - HEADER_TITLE_EXTRAS_WIDTH); i++)
    {
        Console_PutStringInternal(loggingLevel, DBL_LINE_CHAR);
    }
    Console_PrintNewLine(loggingLevel);
}

void Console_PrintDivider(loggingLevel_e loggingLevel)
{
    for (unsigned int i = 0; i < CONSOLE_WIDTH; i++)
    {
        Console_PutStringInternal(loggingLevel, SGL_LINE_CHAR);
    }
    Console_PrintNewLine(loggingLevel);
}

void Console_PrintMenu(consoleMenu_t *menu)
{
    unsigned int totalPages = TOTAL_PAGES(menu->menuLength);
    unsigned int startIndex;
    unsigned int endIndex;
    unsigned int menuOffset;
    unsigned int listOffset;

    // Mutable menus update dynamically, so no need to offset reading the menu array
    // ToDo: there HAS to be a better way of doing this
    if (menu->mode == MENU_MUTABLE)
    {
        menuOffset = (menu->currentPage * PAGE_LENGTH);
        listOffset = 0;
    }
    else
    {
        menuOffset = 0;
        listOffset = (menu->currentPage * PAGE_LENGTH);
    }

    // Calculate start and end indices
    startIndex = (menu->currentPage * PAGE_LENGTH) - menuOffset;
    endIndex = startIndex + PAGE_LENGTH;
    if (endIndex > (menu->menuLength - menuOffset))
    {
        endIndex = (menu->menuLength - menuOffset);
    }

    Console_PrintNewLine(LOGGING_LEVEL_0);
    Console_PrintHeader(LOGGING_LEVEL_0, menu->id.name);
    Console_PrintNewLine(LOGGING_LEVEL_0);
    Console_PrintNoEol(LOGGING_LEVEL_0, " %s", menu->id.description);
    if (totalPages > 1)
    {
        Console_Print(LOGGING_LEVEL_0, " - Page (%i/%i)", menu->currentPage + 1, totalPages);
    }
    else
    {
        Console_PrintNewLine(LOGGING_LEVEL_0);
    }
    Console_PrintNewLine(LOGGING_LEVEL_0);
    // If we have multiple pages and we aren't on the first page, indicate to
    // the user that they can go to the previous page
    if ((totalPages > 1) && (menu->currentPage > 0))
    {
        Console_Print(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"p"ANSI_COLOR_RESET"] <<< Prev Page");
    }
    for (unsigned int i = startIndex; i < endIndex; i++)
    {
        consoleMenuItem_t *menuItem = &(menu->menuItems[i]);
        Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"] %s", '0' + i - listOffset, menuItem->id.name);
        if (menuItem->id.description[0] != '\0')
        {
            Console_Print(LOGGING_LEVEL_0, " - %s", menuItem->id.description);
        }
        else
        {
            Console_PrintNewLine(LOGGING_LEVEL_0);
        }
    }
    // If we have multiple pages and we aren't on the first page, indicate to
    // the user that they can go to the next page
    if ((totalPages > 1) && (menu->currentPage < (totalPages - 1)))
    {
        Console_Print(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"n"ANSI_COLOR_RESET"] >>> Next Page");
    }
    Console_PrintNewLine(LOGGING_LEVEL_0);
}

char Console_GetCharInternal(loggingLevel_e loggingLevel)
{
    if (consoleSettings->loggingLevel >= loggingLevel)
    {
        return Console_GetChar();
    }
    else
    {
        return '\0';
    }
}

void Console_PutCharInternal(loggingLevel_e loggingLevel, char c)
{
    if (consoleSettings->loggingLevel >= loggingLevel)
    {
        Console_PutChar(c);
    }
}

void Console_PutStringInternal(loggingLevel_e loggingLevel, char * string)
{
    if (consoleSettings->loggingLevel >= loggingLevel)
    {
        Console_PutString(string);
    }
}
