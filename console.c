/*
 * MIT License
 *
 * Copyright (c) 2024 Michel Kakulphimp
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
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"

/* Provide a default console settings struct */
static ConsoleSettings_t default_console_settings = {
    /* Setup console interface with default settings */
    .splash_screen_pointer = NULL,
    .main_menu_pointer     = NULL,
    .small_headers         = true, // CLI uses small headers
    .logging_level         = LOGGING_LEVEL_0,
};

static ConsoleSettings_t *console_settings = &default_console_settings;

static const ConsoleSelection_t splash_options[] = {
    {'m', "menus"       },
    {'q', "quit program"}
};
static const ConsoleSelection_t menu_options[] = {
    {'m', "main menu" },
    {'b', "back"      },
    {'n', "next"      },
    {'p', "prev"      },
    {'q', "quit menus"}
};

char         string_buffers[NUM_STRING_BUFFERS][STRING_BUFFER_SIZE];
unsigned int current_string_buffer_index = 0;

/* Maps to TypeEnum_e */
TypeLookupTableEntry_t console_type_lut[TYPE_MAX] = {
    {TYPE_NONE,       "",              0            },
    {TYPE_BOOL_NUM,   "",              sizeof(bool) },
    {TYPE_BOOL_WORD,  "",              sizeof(bool) },
    {TYPE_CHAR,       "%c",            sizeof(char) },
    {TYPE_STRING,     "%s",            0            }, /* Size doesn't make sense for this */
    {TYPE_FLOAT,      "%f",            sizeof(float)},
    {TYPE_DEC_INT8,   "%" PRId8,       1            },
    {TYPE_DEC_INT16,  "%" PRId16,      2            },
    {TYPE_DEC_INT32,  "%" PRId32,      4            },
    {TYPE_DEC_INT64,  "%" PRId64,      8            },
    {TYPE_DEC_UINT8,  "%" PRIu8,       1            },
    {TYPE_DEC_UINT16, "%" PRIu16,      2            },
    {TYPE_DEC_UINT32, "%" PRIu32,      4            },
    {TYPE_DEC_UINT64, "%" PRIu64,      8            },
    {TYPE_HEX_INT4,   "0x%01" PRIx8,   1            },
    {TYPE_HEX_INT8,   "0x%02" PRIx8,   1            },
    {TYPE_HEX_INT12,  "0x%03" PRIx16,  2            },
    {TYPE_HEX_INT16,  "0x%04" PRIx16,  2            },
    {TYPE_HEX_INT20,  "0x%05" PRIx32,  4            },
    {TYPE_HEX_INT24,  "0x%06" PRIx32,  4            },
    {TYPE_HEX_INT28,  "0x%07" PRIx32,  4            },
    {TYPE_HEX_INT32,  "0x%08" PRIx32,  4            },
    {TYPE_HEX_INT36,  "0x%09" PRIx64,  8            },
    {TYPE_HEX_INT40,  "0x%010" PRIx64, 8            },
    {TYPE_HEX_INT44,  "0x%011" PRIx64, 8            },
    {TYPE_HEX_INT48,  "0x%012" PRIx64, 8            },
    {TYPE_HEX_INT52,  "0x%013" PRIx64, 8            },
    {TYPE_HEX_INT56,  "0x%014" PRIx64, 8            },
    {TYPE_HEX_INT60,  "0x%015" PRIx64, 8            },
    {TYPE_HEX_INT64,  "0x%016" PRIx64, 8            },
    {TYPE_HEX_UINT4,  "0x%01" PRIx8,   1            },
    {TYPE_HEX_UINT8,  "0x%02" PRIx8,   1            },
    {TYPE_HEX_UINT12, "0x%03" PRIx16,  2            },
    {TYPE_HEX_UINT16, "0x%04" PRIx16,  2            },
    {TYPE_HEX_UINT20, "0x%05" PRIx32,  4            },
    {TYPE_HEX_UINT24, "0x%06" PRIx32,  4            },
    {TYPE_HEX_UINT28, "0x%07" PRIx32,  4            },
    {TYPE_HEX_UINT32, "0x%08" PRIx32,  4            },
    {TYPE_HEX_UINT36, "0x%09" PRIx64,  8            },
    {TYPE_HEX_UINT40, "0x%010" PRIx64, 8            },
    {TYPE_HEX_UINT44, "0x%011" PRIx64, 8            },
    {TYPE_HEX_UINT48, "0x%012" PRIx64, 8            },
    {TYPE_HEX_UINT52, "0x%013" PRIx64, 8            },
    {TYPE_HEX_UINT56, "0x%014" PRIx64, 8            },
    {TYPE_HEX_UINT60, "0x%015" PRIx64, 8            },
    {TYPE_HEX_UINT64, "0x%016" PRIx64, 8            },
};

