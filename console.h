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
} functionResult_e;

typedef enum
{
    MENU_DEFAULT = 0, // Default menu behavior, immutable definition
    MENU_MUTABLE = 1, // Menu is dynamically populated, consider it mutable
} consoleMenuMode_e;

#define DBL_LINE_CHAR               "═"
#define SGL_LINE_CHAR               "─"

#define NO_TOP_MENU                 (0) // NULL
#define NO_SUB_MENU                 (0) // NULL
#define NO_FUNCTION_POINTER         (0) // NULL
#define NO_ARGS                     (0) // NULL
#define NO_SIZE                     (0)
#define MAX_MENU_NAME_LENGTH        (20)
#define MAX_MENU_DESCRIPTION_LENGTH (48)
#define CONSOLE_WIDTH               (80)
#define STRING_BUFFER_SIZE          (100)
#define HEADER_TITLE_EXTRAS_WIDTH   (6) // "=[  ]=" = 6 characters
#define MAX_HEADER_TITLE_WIDTH      (CONSOLE_WIDTH - HEADER_TITLE_EXTRAS_WIDTH) 
#define PAGE_LENGTH                 (10) // Maximum length of a page (0-9)
#define FIRST_PAGE                  (0) // Pages are zero indexed

#define MENU_SIZE(x)                sizeof(x)/sizeof(consoleMenuItem_t)
#define SELECTION_SIZE(x)           sizeof(x)/sizeof(consoleSelection_t)
#define TOTAL_PAGES(x)              ((x/(PAGE_LENGTH)) + (x % PAGE_LENGTH != 0))

#define _STR(x) #x
#define STR(x) _STR(x)

// User should define a splash screen as an as array of const pointer to const char.
typedef const char *const splash_t[];

typedef struct consoleMenuId
{
    char                name[MAX_MENU_NAME_LENGTH];
    char                description[MAX_MENU_DESCRIPTION_LENGTH];
} consoleMenuId_t;

typedef struct consoleMenuItem
{
    consoleMenuId_t     id;
    struct consoleMenu  *subMenu;
    functionResult_e    (*functionPointer)(int argc, char *argv[]);
} consoleMenuItem_t;

typedef struct consoleMenu
{
    consoleMenuId_t     id;
    consoleMenuItem_t   *menuItems;
    struct consoleMenu  *topMenu;
    unsigned int        menuLength;
    unsigned int        currentPage;
    consoleMenuMode_e   mode;
    void                (*updater)(void);
} consoleMenu_t;

typedef struct consoleSelection
{
    char                key;
    const char          *description;
} consoleSelection_t;

typedef struct consoleSettings
{
    // Splash screen settings
    splash_t            *splashScreenPointer;
    unsigned int        numSplashLines;
    // Pointer to the main menu
    consoleMenu_t       *mainMenuPointer;
} consoleSettings_t;

void Console_Init(consoleSettings_t *settings);
void Console_Main(void);

void Console_PromptForAnyKeyBlocking(void);
char Console_CheckForKeyBlocking(void);
char Console_CheckForKey(void);
unsigned int Console_PromptForInt(const char *prompt);
void Console_TraverseMenus(consoleMenu_t *menu);
char Console_PrintOptionsAndGetResponse(const consoleSelection_t selections[], unsigned int numSelections, unsigned int numMenuSelections);
void Console_Print(const char *format, ...);
void Console_PrintNoEol(const char *format, ...);
void Console_PrintNewLine(void);
void Console_PrintHeader(char *headerString);
void Console_PrintDivider(void);
void Console_PrintMenu(consoleMenu_t *menu);

// Platform-specific functions that must be implemented
extern char Console_GetChar(void);
extern void Console_PutChar(char c);
extern void Console_PutString(char * string);

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_H
