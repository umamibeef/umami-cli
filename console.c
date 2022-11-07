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
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "console.h"

static ConsoleSettings_t *console_settings;

static const ConsoleSelection_t splash_options[] = {{'m',"menus"},{'o',"options"},{'q',"quit program"}};
static const ConsoleSelection_t menu_options[] = {{'t',"top"},{'u',"up"},{'n',"next"},{'p',"prev"},{'q',"quit menus"}};

char string_buffer[STRING_BUFFER_SIZE];

void Console_Init(ConsoleSettings_t *settings)
{
    memset(string_buffer, 0, STRING_BUFFER_SIZE);
    console_settings = settings;
}

void Console_Main(void)
{
    char selection;

    for (;;)
    {
        Console_PrintNewLine(LOGGING_LEVEL_0);
        Console_PrintNewLine(LOGGING_LEVEL_0);
        Console_PrintHeader(LOGGING_LEVEL_0, "Welcome");
        for (unsigned int line = 0; line < console_settings->num_splash_lines; line++)
        {
            Console_Print(LOGGING_LEVEL_0, "%s", (*(console_settings->splash_screen_pointer))[line]);
        }
        selection = Console_PrintOptionsAndGetResponse(splash_options, SELECTION_SIZE(splash_options), 0, 0);
        
        switch(selection)
        {
            case 'm':
                Console_TraverseMenus(console_settings->main_menu_pointer);
                break;
            case 'o':
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" Options not implemented." ANSI_COLOR_RESET);
                break;
            case 'q':
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_CYAN" Bye-bye!\n" ANSI_COLOR_RESET);
                return;
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
    unsigned int return_val;

    Console_PrintNoEol(LOGGING_LEVEL_0, "%s ", prompt);
    return_val = scanf("%d", &input);
    Console_PrintNewLine(LOGGING_LEVEL_0);

    if (return_val == 0)
    {
        return 0;
    }
    else
    {
        return input;
    }
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

void Console_TraverseMenus(ConsoleMenu_t *menu)
{
    bool stay_put = true;
    ConsoleMenu_t *current_menu = menu;
    char selection;
    char *char_pointer;
    unsigned int total_pages;
    unsigned int num_selections;
    unsigned int selected_index;

    do
    {
        // Mutable menus need a function to update them, call it
        // prior to printing the menu out so it's updated.
        if (current_menu->mode == MENU_MUTABLE)
        {
            // ToDo: Assert on null pointer.
            current_menu->updater();
        }

        // Determine maximum selection is for this page
        if (((current_menu->current_page * PAGE_LENGTH) + PAGE_LENGTH) > current_menu->menu_length)
        {
            num_selections = current_menu->menu_length - (current_menu->current_page * PAGE_LENGTH);
        }
        else
        {
            num_selections = PAGE_LENGTH;
        }

        // Determine total pages for current menu (do this after a potential menu update)
        total_pages = TOTAL_PAGES(current_menu->menu_length);
        
        Console_PrintMenu(current_menu);
        selection = Console_PrintOptionsAndGetResponse(menu_options, SELECTION_SIZE(menu_options), num_selections, 0);

        // Check the menu mode to see if the menu is mutable or static
        if (current_menu->mode == MENU_MUTABLE)
        {
            // Menu is mutable, so accept the selection as-is (menu items are populated dynamically)
            selected_index = (selection - '0');
        }
        else
        {
            // Normal behaviour page 
            selected_index = ((current_menu->current_page * PAGE_LENGTH) + selection - '0');
        }

        // First check if it's a menu selection (selection should be valid)
        if ((unsigned int)selection < (unsigned int)(num_selections + '0'))
        {
            // Error out if we have neither
            if ((current_menu->menu_items[selected_index].function_pointer == NO_FUNCTION_POINTER) &&
                (current_menu->menu_items[selected_index].sub_menu == NO_SUB_MENU))
            {
                Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" No submenu or function pointer!!!"ANSI_COLOR_RESET);
                // for (;;);
            }
            
            // Note:    A menu item can have both a submenu as well as a function pointer.
            //          Useful if a function needs to be called prior to entering a new menu.
            // Check if we have a function pointer
            if (current_menu->menu_items[selected_index].function_pointer != NO_FUNCTION_POINTER)
            {
                // ToDo: Handle arguments.
                if (MENU_MUTABLE)
                {
                    // Pass menu name to first argument of function if menu is mutable
                    // ToDo: Either insert name at beginning or the end of the argument list when arguments are supported
                    char_pointer = current_menu->menu_items[selected_index].id.name;
                    current_menu->menu_items[selected_index].function_pointer(1, &char_pointer);
                }
                else
                {
                    current_menu->menu_items[selected_index].function_pointer(NO_ARGS, NO_ARGS);
                }
                // We stay put after executing a function
                // ToDo: Print function return status
            }

            // Check if we have a submenu
            if (current_menu->menu_items[selected_index].sub_menu != NO_SUB_MENU)
            {
                current_menu = current_menu->menu_items[selected_index].sub_menu;
                // Note! After this point, we've changed menus
            }
        }
        // Check if we're traversing up
        else if (selection == 'u')
        {
            // Go up if we can
            if (current_menu->top_menu != NO_TOP_MENU)
            {
                // Reset page
                current_menu->current_page = 0;
                current_menu = current_menu->top_menu;
            }
        }
        // Check if we're traversing to top
        else if (selection == 't')
        {
            // Go up until we hit top menu
            while (current_menu->top_menu != NO_TOP_MENU)
            {
                // Reset page
                current_menu->current_page = 0;
                current_menu = current_menu->top_menu;
            };
            
        }
        // Check if we're going to the previous page
        else if (selection == 'p')
        {
            // Only valid if we have more than one page and we aren't on the first page
            if ((total_pages > 1) && (current_menu->current_page > 0))
            {
                current_menu->current_page--;
            }
        }
        // Check if we're going to the next page
        else if (selection == 'n')
        {
            // Only valid if we have more than one page and we aren't on the last page
            if ((total_pages > 1) && (current_menu->current_page < (total_pages - 1)))
            {
                current_menu->current_page++;
            }
        }
        // Check if we're quitting
        else if (selection == 'q')
        {
            stay_put = false;
        }
        else
        {
            Console_Print(LOGGING_LEVEL_0, ANSI_COLOR_RED" Something went wrong..."ANSI_COLOR_RESET);
            for (;;);
        }
    }
    while (stay_put);
}