void console_init(ConsoleSettings_t *settings)
{
    memset(string_buffers, 0, sizeof(char) * NUM_STRING_BUFFERS * STRING_BUFFER_SIZE);
    current_string_buffer_index = 0;
    console_settings            = settings;

    /* Call the OS-specific init function if it's defined */
    if (settings->os_init_fn)
    {
        settings->os_init_fn();
    }
}

void console_main(void)
{
    char selection;

    for (;;)
    {
        console_print_new_line(LOGGING_LEVEL_0);
        console_print_new_line(LOGGING_LEVEL_0);
        console_print_header(LOGGING_LEVEL_0, "Welcome");
        for (unsigned int line = 0; line < CONSOLE_HEIGHT; line++)
        {
            if (console_settings->splash_screen_pointer)
            {
                if (!strcmp((*(console_settings->splash_screen_pointer))[line], ""))
                {
                    break;
                }
                console_print(LOGGING_LEVEL_0, "%s", (*(console_settings->splash_screen_pointer))[line]);
            }
        }
        selection = console_print_options_and_get_response(splash_options, SELECTION_SIZE(splash_options), 0, 0);

        switch (selection)
        {
            case 'm':
                if (console_settings->main_menu_pointer)
                {
                    console_traverse_menus(console_settings->main_menu_pointer);
                }
                else
                {
                    console_print_error(LOGGING_LEVEL_0, "%s: No menu pointer defined!", __FUNCTION__);
                }
                break;
            case 'o':
                console_print_error(LOGGING_LEVEL_0, "%s: Options not implemented.", __FUNCTION__);
                break;
            case 'q':
                console_print(LOGGING_LEVEL_0, ANSI_COLOR_CYAN " Bye-bye!\n" ANSI_COLOR_RESET);
                return;
                break;
            default:
                console_print_error(LOGGING_LEVEL_0, "%s: Something went wrong...", __FUNCTION__);
                for (;;)
                    ;
                break;
        };
    }
}

void console_small_headers(bool enable) { console_settings->small_headers = enable; }

