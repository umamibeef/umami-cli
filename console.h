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

#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif

// Console ANSI colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ERASE_SCREEN       "\x1b[2J"

typedef enum
{
    ERROR = -1,
    SUCCESS =   0,
} FunctionResult_e;

typedef enum
{
    MENU_DEFAULT = 0, // Default menu behavior, immutable definition
    MENU_MUTABLE = 1, // Menu is dynamically populated, consider it mutable
} ConsoleMenuMode_e;

typedef enum
{
    LOGGING_LEVEL_DISABLED = -1,
    LOGGING_LEVEL_0 = 0, 
    LOGGING_LEVEL_1 = 1, 
    LOGGING_LEVEL_2 = 2, 
    LOGGING_LEVEL_3 = 3, 
} LoggingLevel_e;

#define DBL_LINE_CHAR               "═"
#define SGL_LINE_CHAR               "─"

#define NO_TOP_MENU                 (0) // NULL
#define NO_SUB_MENU                 (0) // NULL
#define NO_FUNCTION_POINTER         (0) // NULL
#define NO_ARGS                     (0) // NULL
#define NO_SIZE                     (0)
#define MAX_MENU_NAME_LENGTH        (40)
#define MAX_MENU_DESCRIPTION_LENGTH (40)
#define CONSOLE_WIDTH               (120)
#define STRING_BUFFER_SIZE          (100)
#define HEADER_TITLE_EXTRAS_WIDTH   (6) // "=[  ]=" = 6 characters
#define MAX_HEADER_TITLE_WIDTH      (CONSOLE_WIDTH - HEADER_TITLE_EXTRAS_WIDTH) 
#define PAGE_LENGTH                 (10) // Maximum length of a page (0-9)
#define FIRST_PAGE                  (0) // Pages are zero indexed

#define MENU_SIZE(x)                sizeof(x)/sizeof(ConsoleMenuItem_t)
#define SELECTION_SIZE(x)           sizeof(x)/sizeof(ConsoleSelection_t)
#define TOTAL_PAGES(x)              ((x/(PAGE_LENGTH)) + (x % PAGE_LENGTH != 0))
#define IGNORE_UNUSED()             (void)(argc);(void)(argv);

#define _STR(x) #x
#define STR(x) _STR(x)

// User should define a splash screen as an as array of const pointer to const char.
typedef const char *const Splash_t[];

typedef struct ConsoleMenuId
{
    char                name[MAX_MENU_NAME_LENGTH];
    char                description[MAX_MENU_DESCRIPTION_LENGTH];
} ConsoleMenuId_t;

typedef struct ConsoleMenuItem
{
    ConsoleMenuId_t     id;
    struct ConsoleMenu  *sub_menu;
    FunctionResult_e    (*function_pointer)(int argc, char *argv[]);
} ConsoleMenuItem_t;

typedef struct ConsoleMenu
{
    ConsoleMenuId_t     id;
    ConsoleMenuItem_t   *menu_items;
    struct ConsoleMenu  *top_menu;
    unsigned int        menu_length;
    unsigned int        current_page;
    ConsoleMenuMode_e   mode;
    void                (*updater)(void);
} ConsoleMenu_t;

typedef struct ConsoleSelection
{
    char                key;
    const char          *description;
} ConsoleSelection_t;

typedef struct ConsoleSettings
{
    // Splash screen settings
    Splash_t            *splash_screen_pointer;
    unsigned int        num_splash_lines;
    // Pointer to the main menu
    ConsoleMenu_t       *main_menu_pointer;
    // Logging level
    LoggingLevel_e      logging_level;
} ConsoleSettings_t;

void Console_Init(ConsoleSettings_t *settings);
void Console_Main(void);

void Console_PromptForAnyKeyBlocking(void);
char Console_CheckForKeyBlocking(void);
char Console_CheckForKey(void);
unsigned int Console_PromptForInt(const char *prompt);

void Console_TraverseMenus(ConsoleMenu_t *menu);
char Console_PrintOptionsAndGetResponse(const ConsoleSelection_t selections[], unsigned int num_selections, unsigned int num_menu_selections);
void Console_Print(LoggingLevel_e logging_level, const char *format, ...);
void Console_PrintNoEol(LoggingLevel_e logging_level, const char *format, ...);
void Console_PrintNewLine(LoggingLevel_e logging_level);
void Console_PrintHeader(LoggingLevel_e logging_level, char *header_string);
void Console_PrintDivider(LoggingLevel_e logging_level);
void Console_PrintMenu(ConsoleMenu_t *menu);

// Fundamental functions wrapped around the logging level
char Console_GetCharInternal(LoggingLevel_e logging_level);
void Console_PutCharInternal(LoggingLevel_e logging_level, char c);
void Console_PutStringInternal(LoggingLevel_e logging_level, char * string);

// Platform-specific functions that must be implemented
extern char Console_GetChar(void);
extern void Console_PutChar(char c);
extern void Console_PutString(char * string);

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H
