/*
 * MIT License
 *
 * Copyright (c) 2023 Michel Kakulphimp
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
#include <inttypes.h>

#include "console-menus/console.h"

static ConsoleSettings_t *console_settings;

static const ConsoleSelection_t splash_options[] = {{'m', "menus"}, {'o', "options"}, {'q', "quit program"}};
static const ConsoleSelection_t menu_options[] = {{'t', "top"}, {'u', "up"}, {'n', "next"}, {'p', "prev"}, {'q', "quit menus"}};

char string_buffers[NUM_STRING_BUFFERS][STRING_BUFFER_SIZE];
unsigned int current_string_buffer_index = 0;

void ConsoleInit(ConsoleSettings_t *settings)
{
    memset(string_buffers, 0, sizeof(char) * NUM_STRING_BUFFERS * STRING_BUFFER_SIZE);
    current_string_buffer_index = 0;
    console_settings = settings;
}

void ConsoleMain(void)
{
    char selection;

    for (;;)
    {
        ConsolePrintNewLine(LOGGING_LEVEL_0);
        ConsolePrintNewLine(LOGGING_LEVEL_0);
        ConsolePrintHeader(LOGGING_LEVEL_0, "Welcome");
        for (unsigned int line = 0; line < console_settings->num_splash_lines; line++)
        {
            ConsolePrint(LOGGING_LEVEL_0, "%s", (*(console_settings->splash_screen_pointer))[line]);
        }
        selection = ConsolePrintOptionsAndGetResponse(splash_options, SELECTION_SIZE(splash_options), 0, 0);

        switch (selection)
        {
            case 'm':
                ConsoleTraverseMenus(console_settings->main_menu_pointer);
                break;
            case 'o':
                ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_RED" Options not implemented." ANSI_COLOR_RESET);
                break;
            case 'q':
                ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_CYAN" Bye-bye!\n" ANSI_COLOR_RESET);
                return;
                break;
            default:
                ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_RED" Something went wrong..." ANSI_COLOR_RESET);
                for (;;);
                break;
        };
    }
}

void ConsoleSmallHeaders(bool enable)
{
    console_settings->small_headers = enable;
}

unsigned int ConsolePromptForInt(const char *prompt, unsigned int default_val)
{
    char buffer[100];
    unsigned int input;
    unsigned int return_val;

    ConsolePrintNoEol(LOGGING_LEVEL_0, "%s (default: %d) > ", prompt, default_val);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return default_val;
    }

    if (sscanf(buffer, "%d", &input) != 1)
    {
        return default_val;
    }

    return input;
}

uint32_t ConsolePromptForHexUInt32(const char *prompt, uint32_t default_val)
{
    char buffer[100];
    uint32_t input;
    uint32_t return_val;

    ConsolePrintNoEol(LOGGING_LEVEL_0, "%s (default: 0x%x) > ", prompt, default_val);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return default_val;
    }

    if (sscanf(buffer, "%x", &input) != 1)
    {
        return default_val;
    }

    return input;
}

uint64_t ConsolePromptForHexUInt64(const char *prompt, uint64_t default_val)
{
    char buffer[100];
    uint64_t input;
    uint64_t return_val;

    ConsolePrintNoEol(LOGGING_LEVEL_0, "%s (default: 0x%x) > ", prompt, default_val);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return default_val;
    }

    if (sscanf(buffer, "%lx", &input) != 1)
    {
        return default_val;
    }

    return input;
}

char *ConsolePromptForString(const char *prompt, const char *default_val)
{
    char *return_val;
    unsigned char *char_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];

    ConsolePrintNoEol(LOGGING_LEVEL_0, "%s (default: %s) > ", prompt, default_val);

    if (fgets(char_buffer, sizeof(STRING_BUFFER_SIZE), stdin) == NULL)
    {
        return default_val;
    }

    if (sscanf(char_buffer, "%s", &string_buffer) != 1)
    {
        return default_val;
    }
}

void ConsolePromptForAnyKeyBlocking(void)
{
    ConsolePrint(LOGGING_LEVEL_0, "Press any key to continue");
    ConsoleCheckForKeyBlocking();
}

char ConsoleCheckForKeyBlocking(void)
{
    char c = 0;

    while (c == 0)
    {
        c = ConsoleGetCharInternal(LOGGING_LEVEL_0);
    }

    return c;
}

char ConsoleCheckForKey(void)
{
    return ConsoleGetCharInternal(LOGGING_LEVEL_0);
}

void ConsoleTraverseMenus(ConsoleMenu_t *menu)
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

        ConsolePrintMenu(current_menu);
        selection = ConsolePrintOptionsAndGetResponse(menu_options, SELECTION_SIZE(menu_options), num_selections, 0);

        selected_index = ((current_menu->current_page * PAGE_LENGTH) + selection - '0');

        // First check if it's a menu selection (selection should be valid)
        if ((unsigned int)selection < (unsigned int)(num_selections + '0'))
        {
            // Error out if we have neither
            if ((current_menu->menu_items[selected_index].function_pointer == NO_FUNCTION_POINTER) &&
                (current_menu->menu_items[selected_index].sub_menu == NO_SUB_MENU))
            {
                ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_RED" No submenu or function pointer!!!"ANSI_COLOR_RESET);
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
            ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_RED" Something went wrong..."ANSI_COLOR_RESET);
            for (;;);
        }
    }
    while (stay_put);
}

char ConsolePrintOptionsAndGetResponse(const ConsoleSelection_t selections[], unsigned int num_selections,
                                       unsigned int num_menu_selections, unsigned int option_flags)
{
    // ToDo: Assert on number of menu selections greater than 10
    char c;
    bool valid = false;

    do
    {
        if (!(option_flags & NO_DIVIDERS))
        {
            ConsolePrintDivider(LOGGING_LEVEL_0);
        }
        else
        {
            ConsolePrintNewLine(LOGGING_LEVEL_0);
        }
        // Print menu selections (these will override any conflicting passed in selections)
        if (num_menu_selections != 0)
        {
            ConsolePrintNoEol(LOGGING_LEVEL_0,
                              " ["ANSI_COLOR_YELLOW"0"ANSI_COLOR_RESET"-"ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-item ",
                              ('0' + (num_menu_selections - 1)));
        }
        // Print passed in selections
        for (unsigned int i = 0; i < num_selections; i++)
        {
            ConsolePrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-%s ", selections[i].key,
                              selections[i].description);
            if ((option_flags & ORIENTATION_V) && !(i == (num_selections - 1)))
            {
                ConsolePrintNewLine(LOGGING_LEVEL_0);
            }
        }
        ConsolePrintNewLine(LOGGING_LEVEL_0);
        if (!(option_flags & NO_DIVIDERS))
        {
            ConsolePrintDivider(LOGGING_LEVEL_0);
        }
        else
        {
            ConsolePrintNewLine(LOGGING_LEVEL_0);
        }
        ConsolePrintNoEol(LOGGING_LEVEL_0, " Selection > ");
        c = ConsoleCheckForKeyBlocking();

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
            ConsolePrintNewLine(LOGGING_LEVEL_0);
            ConsolePrint(LOGGING_LEVEL_0, "Bad selection %c! ", c);
        }
    }
    while (!valid);

    ConsolePrint(LOGGING_LEVEL_0, ANSI_COLOR_GREEN" Selecting %c!"ANSI_COLOR_RESET, c);
    if (!(option_flags & NO_DIVIDERS))
    {
        ConsolePrintDivider(LOGGING_LEVEL_0);
    }

    return c;
}

void ConsolePrint(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePutStringInternal(logging_level, string_buffer);
    ConsolePrintNewLine(logging_level);
}

void ConsolePrintInPlace(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePutStringInternal(logging_level, string_buffer);
    ConsolePutChar('\r');
    fflush(stdout);
}

void ConsolePrintBlock(LoggingLevel_e logging_level, const char *block_string)
{
    unsigned int total_string_len = strlen(block_string);
    unsigned int start = 0;
    unsigned int end = TEXT_BLOCK_SIZE;

    while (start < total_string_len)
    {
        /* Step back until we find a space or we're at the null terminator and
         * we haven't lapped the start */
        while ((block_string[end] != ' ') && (block_string[end] != 0) && (end > start))
        {
            end--;
        }

        /* No space found, we're going to print this entire line */
        if (end == start)
        {
            end = start + TEXT_BLOCK_SIZE;
        }

        /* Print from start to end (add a character margin) */
        ConsolePutChar(' ');
        for (int i = start; i < end; i++)
        {
            ConsolePutChar(block_string[i]);
        }
        ConsolePutChar('\n');

        /* New line */
        start = end + 1;
        /* Make sure we don't go past the string length */
        end = ((start + TEXT_BLOCK_SIZE) > total_string_len) ? total_string_len : start + TEXT_BLOCK_SIZE;
    }
}