unsigned int console_prompt_for_int(const char *prompt, unsigned int default_val)
{
    char         buffer[100];
    unsigned int input;

    console_print_no_eol(LOGGING_LEVEL_0, "%s (default: %d) > ", prompt, default_val);

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

uint32_t console_prompt_for_hex_uint32(const char *prompt, uint32_t default_val)
{
    char     buffer[100];
    uint32_t input;

    console_print_no_eol(LOGGING_LEVEL_0, "%s (default: 0x%x) > ", prompt, default_val);

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

uint64_t console_prompt_for_hex_uint64(const char *prompt, uint64_t default_val)
{
    char     buffer[100];
    uint64_t input;

    console_print_no_eol(LOGGING_LEVEL_0, "%s (default: 0x%x) > ", prompt, default_val);

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return default_val;
    }

    if (sscanf(buffer, "%" SCNu64, &input) != 1)
    {
        return default_val;
    }

    return input;
}

char *console_prompt_for_string(const char *prompt, const char *default_val)
{
    char      *char_buffer   = string_buffers[console_get_string_buffer_index()];
    char      *string_buffer = string_buffers[console_get_string_buffer_index()];
    const char none_string[] = "None";

    if (!default_val)
    {
        default_val = none_string;
        console_print_no_eol(LOGGING_LEVEL_0, "%s > ", prompt);
    }
    else
    {
        console_print_no_eol(LOGGING_LEVEL_0, "%s (default: %s) > ", prompt, default_val);
    }

    if (fgets(char_buffer, STRING_BUFFER_SIZE, stdin) == NULL)
    {
        strcpy(string_buffer, default_val);
    }

    if (sscanf(char_buffer, "%s", string_buffer) != 1)
    {
        strcpy(string_buffer, default_val);
    }

    return string_buffer;
}

void console_prompt_for_any_keys_blocking(void)
{
    console_print(LOGGING_LEVEL_0, "Press any key to continue");
    console_check_for_key_blocking();
}

char console_check_for_key_blocking(void)
{
    char c = 0;

    while (c == 0)
    {
        c = console_get_char_internal(LOGGING_LEVEL_0);
    }

    return c;
}

char console_check_for_key(void) { return console_get_char_internal(LOGGING_LEVEL_0); }

void console_traverse_menus(ConsoleMenu_t *menu)
{
    bool           stay_put     = true;
    ConsoleMenu_t *current_menu = menu;
    char           selection;
    char          *char_pointer;
    unsigned int   total_pages;
    unsigned int   num_selections;
    unsigned int   selected_index;

    do
    {
        /* Mutable menus need a function to update them, call it prior to printing the menu out so it's updated. */
        if (current_menu->mode == MENU_MUTABLE)
        {
            /* ToDo: Assert on null pointer. */
            current_menu->updater();
        }

        /* Determine maximum selection is for this page */
        if (((current_menu->current_page * PAGE_LENGTH) + PAGE_LENGTH) > current_menu->menu_length)
        {
            num_selections = current_menu->menu_length - (current_menu->current_page * PAGE_LENGTH);
        }
        else
        {
            num_selections = PAGE_LENGTH;
        }

        /* Determine total pages for current menu (do this after a potential menu update) */
        total_pages = TOTAL_PAGES(current_menu->menu_length);

        console_print_menu(current_menu);
        selection = console_print_options_and_get_response(menu_options, SELECTION_SIZE(menu_options), num_selections, 0);

        selected_index = ((current_menu->current_page * PAGE_LENGTH) + selection - '0');

        /* First check if it's a menu selection (selection should be valid) */
        if ((unsigned int)selection < (unsigned int)(num_selections + '0'))
        {
            /* Error out if we have neither */
            if ((current_menu->menu_items[selected_index].function_pointer == NO_FUNCTION_POINTER) && (current_menu->menu_items[selected_index].sub_menu == NO_SUB_MENU))
            {
                console_print(LOGGING_LEVEL_0, ANSI_COLOR_RED " No submenu or function pointer!!!" ANSI_COLOR_RESET);
                // for (;;);
            }

            /**
             * Note:    A menu item can have both a submenu as well as a function pointer.
             *          Useful if a function needs to be called prior to entering a new menu.
             * Check if we have a function pointer
             */
            if (current_menu->menu_items[selected_index].function_pointer != NO_FUNCTION_POINTER)
            {
                /* ToDo: Handle arguments. */
                if (MENU_MUTABLE)
                {
                    /**
                     * Pass menu name to first argument of function if menu is mutable
                     * ToDo: Either insert name at beginning or the end of the argument list when arguments are supported
                     */
                    char_pointer = current_menu->menu_items[selected_index].id.name;
                    current_menu->menu_items[selected_index].function_pointer(1, &char_pointer);
                }
                else
                {
                    current_menu->menu_items[selected_index].function_pointer(NO_ARGS, NO_ARGS);
                }
                /* We stay put after executing a function */
                /* ToDo: Print function return status */
            }

            /* Check if we have a submenu */
            if (current_menu->menu_items[selected_index].sub_menu != NO_SUB_MENU)
            {
                current_menu = current_menu->menu_items[selected_index].sub_menu;
                /* Note! After this point, we've changed menus */
            }
        }
        /* Check if we're traversing up */
        else if (selection == 'b')
        {
            /* Go up if we can */
            if (current_menu->parent_menu != NO_MAIN_MENU)
            {
                /* Reset page */
                current_menu->current_page = 0;
                current_menu               = current_menu->parent_menu;
            }
        }
        /* Check if we're traversing to top */
        else if (selection == 'm')
        {
            /* Go up until we hit top menu */
            while (current_menu->parent_menu != NO_MAIN_MENU)
            {
                /* Reset page */
                current_menu->current_page = 0;
                current_menu               = current_menu->parent_menu;
            };
        }
        /* Check if we're going to the previous page */
        else if (selection == 'p')
        {
            /* Only valid if we have more than one page and we aren't on the first page */
            if ((total_pages > 1) && (current_menu->current_page > 0))
            {
                current_menu->current_page--;
            }
        }
        /* Check if we're going to the next page */
        else if (selection == 'n')
        {
            /* Only valid if we have more than one page and we aren't on the last page */
            if ((total_pages > 1) && (current_menu->current_page < (total_pages - 1)))
            {
                current_menu->current_page++;
            }
        }
        /* Check if we're quitting */
        else if (selection == 'q')
        {
            stay_put = false;
        }
        else
        {
            console_print(LOGGING_LEVEL_0, ANSI_COLOR_RED " Something went wrong..." ANSI_COLOR_RESET);
            for (;;)
                ;
        }
    } while (stay_put);
}

char console_print_options_and_get_response(const ConsoleSelection_t selections[], unsigned int num_selections, unsigned int num_menu_selections, unsigned int option_flags)
{
    /* ToDo: Assert on number of menu selections greater than 10 */
    char c;
    bool valid = false;

    do
    {
        if (!(option_flags & NO_DIVIDERS))
        {
            console_print_divider(LOGGING_LEVEL_0);
        }
        else
        {
            console_print_new_line(LOGGING_LEVEL_0);
        }
        /* Print menu selections (these will override any conflicting passed in selections) */
        if (num_menu_selections != 0)
        {
            console_print_no_eol(LOGGING_LEVEL_0, " [" ANSI_COLOR_YELLOW "0" ANSI_COLOR_RESET "-" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET "]-item ", ('0' + (num_menu_selections - 1)));
        }
        /* Print passed in selections */
        for (unsigned int i = 0; i < num_selections; i++)
        {
            console_print_no_eol(LOGGING_LEVEL_0, " [" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET "]-%s ", selections[i].key, selections[i].description);
            if ((option_flags & ORIENTATION_V) && !(i == (num_selections - 1)))
            {
                console_print_new_line(LOGGING_LEVEL_0);
            }
        }
        console_print_new_line(LOGGING_LEVEL_0);
        if (!(option_flags & NO_DIVIDERS))
        {
            console_print_divider(LOGGING_LEVEL_0);
        }
        else
        {
            console_print_new_line(LOGGING_LEVEL_0);
        }
        console_print_no_eol(LOGGING_LEVEL_0, " Selection > ");
        c = console_check_for_key_blocking();

        /* If we have menu selections, check for those first */
        if (num_menu_selections != 0)
        {
            /* Check if it's a valid menu selection */
            if ((unsigned int)(c - '0') < num_menu_selections)
            {
                valid = true;
            }
        }

        /* We didn't get a valid value yet */
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
            console_print_new_line(LOGGING_LEVEL_0);
            console_print(LOGGING_LEVEL_0, "Bad selection %c! ", c);
        }
    } while (!valid);

    console_print(LOGGING_LEVEL_0, ANSI_COLOR_GREEN " Selecting %c!" ANSI_COLOR_RESET, c);
    if (!(option_flags & NO_DIVIDERS))
    {
        console_print_divider(LOGGING_LEVEL_0);
    }

    return c;
}

