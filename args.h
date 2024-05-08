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
#include "console.h"

/* Help string dimensions */
#define MAX_OPT_NAME_LENGTH (25)  ///< Maximum length of option name field
#define MAX_OPT_ARGS_LENGTH (5)   ///< Maximum length of argument type field
#define MAX_OPT_DESC_LENGTH (70)  ///< Maximum length of option description field
#define OPT_DBL_DASH_OFFSET (2)   ///< Offset for '--' prepending options
#define OPT_SGL_DASH_OFFSET (1)   ///< Offset for '-' prepending options
#define MAX_OPTION_GROUPS   (10)  ///< Maximum number of option groups in the options registry
#define MAX_CLI_ARGS        (128) ///< Maximum number of CLI arguments

#define MAX_PARSED_STRING_LEN        (1023)                      ///< Maximum number of characters we can parse from an OPTION_TYPE_STRING
#define MAX_PARSED_STRING_BUFFER_LEN (MAX_PARSED_STRING_LEN + 1) ///< Maximum buffer size for OPTION_TYPE_STRING

/* Source code cleanup helpers */
#define HELP_ENABLED    (true)
#define HELP_DISABLED   (false)
#define NOT_PARSED      (false)
#define NO_DEFINED_PTR  (NULL)
#define NO_FUNC_OPTIONS (NULL)
#define END_OF_OPTIONS         \
    {                          \
        0, 0, 0, 0, 0, 0, 0, 0 \
    }

/**
 * @brief Result of the args_getopt_index() function.
 *
 */
typedef enum GetOptResult
{
    GETOPT_OK = 0,      ///< No error, we managed to parse the option
    GETOPT_BAD_OPTION,  ///< Badly constructed option detected
    GETOPT_STRAY_ARG,   ///< Stray argument detected
    GETOPT_MISSING_ARG, ///< Missing argument for a recognized option that requires one
    GETOPT_UNKNOWN,     ///< Unknown option for this set of options
    GETOPT_HELP,        ///< Help option was requested
    GETOPT_END,         ///< End of options has been reached
} GetOptResult_e;

/**
 * @brief This informs the argument parser if there is no argument expected (e.g. a flag), a required argument (the next
 * token is parsed as a the required argument), or an optional argument (the next token may or may not be an additional argument).
 *
 */
typedef enum ArgType
{
    ARG_TYPE_NO_ARGUMENT       = 0, ///< Option does not take an argument
    ARG_TYPE_REQUIRED_ARGUMENT = 1, ///< Option requires an argument
} ArgType_e;

/**
 * @brief This informs the argument parser what kind of value to expect following the argument. This is used in
 * conjunction with ARG_TYPE_REQUIRED_ARGUMENT.
 *
 */
typedef enum OptionType
{
    OPTION_TYPE_NONE = 0,
    OPTION_TYPE_FLAG,      //< bool
    OPTION_TYPE_STRING,    //< char *
    OPTION_TYPE_ENUM,      //< typedef enum where the 0 is an invalid value
    OPTION_TYPE_FLOAT,     //< float
    OPTION_TYPE_INT,       //< integer
    OPTION_TYPE_UINT,      //< unsigned integer
    OPTION_TYPE_UINT32,    //< uint32_t
    OPTION_TYPE_UINT64,    //< uint64_t
    OPTION_TYPE_HEXUINT8,  //< uint8_t
    OPTION_TYPE_HEXUINT16, //< uint16_t
    OPTION_TYPE_HEXUINT32, //< uint32_t
    OPTION_TYPE_HEXUINT64, //< uint64_t
    OPTION_TYPE_FUNC_PTR,
} OptionType_e;

/* Forward declarations for typedefs below */
struct CliOptions;
struct CliOptionGroup;

/*
 * @brief   This struct defines a group of options, mostly to help visually when printing option help, but also to keep
 *          tabs on the main program options in a single array by adding pointers to these.
 *
 */
typedef struct CliOptionGroup
{
    const char        *name;          ///< The name of the CLI option group
    const char        *extended_help; ///< The extended help of the CLI option group
    struct CliOptions *options;       ///< A pointer to the group of options
} CliOptionGroup_t;

/**
 * @brief   This struct defines the options used by the command line parser as well as the menu interface (emphasis on
 *          he command line parser).
 *
 */
typedef struct CliOptions
{
    const char            *name;             ///< The name of the option
    const char            *description;      ///< The description of the option
    ArgType_e              arg_type;         ///< The argument type
    OptionType_e           option_type;      ///< The option type (the data type to expect)
    void                  *destination;      ///< The destination variable
    bool                   is_parsed;        ///< To keep track if it was parsed or not (used when performing iterations so user is only prompted once)
    bool                  *is_defined_ptr;   ///< To keep track if the option was defined in the invocation
    struct CliOptionGroup *function_options; ///< When option_type is OPTION_TYPE_FUNC_PTR, this stores a pointer to additional options
} CliOptions_t;

void args_set_flag_value(const char *name, CliOptions_t *options, int value);
void args_set_string_value(const char *name, CliOptions_t *options, const char *value);
void args_set_enum_value(const char *name, CliOptions_t *options, int value);
void args_set_int_value(const char *name, CliOptions_t *options, int value);
void args_set_u_int8_value(const char *name, CliOptions_t *options, uint8_t value);
void args_set_u_int16_value(const char *name, CliOptions_t *options, uint16_t value);
void args_set_u_int32_value(const char *name, CliOptions_t *options, uint32_t value);
void args_set_u_int64_value(const char *name, CliOptions_t *options, uint64_t value);

void       *args_get_option_destination_pointer(const char *name, CliOptions_t *options);
int         args_get_flag_value(const char *name, CliOptions_t *options);
const char *args_get_string_value(const char *name, CliOptions_t *options);
int         args_get_enum_value(const char *name, CliOptions_t *options);
int         args_get_int_value(const char *name, CliOptions_t *options);
uint8_t     args_get_u_int8_value(const char *name, CliOptions_t *options);
uint16_t    args_get_u_int16_value(const char *name, CliOptions_t *options);
uint32_t    args_get_u_int32_value(const char *name, CliOptions_t *options);
uint64_t    args_get_u_int64_value(const char *name, CliOptions_t *options);

void args_set_last_option_parsed(bool state);
void args_set_all_parsed(CliOptions_t *options, bool state);
void args_set_parsed(CliOptions_t *options, const char *name, bool state);
void args_set_defined(CliOptions_t *options, const char *name, bool state);
bool args_check_all_parsed(CliOptions_t *options);
bool args_check_parsed(CliOptions_t *options, const char *name);
bool args_check_pointers(CliOptions_t *options);
bool args_check_defined(CliOptions_t *options, const char *name);
void args_print_option(LoggingLevel_e logging_level, CliOptions_t *option);
void args_print_help(ConsoleFunctionPointer_t function);
void args_register_options(CliOptionGroup_t *options, ConsoleFunctionPointer_t function);

GetOptResult_e           args_getopt_index(int argc, char *argv[], CliOptions_t *options, int *option_index, char **option_arg);
ConsoleFunctionPointer_t args_parse(int argc, char *argv[], ConsoleFunctionPointer_t function, bool enable_help);