void ConsolePrintColor(LoggingLevel_e logging_level, const char *color_string, const char *inner_string)
{
    ConsolePutStringInternal(logging_level, color_string);
    ConsolePutStringInternal(logging_level, inner_string);
    ConsolePutStringInternal(logging_level, ANSI_COLOR_RESET);
    ConsolePrintNewLine(logging_level);

}
void ConsolePrintError(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePrintColor(logging_level, ANSI_COLOR_RED, string_buffer);
}

void ConsolePrintWarn(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePrintColor(logging_level, ANSI_COLOR_YELLOW, string_buffer);
}

void ConsolePrintSuccess(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePrintColor(logging_level, ANSI_COLOR_GREEN, string_buffer);
}

void ConsolePrintNoEol(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePutStringInternal(logging_level, string_buffer);
}

void ConsolePrintNewLine(LoggingLevel_e logging_level)
{
    ConsolePutCharInternal(logging_level, '\r');
    ConsolePutCharInternal(logging_level, '\n');
}

void ConsolePrintHeader(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePrintHeaderInternal(logging_level, DBL_LINE_CHAR, string_buffer);
}

void ConsolePrintSubHeader(LoggingLevel_e logging_level, const char *format, ...)
{
    unsigned char *string_buffer = string_buffers[ConsoleGetStringBufferIndex()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    ConsolePrintHeaderInternal(logging_level, SGL_LINE_CHAR, string_buffer);
}

void ConsolePrintHeaderInternal(LoggingLevel_e logging_level, const char *ruler_string, const char *header_string)
{
    unsigned int string_length = strlen(header_string);

    if (string_length > MAX_HEADER_TITLE_WIDTH)
    {
        string_length = MAX_HEADER_TITLE_WIDTH;
    }
    ConsolePrintNewLine(logging_level);
    ConsolePrintNoEol(logging_level, "%s["ANSI_COLOR_YELLOW" %s "ANSI_COLOR_RESET"]%s", ruler_string, header_string,
                      ruler_string);
    if (!console_settings->small_headers)
    {
        for (unsigned int i = 0; i < (CONSOLE_WIDTH - string_length - HEADER_TITLE_EXTRAS_WIDTH); i++)
        {
            ConsolePrintNoEol(logging_level, "%s", ruler_string);
        }
    }
    ConsolePrintNewLine(logging_level);
    ConsolePrintNewLine(logging_level);
}

void ConsolePrintDivider(LoggingLevel_e logging_level)
{
    for (unsigned int i = 0; i < CONSOLE_WIDTH; i++)
    {
        ConsolePrintNoEol(logging_level, "%s", SGL_LINE_CHAR);
    }
    ConsolePrintNewLine(logging_level);
}

void ConsolePrintMenu(ConsoleMenu_t *menu)
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

    ConsolePrintHeader(LOGGING_LEVEL_0, menu->id.name);
    ConsolePrintNoEol(LOGGING_LEVEL_0, " %s", menu->id.description);

    if (total_pages > 1)
    {
        ConsolePrint(LOGGING_LEVEL_0, " - Page (%i/%i)", menu->current_page + 1, total_pages);
    }
    else
    {
        ConsolePrintNewLine(LOGGING_LEVEL_0);
    }

    if (menu->menu_length)
    {
        ConsolePrintNewLine(LOGGING_LEVEL_0); // Space above the menu options
        // If we have multiple pages and we aren't on the first page, indicate
        // to the user that they can go to the previous page
        if ((total_pages > 1) && (menu->current_page > 0))
        {
            ConsolePrint(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"p"ANSI_COLOR_RESET"] <<< Prev Page");
        }
        for (unsigned int i = start_index; i < end_index; i++)
        {
            ConsoleMenuItem_t *menu_item = &(menu->menu_items[i + menu_offset]);
            ConsolePrintNoEol(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"] %s", '0' + i - list_offset,
                              menu_item->id.name);
            if (menu_item->id.description[0] != '\0')
            {
                ConsolePrint(LOGGING_LEVEL_0, " - %s", menu_item->id.description);
            }
            else
            {
                ConsolePrintNewLine(LOGGING_LEVEL_0);
            }
        }
        // If we have multiple pages and we aren't on the first page, indicate
        // to the user that they can go to the next page
        if ((total_pages > 1) && (menu->current_page < (total_pages - 1)))
        {
            ConsolePrint(LOGGING_LEVEL_0, " ["ANSI_COLOR_YELLOW"n"ANSI_COLOR_RESET"] >>> Next Page");
        }
        ConsolePrintNewLine(LOGGING_LEVEL_0); // Space below the menu options
    }
    else
    {
        ConsolePrintNewLine(LOGGING_LEVEL_0);
        ConsolePrintWarn(LOGGING_LEVEL_0, " <empty like your cup of coffee>");
        ConsolePrintNewLine(LOGGING_LEVEL_0);
    }

}

unsigned int ConsoleGetStringBufferIndex()
{
    unsigned int returned_string_buffer_index = current_string_buffer_index;

    if (++current_string_buffer_index >= NUM_STRING_BUFFERS)
    {
        current_string_buffer_index = 0;
    };

    return returned_string_buffer_index;
}

char ConsoleGetCharInternal(LoggingLevel_e logging_level)
{
    if (console_settings->logging_level >= logging_level)
    {
        return ConsoleGetChar();
    }
    else
    {
        return '\0';
    }
}

void ConsolePutCharInternal(LoggingLevel_e logging_level, char c)
{
    if (console_settings->logging_level >= logging_level)
    {
        ConsolePutChar(c);
    }
}

void ConsolePutStringInternal(LoggingLevel_e logging_level, const char *string)
{
    if (console_settings->logging_level >= logging_level)
    {
        ConsolePutString(string);
    }
}