void console_print(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_put_string_internal(logging_level, string_buffer);
    console_print_new_line(logging_level);
}

void console_print_in_place(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_put_string_internal(logging_level, "\33[2K"); /* Clear out the line*/
    console_put_string_internal(logging_level, string_buffer);
    console_put_char_internal(logging_level, '\r');
    fflush(stdout);
}

void console_print_block(LoggingLevel_e logging_level, const char *block_string)
{
    size_t total_string_len      = console_isprint_str_len(block_string);
    size_t line_start            = 0;
    size_t line_end              = 0;
    size_t line_end_space        = 0;
    size_t printable_line_length = 0;
    bool   inside_escape_code    = false;

    while (line_start < total_string_len)
    {
        line_end              = line_start;
        printable_line_length = 0;
        /* Extract a line up to TEXT_BLOCK_SIZE of printable characters */
        while (block_string[line_end] != 0)
        {
            /* Ignore ANSI escape sequences */
            if (block_string[line_end] == '\x1b')
            {
                inside_escape_code = true;
            }
            else if (inside_escape_code && ((block_string[line_end] == 'm') || (block_string[line_end] == 'J')))
            {
                inside_escape_code = false;
            }
            else if (!inside_escape_code && isprint(block_string[line_end]))
            {
                printable_line_length++;
            }
            line_end++;

            if (printable_line_length >= TEXT_BLOCK_SIZE)
            {
                break;
            }
        }

        /* Step back until we find a space  and we haven't lapped the line_start.*/
        if (printable_line_length >= TEXT_BLOCK_SIZE)
        {
            line_end_space = line_end;
            while ((block_string[line_end_space] != ' ') && (line_end_space > line_start))
            {
                line_end_space--;
            }

            /* If we find a space, print up to that space. */
            if (line_end_space != line_start)
            {
                line_end = line_end_space;
            }
        }

        /* Print from line_start to line_end (add a character margin) */
        console_put_char_internal(logging_level, ' ');
        for (size_t i = line_start; i < line_end; i++)
        {
            if (!block_string[i])
            {
                /* NULL! We're done here. */
                break;
            }

            console_put_char_internal(logging_level, block_string[i]);
        }
        console_put_char_internal(logging_level, '\n');

        /* New line */
        line_start = line_end + 1;
    }
}

void console_print_color(LoggingLevel_e logging_level, const char *color_string, const char *inner_string)
{
    console_put_string_internal(logging_level, color_string);
    console_put_string_internal(logging_level, inner_string);
    console_put_string_internal(logging_level, ANSI_COLOR_RESET);
    console_print_new_line(logging_level);
}

void console_print_color_no_eol(LoggingLevel_e logging_level, const char *color_string, const char *inner_string)
{
    console_put_string_internal(logging_level, color_string);
    console_put_string_internal(logging_level, inner_string);
    console_put_string_internal(logging_level, ANSI_COLOR_RESET);
}

void console_print_debug(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_color(logging_level, ANSI_COLOR_MAGENTA, string_buffer);
}

void console_print_debug_no_eol(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_color_no_eol(logging_level, ANSI_COLOR_MAGENTA, string_buffer);
}

void console_print_debug_new_line(LoggingLevel_e logging_level)
{
    console_put_string_internal(logging_level, "\r\n");
}

void console_print_error(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_color(logging_level, ANSI_COLOR_RED, string_buffer);
}

void console_print_warn(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_color(logging_level, ANSI_COLOR_YELLOW, string_buffer);
}

void console_print_success(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_color(logging_level, ANSI_COLOR_GREEN, string_buffer);
}

void console_print_no_eol(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_put_string_internal(logging_level, string_buffer);
}

void console_print_new_line(LoggingLevel_e logging_level)
{
    console_put_string_internal(logging_level, "\r\n");
}

void console_print_header(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_header_internal(logging_level, DBL_LINE_CHAR, string_buffer);
}

