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

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Console ANSI colors
#define ANSI_COLOR_BLACK   "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_TEXT_BOLD     "\x1b[1m"
#define ERASE_SCREEN       "\x1b[2J"

typedef enum FunctionResult
{
    FR_OK          = 0,   // No error, operation successful
    FR_FAIL        = -1,  // Generic failure
    FR_ASSERT_FAIL = -2,  // Assertion failure
    FR_INVALID     = -3,  // Invalid input
    FR_TIMEOUT     = -4,  // Operation timed out
    FR_NOMEM       = -5,  // Out of memory
    FR_NOACCESS    = -6,  // No access or permission denied
    FR_NOTFOUND    = -7,  // Item not found
    FR_BUSY        = -8,  // Resource is busy
    FR_DISCONNECT  = -9,  // Disconnected from a resource
    FR_UNSUPPORTED = -10, // Unsupported operation or feature
} FunctionResult_e;

typedef enum ConsoleMenuMode
{
    MENU_DEFAULT = 0, // Default menu behavior, immutable definition
    MENU_MUTABLE = 1, // Menu is dynamically populated, consider it mutable
} ConsoleMenuMode_e;

typedef enum ConsoleOptionFlags
{
    ORIENTATION_V = 1 << 0, // Default is options horizontal
    NO_DIVIDERS   = 1 << 1, // Default is dividers drawn
} ConsoleOptionFlags_e;

typedef enum LoggingLevel
{
    LOGGING_LEVEL_DISABLED = -1,
    LOGGING_LEVEL_0        = 0,
    LOGGING_LEVEL_1        = 1,
    LOGGING_LEVEL_2        = 2,
    LOGGING_LEVEL_3        = 3,
} LoggingLevel_e;

typedef enum TypeEnum
{
    TYPE_NONE = 0,
    TYPE_BOOL_NUM,
    TYPE_BOOL_WORD,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_FLOAT,
    TYPE_DEC_INT8,
    TYPE_DEC_INT16,
    TYPE_DEC_INT32,
    TYPE_DEC_INT64,
    TYPE_DEC_UINT8,
    TYPE_DEC_UINT16,
    TYPE_DEC_UINT32,
    TYPE_DEC_UINT64,
    TYPE_HEX_INT4, /* in8_t */
    TYPE_HEX_INT8,
    TYPE_HEX_INT12, /* int16_t */
    TYPE_HEX_INT16,
    TYPE_HEX_INT20, /* int32_t */
    TYPE_HEX_INT24, /* int32_t */
    TYPE_HEX_INT28, /* int32_t */
    TYPE_HEX_INT32,
    TYPE_HEX_INT36, /* int64_t */
    TYPE_HEX_INT40, /* int64_t */
    TYPE_HEX_INT44, /* int64_t */
    TYPE_HEX_INT48, /* int64_t */
    TYPE_HEX_INT52, /* int64_t */
    TYPE_HEX_INT56, /* int64_t */
    TYPE_HEX_INT60, /* int64_t */
    TYPE_HEX_INT64,
    TYPE_HEX_UINT4, /* uin8_t */
    TYPE_HEX_UINT8,
    TYPE_HEX_UINT12, /* uint16_t */
    TYPE_HEX_UINT16,
    TYPE_HEX_UINT20, /* uint32_t */
    TYPE_HEX_UINT24, /* uint32_t */
    TYPE_HEX_UINT28, /* uint32_t */
    TYPE_HEX_UINT32,
    TYPE_HEX_UINT36, /* uint64_t */
    TYPE_HEX_UINT40, /* uint64_t */
    TYPE_HEX_UINT44, /* uint64_t */
    TYPE_HEX_UINT48, /* uint64_t */
    TYPE_HEX_UINT52, /* uint64_t */
    TYPE_HEX_UINT56, /* uint64_t */
    TYPE_HEX_UINT60, /* uint64_t */
    TYPE_HEX_UINT64,
    TYPE_MAX,
} TypeEnum_e;

typedef struct TypeLookupTableEntry
{
    TypeEnum_e  type_enum;
    const char *format_string;
    size_t      size;
} TypeLookupTableEntry_t;

extern TypeLookupTableEntry_t console_type_lut[TYPE_MAX];