char Console_PrintOptionsAndGetResponse(const ConsoleSelection_t selections[], unsigned int num_selections, unsigned int num_menu_selections, unsigned int option_flags)
{
    // ToDo: Assert on number of menu selections greater than 10
    char c;
    bool valid = false;

    do
    {
        if (!(option_flags & NO_DIVIDERS))
        {
            Console_PrintDivider(LOGGING_LEVEL_0);
        }
        // Print menu selections (these will override any conflicting passed in selections)
        if (num_menu_selections != 0)
        {
            Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"0"ANSI_COLOR_RESET"-"ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-item ", ('0' + (num_menu_selections - 1)));
        }
        // Print passed in selections
        for (unsigned int i = 0; i < num_selections; i++)
        {
            Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-%s ", selections[i].key, selections[i].description);
            if ((option_flags & ORIENTATION_V) && !(i == (num_selections - 1)))
            {
                Console_PrintNewLine(LOGGING_LEVEL_0);
            }
        }
        Console_PrintNewLine(LOGGING_LEVEL_0);
        if (!(option_flags & NO_DIVIDERS))
        {
            Console_PrintDivider(LOGGING_LEVEL_0);
        }
        else
        {
            Console_PrintNewLine(LOGGING_LEVEL_0);
        }
        Console_PrintNoEol(LOGGING_LEVEL_0, " Selection > ");
        c = Console_CheckForKeyBlocking();

        // If we have menu selections, check for those first
        if (num_menu_selections != 0)
        {
            // Check if it's a valid menu selection
            if ((unsigned int)(c - '0') < num_menu_selections)
            {
                valid = true;
            }
        }
        
        // We didn't get a valid value yet
        if (!valid)
        {
            for (unsigned int i = 0; i < num_selections; i++)
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

void Console_Print(LoggingLevel_e logging_level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    Console_PutStringInternal(logging_level, string_buffer);
    Console_PrintNewLine(logging_level);
}

void Console_PrintNoEol(LoggingLevel_e logging_level, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    Console_PutStringInternal(logging_level, string_buffer);
}

void Console_PrintNewLine(LoggingLevel_e logging_level)
{
    Console_PutCharInternal(logging_level, '\r');
    Console_PutCharInternal(logging_level, '\n');
}

void Console_PrintHeader(LoggingLevel_e logging_level, const char *header_string)
{
    unsigned int string_length = strlen(header_string);

    if (string_length > MAX_HEADER_TITLE_WIDTH)
    {
        string_length = MAX_HEADER_TITLE_WIDTH;
    }

    Console_PrintNoEol(logging_level, DBL_LINE_CHAR"["ANSI_COLOR_YELLOW" %s "ANSI_COLOR_RESET"]"DBL_LINE_CHAR, header_string);
    // Fill the rest of the line with '='
    for (unsigned int i = 0; i < (CONSOLE_WIDTH - string_length - HEADER_TITLE_EXTRAS_WIDTH); i++)
    {
        Console_PutStringInternal(logging_level, DBL_LINE_CHAR);
    }
    Console_PrintNewLine(logging_level);
}

void Console_PrintDivider(LoggingLevel_e logging_level)
{
    for (unsigned int i = 0; i < CONSOLE_WIDTH; i++)
    {
        Console_PutStringInternal(logging_level, SGL_LINE_CHAR);
    }
    Console_PrintNewLine(logging_level);
}

void Console_PrintMenu(ConsoleMenu_t *menu)
{
    unsigned int total_pages = TOTAL_PAGES(menu->menu_length);
    unsigned int start_index;
    unsigned int end_index;
    unsigned int menu_offset;
    unsigned int list_offset;

    // Mutable menus update dynamically, so no need to offset reading the menu array
    // ToDo: there HAS to be a better way of doing this
    if (menu->mode == MENU_MUTABLE)
    {
        menu_offset = (menu->current_page * PAGE_LENGTH);
        list_offset = 0;
    }
    else
    {
        menu_offset = 0;
        list_offset = (menu->current_page * PAGE_LENGTH);
    }

    // Calculate start and end indices
    start_index = (menu->current_page * PAGE_LENGTH) - menu_offset;
    end_index = start_index + PAGE_LENGTH;
    if (end_index > (menu->menu_length - menu_offset))
    {
        end_index = (menu->menu_length - menu_offset);
    }

    Console_PrintNewLine(LOGGING_LEVEL_0);
    Console_PrintHeader(LOGGING_LEVEL_0, menu->id.name);
    Console_PrintNewLine(LOGGING_LEVEL_0);
    Console_PrintNoEol(LOGGING_LEVEL_0, " %s", menu->id.description);
    if (total_pages > 1)
    {
        Console_Print(LOGGING_LEVEL_0, " - Page (%i/%i)", menu->current_page + 1, total_pages);
    }
    else
    {
        Console_PrintNewLine(LOGGING_LEVEL_0);
    }
    Console_PrintNewLine(LOGGING_LEVEL_0);
    // If we have multiple pages and we aren't on the first page, indicate to
    // the user that they can go to the previous page
    if ((total_pages > 1) && (menu->current_page > 0))
    {
        Console_Print(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"p"ANSI_COLOR_RESET"] <<< Prev Page");
    }
    for (unsigned int i = start_index; i < end_index; i++)
    {
        ConsoleMenuItem_t *menu_item = &(menu->menu_items[i]);
        Console_PrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"] %s", '0' + i - list_offset, menu_item->id.name);
        if (menu_item->id.description[0] != '\0')
        {
            Console_Print(LOGGING_LEVEL_0, " - %s", menu_item->id.description);
        }
        else
        {
            Console_PrintNewLine(LOGGING_LEVEL_0);
        }
    }
    // If we have multiple pages and we aren't on the first page, indicate to
    // the user that they can go to the next page
    if ((total_pages > 1) && (menu->current_page < (total_pages - 1)))
    {
        Console_Print(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"n"ANSI_COLOR_RESET"] >>> Next Page");
    }
    Console_PrintNewLine(LOGGING_LEVEL_0);
}

char Console_GetCharInternal(LoggingLevel_e logging_level)
{
    if (console_settings->logging_level >= logging_level)
    {
        return Console_GetChar();
    }
    else
    {
        return '\0';
    }
}

void Console_PutCharInternal(LoggingLevel_e logging_level, char c)
{
    if (console_settings->logging_level >= logging_level)
    {
        Console_PutChar(c);
    }
}

void Console_PutStringInternal(LoggingLevel_e logging_level, const char * string)
{
    if (console_settings->logging_level >= logging_level)
    {
        Console_PutString(string);
    }
}