void console_print_sub_header(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_header_internal(logging_level, SGL_LINE_CHAR, string_buffer);
}

void console_print_footer_banner(LoggingLevel_e logging_level, const char *format, ...)
{
    char   *string_buffer = string_buffers[console_get_string_buffer_index()];
    va_list args;
    va_start(args, format);
    vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
    va_end(args);
    console_print_divider(logging_level);
    console_put_string_internal(logging_level, "  "); /* Add some space */
    console_put_string_internal(logging_level, string_buffer);
    console_print_divider(logging_level);
}

void console_print_header_internal(LoggingLevel_e logging_level, const char *ruler_string, const char *header_string)
{
    unsigned int is_print_string_length = console_isprint_str_len(header_string);

    if (is_print_string_length > MAX_HEADER_TITLE_WIDTH)
    {
        is_print_string_length = MAX_HEADER_TITLE_WIDTH;
    }
    console_print_new_line(logging_level);
    console_print_no_eol(logging_level, "%s[" ANSI_COLOR_YELLOW " %s " ANSI_COLOR_RESET "]%s", ruler_string, header_string, ruler_string);
    if (!console_settings->small_headers)
    {
        for (unsigned int i = 0; i < (CONSOLE_WIDTH - is_print_string_length - HEADER_TITLE_EXTRAS_WIDTH); i++)
        {
            console_print_no_eol(logging_level, "%s", ruler_string);
        }
    }
    console_print_new_line(logging_level);
    console_print_new_line(logging_level);
}

void console_print_divider(LoggingLevel_e logging_level)
{
    for (unsigned int i = 0; i < CONSOLE_WIDTH; i++)
    {
        console_print_no_eol(logging_level, "%s", SGL_LINE_CHAR);
    }
    console_print_new_line(logging_level);
}

void console_print_menu(ConsoleMenu_t *menu)
{
    unsigned int total_pages = TOTAL_PAGES(menu->menu_length);
    unsigned int start_index;
    unsigned int end_index;
    unsigned int menu_offset;
    unsigned int list_offset;
    const char   menu_breadcrumb_separator[]                               = BREADCRUMB_SEPARATOR;
    char         menu_breadcrumb_string[MAX_MENU_DESCRIPTION_LENGTH + 100] = {0}; /* Adding a bit of breathing room for the breadcrumb to move */

    /* Make breadcrumbs */
    ConsoleMenu_t *menu_breadcrumb = menu;
    snprintf(menu_breadcrumb_string, sizeof(menu_breadcrumb_string), "%s" ANSI_COLOR_RESET, menu_breadcrumb->id.name);
    do
    {
        /* Go up a level */
        menu_breadcrumb = menu_breadcrumb->parent_menu;
        if (menu_breadcrumb)
        {
            size_t parent_menu_name_length = strlen(menu_breadcrumb->id.name);
            size_t separator_length        = strlen(menu_breadcrumb_separator);
            memmove(menu_breadcrumb_string + parent_menu_name_length + separator_length, menu_breadcrumb_string, strlen(menu_breadcrumb_string));
            strncpy(menu_breadcrumb_string, menu_breadcrumb->id.name, parent_menu_name_length);
            strncpy(menu_breadcrumb_string + parent_menu_name_length, menu_breadcrumb_separator, strlen(menu_breadcrumb_separator));
        }
    } while (menu_breadcrumb);

    /**
     * Mutable menus update dynamically, so no need to offset reading the menu array
     * ToDo: there HAS to be a better way of doing this
     */

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

    /* Calculate start and end indices */
    start_index = (menu->current_page * PAGE_LENGTH) - menu_offset;
    end_index   = start_index + PAGE_LENGTH;
    if (end_index > (menu->menu_length - menu_offset))
    {
        end_index = (menu->menu_length - menu_offset);
    }

    console_print_header(LOGGING_LEVEL_0, "%s", menu_breadcrumb_string);
    console_print_no_eol(LOGGING_LEVEL_0, " %s", menu->id.description);

    if (total_pages > 1)
    {
        console_print(LOGGING_LEVEL_0, " - Page (%i/%i)", menu->current_page + 1, total_pages);
    }
    else
    {
        console_print_new_line(LOGGING_LEVEL_0);
    }

    if (menu->menu_length)
    {
        /* Space above the menu options */
        console_print_new_line(LOGGING_LEVEL_0);
        /**
         * If we have multiple pages and we aren't on the first page, indicate
         * to the user that they can go to the previous page
         */
        if ((total_pages > 1) && (menu->current_page > 0))
        {
            console_print(LOGGING_LEVEL_0, " [" ANSI_COLOR_YELLOW "p" ANSI_COLOR_RESET "] <<< Prev Page");
        }
        for (unsigned int i = start_index; i < end_index; i++)
        {
            ConsoleMenuItem_t *menu_item = &(menu->menu_items[i + menu_offset]);
            console_print_no_eol(LOGGING_LEVEL_0, " [" ANSI_COLOR_YELLOW "%c" ANSI_COLOR_RESET "] %s", '0' + i - list_offset, menu_item->id.name);
            if (menu_item->id.description[0] != '\0')
            {
                console_print(LOGGING_LEVEL_0, " - %s", menu_item->id.description);
            }
            else
            {
                console_print_new_line(LOGGING_LEVEL_0);
            }
        }
        /**
         * If we have multiple pages and we aren't on the first page, indicate
         * to the user that they can go to the next page
         */
        if ((total_pages > 1) && (menu->current_page < (total_pages - 1)))
        {
            console_print(LOGGING_LEVEL_0, " [" ANSI_COLOR_YELLOW "n" ANSI_COLOR_RESET "] >>> Next Page");
        }
        /* Space below the menu options */
        console_print_new_line(LOGGING_LEVEL_0);
    }
    else
    {
        console_print_new_line(LOGGING_LEVEL_0);
        console_print_warn(LOGGING_LEVEL_0, " <empty like your cup of coffee>");
        console_print_new_line(LOGGING_LEVEL_0);
    }
}