#define DBL_LINE_CHAR        "="
#define SGL_LINE_CHAR        "-"
#define BREADCRUMB_SEPARATOR ANSI_COLOR_RESET " > " ANSI_COLOR_YELLOW

#define NO_MAIN_MENU                (0)
#define NO_SUB_MENU                 (0)
#define NO_FUNCTION_POINTER         (0)
#define NO_ARGS                     (0)
#define NO_SIZE                     (0)
#define MAX_MENU_NAME_LENGTH        (CONSOLE_WIDTH - 40)
#define MAX_MENU_DESCRIPTION_LENGTH (CONSOLE_WIDTH - 40)
#define CONSOLE_WIDTH               (120)
#define CONSOLE_HEIGHT              (24)
#define TEXT_BLOCK_SIZE             (CONSOLE_WIDTH - 40)
#define STRING_BUFFER_SIZE          (1024)
#define NUM_STRING_BUFFERS          (50)
#define HEADER_TITLE_EXTRAS_WIDTH   (6) ///< "=[  ]=" = 6 characters
#define MAX_HEADER_TITLE_WIDTH      (CONSOLE_WIDTH - HEADER_TITLE_EXTRAS_WIDTH)
#define MAX_TABLE_COL_CHAR_WIDTH    ((50) + 1)
#define PAGE_LENGTH                 (10) ///< Maximum length of a page (0-9)
#define FIRST_PAGE                  (0)  ///< Pages are zero indexed

#define MENU_SIZE(x)      sizeof(x) / sizeof(ConsoleMenuItem_t)
#define SELECTION_SIZE(x) sizeof(x) / sizeof(ConsoleSelection_t)
#define TOTAL_PAGES(x)    ((x / (PAGE_LENGTH)) + (x % PAGE_LENGTH != 0))
#define IGNORE_UNUSED_FN_WRAPPER_ARGS() \
    (void)(argc);                       \
    (void)(argv); /* To avoid [-Werror=unused-parameter] when implementing FunctionResult_e Function(int argc, char *argv[]) function types */
#define IGNORE_UNUSED_ARG(x) (void)(x)

#define _STR(x) #x
#define STR(x)  _STR(x)

// User should define a splash screen as an as array of const pointer to const char.
typedef const char *const Splash_t[];

// Typedef the console.h function pointer
typedef FunctionResult_e (*ConsoleFunctionPointer_t)(int argc, char *argv[]);

typedef struct ConsoleMenuId
{
    char name[MAX_MENU_NAME_LENGTH];
    char description[MAX_MENU_DESCRIPTION_LENGTH];
} ConsoleMenuId_t;

typedef struct ConsoleMenuItem
{
    ConsoleMenuId_t          id;
    struct ConsoleMenu      *sub_menu;
    ConsoleFunctionPointer_t function_pointer;
} ConsoleMenuItem_t;

typedef struct ConsoleMenu
{
    ConsoleMenuId_t     id;
    ConsoleMenuItem_t  *menu_items;
    struct ConsoleMenu *parent_menu;
    unsigned int        menu_length;
    unsigned int        current_page;
    ConsoleMenuMode_e   mode;
    void (*updater)(void);
} ConsoleMenu_t;

typedef struct ConsoleSelection
{
    char        key;
    const char *description;
} ConsoleSelection_t;

typedef ConsoleSelection_t ConsoleSelections[];

typedef struct ConsoleSettings
{
    /* Splash screen settings */
    Splash_t    *splash_screen_pointer;
    /* Pointer to the main menu */
    ConsoleMenu_t *main_menu_pointer;
    /* Small Headers */
    bool small_headers;
    /* Logging level */
    LoggingLevel_e logging_level;
    /* Implemented functions */
    FunctionResult_e (*os_init_fn)(void);
    char (*get_char_fn)(void);
    void (*put_char_fn)(char);
    void (*put_string_fn)(const char *);
} ConsoleSettings_t;

#define TABLE_CELL_NO_OPTIONS (0)

typedef enum TableCellOptions
{
    TABLE_CELL_OPTIONS_NONE    = 0,
    TABLE_CELL_OPTIONS_BLANK   = 1 << 0,
    TABLE_CELL_OPTIONS_NO_DOTS = 1 << 1,
    /* Add more options here, up to 8 for each bit */
} TableCellOptions_e;