unsigned int console_get_string_buffer_index()
{

    unsigned int returned_string_buffer_index = current_string_buffer_index;

    if (++current_string_buffer_index >= NUM_STRING_BUFFERS)
    {
        current_string_buffer_index = 0;
    };

    return returned_string_buffer_index;
}

LoggingLevel_e console_get_logging_level()
{
    return console_settings->logging_level;
}

char console_get_char_internal(LoggingLevel_e logging_level)
{
    if (console_settings->logging_level >= logging_level)
    {
        return console_settings->get_char_fn();
    }
    else
    {
        return '\0';
    }
}

void console_put_char_internal(LoggingLevel_e logging_level, char c)
{
    if (console_settings->logging_level >= logging_level)
    {
        console_settings->put_char_fn(c);
    }
}

void console_put_string_internal(LoggingLevel_e logging_level, const char *string)
{
    if (console_settings->logging_level >= logging_level)
    {
        console_settings->put_string_fn(string);
    }
}

size_t console_isprint_str_len(const char *str)
{
    size_t len                = 0;
    bool   inside_escape_code = false;

    while (*str)
    {
        unsigned char c = *str;
        if (c == '\x1b')
        {
            inside_escape_code = true;
        }
        else if (inside_escape_code && ((c == 'm') || (c == 'J')))
        {
            inside_escape_code = false;
        }
        else if (!inside_escape_code && isprint(c))
        {
            ++len;
        }
        ++str;
    }

    return len;
}

void console_assert_warn(LoggingLevel_e logging_level, bool condition, const char *format, ...)
{
    if (!condition)
    {
        char   *string_buffer = string_buffers[console_get_string_buffer_index()];
        va_list args;
        va_start(args, format);
        vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
        va_end(args);
        console_print(logging_level, ANSI_COLOR_RED "Assert Warning: %s" ANSI_COLOR_RESET, string_buffer);
    }
}

FunctionResult_e console_assert_error(LoggingLevel_e logging_level, bool condition, const char *format, ...)
{
    FunctionResult_e result = FR_OK;

    if (!condition)
    {
        char   *string_buffer = string_buffers[console_get_string_buffer_index()];
        va_list args;
        va_start(args, format);
        vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
        va_end(args);
        console_print(logging_level, ANSI_COLOR_RED "Assert Error: %s" ANSI_COLOR_RESET, string_buffer);
        result = FR_FAIL;
    }

    return result;
}

void console_assert_fatal(LoggingLevel_e logging_level, bool condition, const char *format, ...)
{
    if (!condition)
    {
        char   *string_buffer = string_buffers[console_get_string_buffer_index()];
        va_list args;
        va_start(args, format);
        vsnprintf(string_buffer, STRING_BUFFER_SIZE, format, args);
        va_end(args);
        console_print(logging_level, ANSI_COLOR_RED "Assert Fatal: %s - Program exiting!" ANSI_COLOR_RESET, string_buffer);
        exit(FR_FAIL);
    }
}

/**
 * @brief   This utility prints a table divider for use with the table printer
 *
 * @param   logging_level   The logging level for the print
 * @param   column_widths   The widths of each column
 * @param   num_columns     The number of columns
 */
void console_print_table_divider(LoggingLevel_e logging_level, size_t *column_widths, int num_columns)
{
    for (int i = 0; i < num_columns; i++)
    {
        console_print_no_eol(logging_level, "+");
        for (size_t j = 0; j < column_widths[i] + 1; j++)
        {
            console_print_no_eol(logging_level, "-");
        }
    }
    console_print(logging_level, "+");
}

/**
 * @brief   This is a utility function for the table printing function to format a void* pointer to a specific type.
 *
 * @param buffer        The string buffer to write into
 * @param buff_size     The string buffer size
 * @param type          The type to treat the data as
 * @param data          The pointer to the data
 * @param formats       The pointer to the cell formats
 * @param index         The index of the data
 */