typedef enum TableCellHighlight
{
    TABLE_CELL_HIGHLIGHT_NONE    = 0,
    TABLE_CELL_HIGHLIGHT_RED     = 1,
    TABLE_CELL_HIGHLIGHT_GREEN   = 2,
    TABLE_CELL_HIGHLIGHT_YELLOW  = 3,
    TABLE_CELL_HIGHLIGHT_BLUE    = 4,
    TABLE_CELL_HIGHLIGHT_MAGENTA = 5,
    TABLE_CELL_HIGHLIGHT_CYAN    = 6,
    /* Add more options, up to 8 for 2^3 possibilities */
} TableCellHighlight_e;

typedef union
{
    struct
    {
        uint8_t options : 8;
        uint8_t highlight : 3;
    };
    uint32_t all;
} TableCellOptions_t;

typedef struct TableColumn
{
    const char         *header;  /* Column's header string */
    void               *values;  /* Pointer to array of values described by type TypeEnum_e */
    TypeEnum_e          type;    /* Data type of the column */
    TableCellOptions_t *options; /* Cell options */
} TableColumn_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Main functions */
void console_init(ConsoleSettings_t *settings);
void console_main(void);
void console_traverse_menus(ConsoleMenu_t *menu);

/* Settings functions */
void console_small_headers(bool enable);

/* Prompting options */
void         console_prompt_for_any_keys_blocking(void);
char         console_check_for_key_blocking(void);
char         console_check_for_key(void);
uint32_t     console_prompt_for_hex_uint32(const char *prompt, uint32_t default_val);
uint64_t     console_prompt_for_hex_uint64(const char *prompt, uint64_t default_val);
unsigned int console_prompt_for_int(const char *prompt, unsigned int default_val);
char        *console_prompt_for_string(const char *prompt, const char *default_val);

/* Core printing options */
char           console_print_options_and_get_response(const ConsoleSelection_t selections[], unsigned int num_selections, unsigned int num_menu_selections, unsigned int option_flags);
void           console_print(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_in_place(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_block(LoggingLevel_e logging_level, const char *block_string);
void           console_print_color(LoggingLevel_e logging_level, const char *color_string, const char *inner_string);
void           console_print_color_no_eol(LoggingLevel_e logging_level, const char *color_string, const char *inner_string);
void           console_print_debug(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_debug_no_eol(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_debug_new_line(LoggingLevel_e logging_level);
void           console_print_error(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_warn(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_success(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_no_eol(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_new_line(LoggingLevel_e logging_level);
void           console_print_header(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_sub_header(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_footer_banner(LoggingLevel_e logging_level, const char *format, ...);
void           console_print_header_internal(LoggingLevel_e logging_level, const char *ruler_string, const char *header_string);
void           console_print_divider(LoggingLevel_e logging_level);
void           console_print_menu(ConsoleMenu_t *menu);
unsigned int   console_get_string_buffer_index(void);
LoggingLevel_e console_get_logging_level(void);

/* Table printing functions */
void                console_print_table_divider(LoggingLevel_e logging_level, size_t *column_widths, int num_columns);
void                console_sprint_table_format_entry(char *buffer, size_t buff_size, TypeEnum_e type, void *data, TableCellOptions_t *options, int index);
TableCellOptions_t *console_get_table_cell_options_array(int num_rows, TableCellOptions_e default_options, TableCellHighlight_e default_highlight);
void                console_print_table(LoggingLevel_e logging_level, int num_rows, int num_columns, ...);

/* Fundamental functions wrapped around the logging level */
char console_get_char_internal(LoggingLevel_e logging_level);
void console_put_char_internal(LoggingLevel_e logging_level, char c);
void console_put_string_internal(LoggingLevel_e logging_level, const char *string);

/* Utility functions */
size_t           console_isprint_str_len(const char *str);
void             console_assert_warn(LoggingLevel_e logging_level, bool condition, const char *format, ...);
FunctionResult_e console_assert_error(LoggingLevel_e logging_level, bool condition, const char *format, ...);
void             console_assert_fatal(LoggingLevel_e logging_level, bool condition, const char *format, ...);

/* Platform-specific functions that must be implemented */
extern char console_get_char(void);
extern void console_put_char(char c);
extern void console_put_string(const char *string);

#ifdef __cplusplus
}
#endif