void console_sprint_table_format_entry(char *buffer, size_t buff_size, TypeEnum_e type, void *data, TableCellOptions_t *options, int index)
{

    buffer[0]                = '\0';
    size_t offset            = 0;
    size_t updated_buff_size = buff_size;
    bool   skip_snprintf     = false;

    /* Allow no formatting by passing in a NULL pointer */
    if (options)
    {
        switch (options[index].highlight)
        {
            /* Prepend the color bytes if we're highlighting */
            /* Updated buffer size will now be shrunk by the bytes it took to make the color, and the bytes it took to reset
            it. */
            case TABLE_CELL_HIGHLIGHT_RED:
                strncat(buffer, ANSI_COLOR_RED, buff_size);
                offset = sizeof(ANSI_COLOR_RED) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_GREEN:
                strncat(buffer, ANSI_COLOR_GREEN, buff_size);
                offset = sizeof(ANSI_COLOR_GREEN) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_YELLOW:
                strncat(buffer, ANSI_COLOR_YELLOW, buff_size);
                offset = sizeof(ANSI_COLOR_YELLOW) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_BLUE:
                strncat(buffer, ANSI_COLOR_BLUE, buff_size);
                offset = sizeof(ANSI_COLOR_BLUE) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_MAGENTA:
                strncat(buffer, ANSI_COLOR_MAGENTA, buff_size);
                offset = sizeof(ANSI_COLOR_MAGENTA) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_CYAN:
                strncat(buffer, ANSI_COLOR_CYAN, buff_size);
                offset = sizeof(ANSI_COLOR_CYAN) - 1;
                updated_buff_size -= (offset - sizeof(ANSI_COLOR_RESET));
                break;
            case TABLE_CELL_HIGHLIGHT_NONE:
            default:
                break;
        }

        if (options[index].options & TABLE_CELL_OPTIONS_BLANK)
        {
            buffer[1]     = '\0';
            skip_snprintf = true;
        }
    }

    if (!skip_snprintf)
    {
        switch (type)
        {
            case TYPE_NONE:
                buffer[0] = 0;
                break;
            case TYPE_BOOL_NUM:
                if (((bool *)data)[index])
                {
                    snprintf(buffer + offset, updated_buff_size, ANSI_COLOR_RESET ANSI_COLOR_CYAN "1" ANSI_COLOR_RESET);
                }
                else
                {
                    snprintf(buffer + offset, updated_buff_size, ANSI_COLOR_RESET ANSI_COLOR_MAGENTA "0" ANSI_COLOR_RESET);
                }
                break;
            case TYPE_BOOL_WORD:
                if (((bool *)data)[index])
                {
                    snprintf(buffer + offset, updated_buff_size, ANSI_COLOR_RESET ANSI_COLOR_CYAN "true" ANSI_COLOR_RESET);
                }
                else
                {
                    snprintf(buffer + offset, updated_buff_size, ANSI_COLOR_RESET ANSI_COLOR_MAGENTA "false" ANSI_COLOR_RESET);
                }
                break;
            case TYPE_CHAR:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((char *)data)[index]);
                break;
            case TYPE_STRING:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((char **)data)[index]);
                break;
            case TYPE_FLOAT:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((float *)data)[index]);
                break;
            case TYPE_DEC_INT8:
            case TYPE_HEX_INT4:
            case TYPE_HEX_INT8:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((int8_t *)data)[index]);
                break;
            case TYPE_DEC_INT16:
            case TYPE_HEX_INT12:
            case TYPE_HEX_INT16:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((int16_t *)data)[index]);
                break;
            case TYPE_DEC_INT32:
            case TYPE_HEX_INT20:
            case TYPE_HEX_INT24:
            case TYPE_HEX_INT28:
            case TYPE_HEX_INT32:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((int32_t *)data)[index]);
                break;
            case TYPE_DEC_INT64:
            case TYPE_HEX_INT36:
            case TYPE_HEX_INT40:
            case TYPE_HEX_INT44:
            case TYPE_HEX_INT48:
            case TYPE_HEX_INT52:
            case TYPE_HEX_INT56:
            case TYPE_HEX_INT60:
            case TYPE_HEX_INT64:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((int64_t *)data)[index]);
                break;
            case TYPE_DEC_UINT8:
            case TYPE_HEX_UINT4:
            case TYPE_HEX_UINT8:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((uint8_t *)data)[index]);
                break;
            case TYPE_DEC_UINT16:
            case TYPE_HEX_UINT12:
            case TYPE_HEX_UINT16:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((uint16_t *)data)[index]);
                break;
            case TYPE_DEC_UINT32:
            case TYPE_HEX_UINT20:
            case TYPE_HEX_UINT24:
            case TYPE_HEX_UINT28:
            case TYPE_HEX_UINT32:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((uint32_t *)data)[index]);
                break;
            case TYPE_DEC_UINT64:
            case TYPE_HEX_UINT36:
            case TYPE_HEX_UINT40:
            case TYPE_HEX_UINT44:
            case TYPE_HEX_UINT48:
            case TYPE_HEX_UINT52:
            case TYPE_HEX_UINT56:
            case TYPE_HEX_UINT60:
            case TYPE_HEX_UINT64:
                snprintf(buffer + offset, updated_buff_size, console_type_lut[type].format_string, ((uint64_t *)data)[index]);
                break;
            default:
                console_print_error(LOGGING_LEVEL_0, "%s: Unsupported value of TypeEnum_e detected! (%d)", __FUNCTION__, type);
                break;
        }

        strncat(buffer + offset, ANSI_COLOR_RESET, updated_buff_size);
    }
}

/**
 * @brief  This utility returns a pointer to an array of TableCellOptions_t structs. The pointer returned must be freed.
 *
 */
TableCellOptions_t *console_get_table_cell_options_array(int num_rows, TableCellOptions_e default_options, TableCellHighlight_e default_highlight)
{
    TableCellOptions_t *options = (TableCellOptions_t *)malloc(sizeof(TableCellOptions_t) * num_rows);
    memset(options, 0, sizeof(TableCellOptions_t) * num_rows);

    for (int i = 0; i < num_rows; i++)
    {
        options[i].options   = default_options;
        options[i].highlight = default_highlight;
    }

    return options;
}

/**
 * @brief   This utility prints a table, constructed using TableColumn_t structs that make up the table.
 *
 * @param logging_level     The logging level for the print
 * @param num_rows          The number of rows of the table
 * @param num_columns       The number of columns of the table
 * @param ...               For every column, a reference to TableColumn_t is provided as a variable arg
 */
void console_print_table(LoggingLevel_e logging_level, int num_rows, int num_columns, ...)
{
    char    buffer[STRING_BUFFER_SIZE];
    va_list args;

    /* Prepare to calculate the maximum size of each column */
    size_t *column_widths = (size_t *)malloc(sizeof(size_t) * num_columns);
    memset(column_widths, 0, sizeof(size_t) * num_columns);

    /* Start args to calculate the maximum length of each column's content. */
    /* All calculations add + 1 so that we have some padding. */
    va_start(args, num_columns);
    for (int i = 0; i < num_columns; i++)
    {
        TableColumn_t *table_column = va_arg(args, TableColumn_t *);
        if (!table_column)
        {
            console_print_error(LOGGING_LEVEL_0, "%s: table_column pointer is null!", __FUNCTION__);
            break;
        }

        /* Header width */
        if (console_isprint_str_len(table_column->header) > column_widths[i])
        {
            column_widths[i] = console_isprint_str_len(table_column->header) + 1;
        }
        /* Column content widths. We calculate the widths by actually formatting and then taking the lengths. */
        for (int j = 0; j < num_rows; j++)
        {
            console_sprint_table_format_entry(buffer, sizeof(buffer), table_column->type, table_column->values, table_column->options, j);

            if (console_isprint_str_len(buffer) + 1 > column_widths[i])
            {
                column_widths[i] = console_isprint_str_len(buffer) + 1;
            }
        }
    }
    va_end(args);

    /* Print divider */
    console_print_table_divider(logging_level, column_widths, num_columns);

    /* Print headers */
    va_start(args, num_columns);
    for (int i = 0; i < num_columns; i++)
    {
        TableColumn_t *table_column = va_arg(args, TableColumn_t *);
        console_print_no_eol(logging_level, "| %-*s", column_widths[i], table_column->header);
    }
    console_print(logging_level, "|");
    va_end(args);

    /* Print divider */
    console_print_table_divider(logging_level, column_widths, num_columns);

    /* Print row labels and corresponding column values */
    va_start(args, num_columns);
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_columns; j++)
        {
            TableColumn_t *table_column = va_arg(args, TableColumn_t *);

            console_print_no_eol(logging_level, "| ");

            console_sprint_table_format_entry(buffer, sizeof(buffer), table_column->type, table_column->values, table_column->options, i);

            console_print_no_eol(logging_level, "%s", buffer);

            bool print_dots = true;
            for (size_t k = console_isprint_str_len(buffer); k < column_widths[j]; k++)
            {
                if (table_column->options)
                {
                    if ((table_column->options[i].options & TABLE_CELL_OPTIONS_NO_DOTS))
                    {
                        print_dots = false;
                    }
                }

                if (print_dots)
                {
                    console_print_no_eol(logging_level, ".");
                }
                else
                {
                    console_print_no_eol(logging_level, " ");
                }
            }
        }
        console_print(logging_level, "|");

        /* reset args to the beginning */
        va_end(args);
        va_start(args, num_columns);
    }
    va_end(args);

    /* Print divider */
    console_print_table_divider(logging_level, column_widths, num_columns);

    free(column_widths);
}