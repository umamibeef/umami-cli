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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args.h"
#include "console.h"

int               current_arg_index = 1;
CliOptionGroup_t *options_registry[MAX_OPTION_GROUPS];
bool              arg_ledger[MAX_CLI_ARGS] = {false};
int               num_registered_options   = 0;
CliOptions_t     *last_options_parsed      = NULL;

/* String representations of the option types */
const char *opt_type_strings[] = {
    "NONE",      // OPTION_TYPE_NONE
    "FLAG",      // OPTION_TYPE_FLAG
    "STRING",    // OPTION_TYPE_STRING
    "ENUM",      // OPTION_TYPE_ENUM
    "FLOAT",     // OPTION_TYPE_FLOAT
    "INT",       // OPTION_TYPE_INT
    "UINT",      // OPTION_TYPE_UINT
    "UINT32",    // OPTION_TYPE_UINT32
    "UINT64",    // OPTION_TYPE_UINT64
    "HEXUINT8",  // OPTION_TYPE_HEXUINT8
    "HEXUINT16", // OPTION_TYPE_HEXUINT16
    "HEXUINT32", // OPTION_TYPE_HEXUINT32
    "HEXUINT64", // OPTION_TYPE_HEXUINT64
    "FUNC_PTR",  // OPTION_TYPE_FUNC_PTR
};

/**
 * @brief Set an OPTION_TYPE_FLAG option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the flag to
 */
void args_set_flag_value(const char *name, CliOptions_t *options, int value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((bool *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_STRING option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the string to
 */
void args_set_string_value(const char *name, CliOptions_t *options, const char *value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            strcpy((char *)(options[i].destination), value);
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_ENUM option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the enum to
 */
void args_set_enum_value(const char *name, CliOptions_t *options, int value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((int *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_INT option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the option to
 */
void args_set_int_value(const char *name, CliOptions_t *options, int value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((int *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_UINT8 option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the option to
 */
void args_set_u_int8_value(const char *name, CliOptions_t *options, uint8_t value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((uint8_t *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_UINT16 option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the option to
 */
void args_set_u_int16_value(const char *name, CliOptions_t *options, uint16_t value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((uint16_t *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_UINT32 option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the option to
 */
void args_set_u_int32_value(const char *name, CliOptions_t *options, uint32_t value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((uint32_t *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Set an OPTION_TYPE_UINT64 option to a value.
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @param value     The value to set the option to
 */
void args_set_u_int64_value(const char *name, CliOptions_t *options, uint64_t value)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            (*((uint64_t *)(options[i].destination))) = value;
            break;
        }
    }
}

/**
 * @brief Get an option's destination pointer
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return void*    The destination pointer of the option
 */
void *args_get_option_destination_pointer(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return options[i].destination;
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_FLAG option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return int      The value of the option
 */
int args_get_flag_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((bool *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_STRING option's value
 *
 * @param name          The name of the option (for lookup)
 * @param options       The list of options to look through
 * @return const char*  The value of the option
 */
const char *args_get_string_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (char *)(options[i].destination);
        }
    }

    return NULL;
}

/**
 * @brief Get an OPTION_TYPE_ENUM option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return int      The value of the option
 */
int args_get_enum_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((int *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_INT option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return int      The value of the option
 */
int args_get_int_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((int *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_UINT8 option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return uint8_t  The value of the option
 */
uint8_t args_get_u_int8_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((uint8_t *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_UINT16 option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return uint16_t The value of the option
 */
uint16_t args_get_u_int16_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((uint16_t *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_UINT32 option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return uint32_t The value of the option
 */
uint32_t args_get_u_int32_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((uint32_t *)(options[i].destination)));
        }
    }

    return 0;
}

/**
 * @brief Get an OPTION_TYPE_UINT64 option's value
 *
 * @param name      The name of the option (for lookup)
 * @param options   The list of options to look through
 * @return uint64_t The value of the option
 */
uint64_t args_get_u_int64_value(const char *name, CliOptions_t *options)
{
    for (int i = 0; options[i].name != 0; i++)
    {
        if (strcmp(options[i].name, name) == 0)
        {
            return (*((uint64_t *)(options[i].destination)));
            ;
        }
    }

    return 0;
}

/**
 * @brief Set the last options as all parsed or not all parsed
 *
 * @param state The parsed state to set the last options to
 */
void args_set_last_option_parsed(bool state)
{
    if (last_options_parsed != NULL)
    {
        args_set_all_parsed(last_options_parsed, state);
    }
}

/**
 * @brief Set all provided options as all parsed or not all parsed
 *
 * @param options The options whose parsed state is to be all set
 * @param state The parsed state to set the options to
 */
void args_set_all_parsed(CliOptions_t *options, bool state)
{
    unsigned int index = 0;
    while (options[index].name != 0)
    {
        options[index].is_parsed = state;
        index++;
    }
}

/**
 * @brief Set the parsed state of an option using its name as the key
 *
 * @param options The options to look through
 * @param name    The name of the option to set the parsed state of
 * @param state   The parsed state to set the option to
 */
void args_set_parsed(CliOptions_t *options, const char *name, bool state)
{
    unsigned int index = 0;
    while (options[index].name != 0)
    {
        if (strcmp(options[index].name, name) == 0)
        {
            options[index].is_parsed = state;
            break;
        }
        index++;
    }
}

/**
 * @brief Sets the is_defined_ptr flag for the option with the given name
 *
 * @param options The options that the option is in
 * @param name The name of the option
 * @param state The state to set the is_defined_ptr flag to
 */
void args_set_defined(CliOptions_t *options, const char *name, bool state)
{
    unsigned int index = 0;
    while (options[index].name != 0)
    {
        if (strcmp(options[index].name, name) == 0)
        {
            if (options[index].is_defined_ptr != NULL)
            {
                *options[index].is_defined_ptr = state;
                console_print_debug(LOGGING_LEVEL_1, "%s: Option \"%s\" set as defined through its pointer @ 0x%p.", __func__, name, options[index].is_defined_ptr);
            }
            else
            {
                console_print_warn(LOGGING_LEVEL_1, "%s: Option \"%s\" does not have a defined flag variable associated! Cannot set to defined state.", __func__, name);
            }
            break;
        }
        index++;
    }
}

/**
 * @brief Check if all options have been parsed
 *
 * @param options The options to check
 * @return true   All options have been parsed
 * @return false  Not all options have been parsed
 */
bool args_check_all_parsed(CliOptions_t *options)
{
    unsigned int index          = 0;
    bool         options_parsed = true;
    while (options[index].name != 0)
    {
        options_parsed &= options[index].is_parsed;
        index++;
    }

    return options_parsed;
}

/**
 * @brief Check if an option has been parsed
 *
 * @param options The options to check
 * @param name    The name of the option to check
 * @return true   The option has been parsed
 * @return false  The option has not been parsed or the option name was not found
 */
bool args_check_parsed(CliOptions_t *options, const char *name)
{
    unsigned int index         = 0;
    bool         option_parsed = false;
    while (options[index].name != 0)
    {
        if (strcmp(options[index].name, name) == 0)
        {
            option_parsed = options[index].is_parsed;
            break;
        }
        index++;
    }

    return option_parsed;
}

/**
 * @brief Check if all option pointers are valid (are not NULL)
 *
 * @param options The options to check
 * @return true   All option pointers are valid
 * @return false  One or more option pointers are invalid
 */
bool args_check_pointers(CliOptions_t *options)
{
    bool         fatal_error = false;
    unsigned int index       = 0;
    while (options[index].name != 0)
    {
        if (!options[index].destination)
        {
            console_print_error(LOGGING_LEVEL_0, "%s: Null pointer for option %d destination!", __FUNCTION__, index);
            return false;
        }
        index++;
    }

    return true;
}

/**
 * @brief Check if an option is set or not (i.e. if it was specified on the command line)
 *
 * @param options The options to check
 * @param name The name of the option to check
 * @return true The option is set
 * @return false The option is not set or the option name was not found
 */
bool args_check_defined(CliOptions_t *options, const char *name)
{
    unsigned int index = 0;
    while (options[index].name != 0)
    {
        if (strcmp(options[index].name, name) == 0)
        {
            if (options[index].is_defined_ptr != NULL)
            {
                return *options[index].is_defined_ptr;
            }
            else
            {
                return false;
            }
        }
        index++;
    }

    return false;
}

/**
 * @brief Print an option to the console
 *
 * @param logging_level The logging level to print the option at
 * @param option        The option to print
 */
void args_print_option(LoggingLevel_e logging_level, CliOptions_t *option)
{
    char        option_name_buff[MAX_OPT_NAME_LENGTH + 1];
    const char  tag_arg[]  = ANSI_COLOR_CYAN "[arg]" ANSI_COLOR_RESET;
    const char  tag_func[] = ANSI_COLOR_GREEN "[fnc]" ANSI_COLOR_RESET;
    const char  tag_none[] = ".....";
    const char *tag        = NULL;
    if (option->arg_type)
    {
        tag = tag_arg;
    }
    else if (option->option_type == OPTION_TYPE_FUNC_PTR)
    {
        tag = tag_func;
    }
    else
    {
        tag = tag_none;
    }
    snprintf(option_name_buff, sizeof(option_name_buff), "--%s", option->name);
    console_print(logging_level, "    %-0*s %-0*s %-0*s", MAX_OPT_NAME_LENGTH, option_name_buff, MAX_OPT_ARGS_LENGTH, tag, MAX_OPT_DESC_LENGTH, option->description);
}

/**
 * @brief Print the help for a function by looking up the function pointer in the options registry
 *
 * @param function The function pointer to print the help for
 */
void args_print_help(ConsoleFunctionPointer_t function)
{
    console_print_header(LOGGING_LEVEL_0, "Help");

    if (function)
    {
        CliOptionGroup_t *function_options       = NULL;
        CliOptions_t     *parent_function_option = NULL;
        /* Function pointer specified, print that function's options and any other option that is not a function pointer */
        for (int i = 0; i < num_registered_options; i++)
        {
            /* Look for the specified function pointer in the options registry */
            CliOptions_t *options = options_registry[i]->options;
            unsigned int  index   = 0;
            while (options[index].name != 0)
            {
                if (options[index].destination == (void *)function)
                {
                    parent_function_option = &options[index];
                    function_options       = options[index].function_options;
                    break;
                }
                index++;
            }
        }
        if (!function_options)
        {
            console_print(LOGGING_LEVEL_0, "The following function does not have specific arguments:");
        }
        else
        {
            console_print(LOGGING_LEVEL_0, "The following are options for the following function:");
        }
        console_print_new_line(LOGGING_LEVEL_0);
        args_print_option(LOGGING_LEVEL_0, parent_function_option);
        if (function_options)
        {
            /* Print that function's extended help if it exists */
            if (function_options->extended_help)
            {
                console_print_sub_header(LOGGING_LEVEL_0, "Extended Help");
                console_print_block(LOGGING_LEVEL_0, function_options->extended_help);
            }

            /* Print that function's options */
            console_print_sub_header(LOGGING_LEVEL_0, "%s", function_options->name);
            CliOptions_t *options = function_options->options;
            unsigned int  index   = 0;
            while (options[index].name != 0)
            {
                args_print_option(LOGGING_LEVEL_0, &options[index]);
                index++;
            }
        }
    }
    else
    {
        console_print_block(LOGGING_LEVEL_0,
                            "The following are the options for this program. If the option represents a function pointer that directly executes an internal function, it will be proceeded by a " ANSI_COLOR_GREEN "[fnc]" ANSI_COLOR_RESET " tag. If the option expects an argument, it will be proceeded by an " ANSI_COLOR_CYAN "[arg]" ANSI_COLOR_RESET " tag. For further help on a function, --help can be appended after a function for specific help on that function.");
    }

    for (int i = 0; i < num_registered_options; i++)
    {
        if (function)
        {
            /* If we're printing a function pointer's option, ignore all other function pointer options. Do this by
            going through the current registered option group and skip it entirely if all options are function pointers
            */
            bool          dont_print_group = true;
            CliOptions_t *options          = options_registry[i]->options;
            unsigned int  index            = 0;
            while (options[index].name != 0)
            {
                /* Go through all options and see if any aren't function pointers */
                if (options[index].option_type != OPTION_TYPE_FUNC_PTR)
                {
                    dont_print_group = false;
                    break;
                }
                index++;
            }
            if (dont_print_group)
            {
                /* Skip printing this group entirely */
                continue;
            }
        }
        console_print_sub_header(LOGGING_LEVEL_0, "%s", options_registry[i]->name);
        CliOptions_t *options = options_registry[i]->options;
        unsigned int  index   = 0;
        while (options[index].name != 0)
        {
            args_print_option(LOGGING_LEVEL_0, &options[index]);
            index++;
        }
    }
    console_print_new_line(LOGGING_LEVEL_0);
}

/**
 * @brief   This function registers an options group with the central registry. If a function pointer is passed it, the
 *          group is instead associated with the option that matches the same function pointer in the options registry.
 *
 * @param   option_group        A pointer to the options group to register
 * @param   function            An optional function pointer to register the options group with the same function
 *                              pointer option found in the main registry.
 */
void args_register_options(CliOptionGroup_t *option_group, ConsoleFunctionPointer_t function)
{
    bool          fatal_error                = false;
    CliOptions_t *parent_function_option     = NULL;
    CliOptions_t *options                    = NULL;
    bool          function_option_registered = false;
    int           index                      = 0;

    console_print_debug(LOGGING_LEVEL_1, "%s: Attempting to register \"%s\" to options registry", __FUNCTION__, option_group->name);
    if (num_registered_options >= MAX_OPTION_GROUPS)
    {
        console_print_error(LOGGING_LEVEL_0, "%s: Fatal error: can't register any more option groups!", __FUNCTION__);
        fatal_error = true;
    }
    else
    {
        /* Perform a sanity check on the options */
        options = option_group->options;
        index   = 0;
        while (options[index].name)
        {
            if (options[index].arg_type == ARG_TYPE_NO_ARGUMENT)
            {
                /* Sanity check that the option type is a flag or function pointer */
                if (options[index].option_type != OPTION_TYPE_FLAG && options[index].option_type != OPTION_TYPE_FUNC_PTR)
                {
                    console_print_error(LOGGING_LEVEL_0, "%s: Fatal error! Option \"%s\" is not a OPTION_TYPE_FLAG or OPTION_TYPE_FUNC_PTR but was specified with ARG_TYPE_NO_ARGUMENT!", __FUNCTION__, options[index].name);
                    exit(FR_ASSERT_FAIL);
                }
            }
            else if (options[index].arg_type == ARG_TYPE_NO_ARGUMENT)
            {
                /* Sanity check that the option type is not a flag or function pointer */
                if (options[index].option_type == OPTION_TYPE_FLAG || options[index].option_type == OPTION_TYPE_FUNC_PTR)
                {
                    console_print_error(LOGGING_LEVEL_0, "%s: Fatal error! Option \"%s\" is a OPTION_TYPE_FLAG or OPTION_TYPE_FUNC_PTR but was specified with ARG_TYPE_REQUIRED_ARGUMENT!", __FUNCTION__, options[index].name);
                    exit(FR_ASSERT_FAIL);
                }
            }
            index++;
        }

        /* If a function pointer was passed in, the user intends to link the options to an existing option that is of
        the function pointer type. This allows one to link both option groups to display from a single help print option.*/
        if (function)
        {
            console_print_debug(LOGGING_LEVEL_1, "%s: Looking for option with destination 0x%x", __FUNCTION__, function);

            /* Iterate through existing registered option groups */
            for (int i = 0; i < num_registered_options; i++)
            {
                options = options_registry[i]->options;
                index   = 0;
                while (options[index].name)
                {
                    /* Iterate through function group options */
                    if (options[index].option_type == OPTION_TYPE_FUNC_PTR &&
                        options[index].destination == (void *)function)
                    {
                        /* Found a match, update the function_options member */
                        options[index].function_options = option_group;
                        function_option_registered      = true;
                        parent_function_option          = &options[index];
                        break;
                    }
                    index++;
                }
            }
            if (!function_option_registered)
            {
                console_print_error(LOGGING_LEVEL_0, "%s: Fatal error: couldn't find a matching function pointer for option registration!", __FUNCTION__);
                fatal_error = true;
            }
            else
            {
                console_print_debug(LOGGING_LEVEL_1, "%s: Successfully registered options \"%s\" to function \"%s\"", __FUNCTION__, option_group->name, parent_function_option->name);
            }
        }
        /* Options not tied to a function, add it to the main registry as another entry */
        else
        {
            /* Don't register more than once */
            bool options_registered = false;
            for (int i = 0; i < num_registered_options; i++)
            {
                if (options_registry[i] == option_group)
                {
                    options_registered = true;
                }
            }
            if (!options_registered)
            {
                options_registry[num_registered_options++] = option_group;
                console_print_debug(LOGGING_LEVEL_1, "%s: Successfully registered options \"%s\" to options registry! Registry now has %d options registered.", __FUNCTION__, option_group->name, num_registered_options);
            }
            else
            {
                console_print_debug(LOGGING_LEVEL_1, "%s: Options \"%s\" was already in the registry. Nothing happened.", __FUNCTION__, option_group->name, num_registered_options);
            }
        }
    }

    /* Exit after fatal errors */
    if (fatal_error)
    {
        exit(1);
    }
}

/**
 * @brief                   This function performs the equivalent of the standard getopt_long_only but for enhanced
 *                          options defined by CliOptions_t. This function will update the argument ledger to keep track
 *                          of which arguments have been parsed.
 *
 * @param argc              The count of arguments
 * @param argv              The arguments array
 * @param options           The options to parse
 * @param option_index      The index of the option that was parsed
 * @param option_arg        The argument of the option that was parsed
 *
 * @return GetOptResult_e   The result of the parsing. GETOPT_OK if the option was parsed successfully,
 *                          GETOPT_MISSING_ARG if an argument was missing, GETOPT_STRAY_ARG if a stray argument was
 *                          found, GETOPT_END if the end of the options was reached, GETOPT_HELP if the help option was
 *                          requested.
 */
GetOptResult_e args_getopt_index(int argc, char *argv[], CliOptions_t *options, int *option_index, char **option_arg)
{
    /* Go through arguments from where we left off */
    int dash_count = 0;

    for (int arg_index = current_arg_index; arg_index < argc; arg_index++)
    {
        /* Skip if we've already successfully parsed this argument */
        if (arg_ledger[arg_index])
        {
            continue;
        }

        console_print_debug(LOGGING_LEVEL_1, "%s: Parsing argument \"%s\"...", __FUNCTION__, argv[arg_index]);

        /* Check if the argument is help */
        if ((strcmp(argv[arg_index], "--help") == 0) || (strcmp(argv[arg_index], "-help") == 0))
        {
            /* Help was requested */
            console_print_debug(LOGGING_LEVEL_1, "%s: Help requested!", __FUNCTION__);
            arg_ledger[arg_index] = true; /* Mark the help argument as parsed */
            return GETOPT_HELP;
        }

        /* Get dash count */
        if (argv[arg_index][0] == '-')
        {
            dash_count = 1;
        }
        if (argv[arg_index][0] == '-' && argv[arg_index][1] == '-')
        {
            dash_count = 2;
        }

        /* Check if we've run into a stray argument (we should always be pointing at an option) */
        if (dash_count == 0)
        {
            console_print_error(LOGGING_LEVEL_0, "%s: Fatal error! Stray argument \"%s\" found!", __FUNCTION__, argv[arg_index]);
            return GETOPT_STRAY_ARG;
        }

        /* Iterate through the options that were passed in */
        for (int options_index = 0; options[options_index].name != 0; options_index++)
        {
            /* Skip if we've already parsed this option */
            if (options[options_index].is_parsed)
            {
                continue;
            }

            /* Check if argument matches */
            if (strcmp(options[options_index].name, argv[arg_index] + dash_count) == 0)
            {
                /* Check if the option is a flag or a function pointer */
                if (options[options_index].arg_type == ARG_TYPE_NO_ARGUMENT)
                {
                    /* We found the option, set the index and return */
                    *option_index                    = options_index; /* Set the index */
                    *option_arg                      = NULL;          /* No argument for this option */
                    options[options_index].is_parsed = true;          /* Mark the option as parsed */
                    arg_ledger[arg_index]            = true;          /* Mark the argument as parsed */
                    current_arg_index                = arg_index + 1; /* Move the current argument index so that we start parsing on the next one */
                    console_print_debug(LOGGING_LEVEL_1, "%s: Found option \"%s\".", __FUNCTION__, options[options_index].name);
                    return GETOPT_OK;
                }
                /* Check if the option is a required argument */
                else if (options[options_index].arg_type == ARG_TYPE_REQUIRED_ARGUMENT)
                {
                    /* Make sure we have an argument */
                    if (arg_index + 1 < argc)
                    {
                        /* Make sure the next argument is not an option */
                        if (argv[arg_index + 1][0] != '-')
                        {
                            /* We found the option's argument, set the index and argument pointer and return */
                            *option_index                    = options_index;       /* Set the index */
                            *option_arg                      = argv[arg_index + 1]; /* Set the argument */
                            options[options_index].is_parsed = true;                /* Mark the option as parsed */
                            arg_ledger[arg_index]            = true;                /* Mark the argument as parsed (recognized option) */
                            arg_ledger[arg_index + 1]        = true;                /* Mark the argument as parsed (recognized argument) */
                            current_arg_index                = arg_index + 2;       /* Move the current argument index so that we start parsing on the next one */
                            console_print_debug(LOGGING_LEVEL_1, "%s: Found option \"%s\" with required argument \"%s\"", __FUNCTION__, options[options_index].name, argv[arg_index + 1]);
                            return GETOPT_OK;
                        }
                    }

                    /* If we got here, we're missing an argument */
                    console_print_error(LOGGING_LEVEL_0, "%s: Error! Option \"%s\" requires an argument!", __FUNCTION__, options[options_index].name);
                    return GETOPT_MISSING_ARG;
                }
            }
            else
            {
                /* Option didn't match, go to the next one */
                continue;
            }
        }
    }

    /* If we're escaped the for loop, we've reached the end of the options */
    current_arg_index = argc;
    return GETOPT_END;
}

/**
 * @brief       Parse the command line arguments for function arguments. Options can be specified through the command
 *              line with either a double dash or single dash. An argument is specified by appending the argument
 *              proceeding an option, but without dashes. For example, to specify an argument for the option "--option",
 *              the command line argument would be "--option argument".
 *
 * @param[in]   argc                The count of arguments
 * @param       argv                The arguments array
 * @param       function            If set, this function will parse options pointed to by options with
 *                                  arg_type==OPTION_TYPE_FUNC_PTR
 * @param       enable_help         If set, this function will display help if the --help option is passed
 */
ConsoleFunctionPointer_t args_parse(int argc, char *argv[], ConsoleFunctionPointer_t function, bool enable_help)
{
    int                      option_index;
    char                    *option_argument;
    int                      option_offset;
    GetOptResult_e           get_opt_result;
    ConsoleFunctionPointer_t function_pointer_argument = NULL;
    CliOptionGroup_t        *function_options_group    = NULL;
    CliOptionGroup_t        *current_options_group     = NULL;
    CliOptions_t            *current_options           = NULL;
    bool                     fatal_error               = false;
    bool                     help_wanted               = false;

    /* Check if we have arguments to parse */
    if (!(argc > 1))
    {
        console_print_warn(LOGGING_LEVEL_1, "%s: No arguments to parse!", __FUNCTION__);
        return NULL;
    }

    /* Check if we support the number of arguments */
    if (argc > MAX_CLI_ARGS)
    {
        console_print_error(LOGGING_LEVEL_0, "%s: Fatal error! Too many arguments to parse! (%d > %d)", __FUNCTION__, argc, MAX_CLI_ARGS);
        return NULL;
    }

    console_print_debug(LOGGING_LEVEL_1, "%s: Command line arguments detected, will try to parse them", __FUNCTION__);

    /* If we were provided a function pointer, we must also parse those arguments. We parse it at the end of our list,
    after the option registry option groups. */
    int option_groups_to_parse = (function ? num_registered_options + 1 : num_registered_options);
    for (int i = 0; i < option_groups_to_parse; i++)
    {
        /* If we're also parsing a function's arguments, parse the function arguments if we're at the last item */
        if (function && (i == num_registered_options))
        {
            if (!function_options_group)
            {
                console_print_warn(LOGGING_LEVEL_1, "%s: Function does not have options to parse.", __FUNCTION__);
                break;
            }
            else
            {
                current_options_group = function_options_group;
                current_options       = function_options_group->options;
            }
        }
        else
        {
            current_options_group = options_registry[i];
            current_options       = options_registry[i]->options;
        }

        if (current_options_group)
        {
            console_print_debug(LOGGING_LEVEL_1, "%s: Parsing options group: \"%s\" [%d/%d]", __FUNCTION__, current_options_group->name, i + 1, option_groups_to_parse);
        }

        /* If we're specifying a function pointer and we haven't found it yet, then we need to look for that pointer in our registry */
        if (function && !function_options_group)
        {
            unsigned int index = 0;
            while (current_options[index].name != 0)
            {
                if (current_options[index].destination == (void *)function)
                {
                    if (current_options[index].function_options)
                    {
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found options \"%s\" for function \"%s\"", __FUNCTION__, current_options[index].function_options->name, current_options[index].name);
                        function_options_group = current_options[index].function_options;
                    }
                    else
                    {
                        console_print_warn(LOGGING_LEVEL_1, "%s: Found function \"%s\" but it does not have options associated with it. ", __FUNCTION__, current_options[index].name);
                    }
                }
                index++;
            }
        }

        /* Check if we've already parsed these options. We won't waste cycles parsing again if we did.*/
        if (args_check_all_parsed(current_options))
        {
            console_print_debug(LOGGING_LEVEL_1, "%s: Already parsed, moving on...", __FUNCTION__);
            continue;
        }

        /* Make sure we don't have any NULL pointers in our option destinations */
        if (!args_check_pointers(current_options))
        {
            fatal_error = true;
            break;
        }

        /* Reset the current argument index */
        current_arg_index = 1;

        while (1)
        {
            option_index    = 0;
            option_argument = NULL;
            get_opt_result  = args_getopt_index(argc, argv, current_options, &option_index, &option_argument);

            /* Check if we're done parsing */
            if (get_opt_result == GETOPT_END)
            {
                console_print_debug(LOGGING_LEVEL_1, "%s: Reached end of current_options!", __FUNCTION__);
                break;
            }
            else if (get_opt_result == GETOPT_HELP)
            {
                help_wanted = true;
                console_print_debug(LOGGING_LEVEL_1, "%s: Help wanted! Help's on the way.", __FUNCTION__);
                break;
            }
            /* If we got a GETOPT_OK, then we have a valid option_index (and option_argument if applicable) */
            else if (get_opt_result == GETOPT_OK)
            {
                OptionType_e option_type = current_options[option_index].option_type;
                switch (option_type)
                {
                    case OPTION_TYPE_FLAG:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a flag argument %s", __FUNCTION__, current_options[option_index].name);
                        (*((bool *)(current_options[option_index].destination))) = true;
                        break;
                    case OPTION_TYPE_STRING:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a string argument %s", __FUNCTION__, option_argument);
                        strncpy((char *)(current_options[option_index].destination), option_argument, MAX_PARSED_STRING_LEN);
                        break;
                    case OPTION_TYPE_ENUM:
                        // Copy integer, and add one. Enums always start with a null value,
                        // so we offset by one to zero index the first item.
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found an enum argument %s", __FUNCTION__, option_argument);
                        (*((int *)(current_options[option_index].destination))) = atoi(option_argument) + 1;
                        break;
                    case OPTION_TYPE_FLOAT:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a float argument %s", __FUNCTION__, option_argument);
                        (*((int *)(current_options[option_index].destination))) = atof(option_argument);
                        break;
                    case OPTION_TYPE_INT:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a decimal int argument %s", __FUNCTION__, option_argument);
                        (*((int *)(current_options[option_index].destination))) = atoi(option_argument);
                        break;
                    case OPTION_TYPE_UINT:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a decimal unsigned int argument %s", __FUNCTION__, option_argument);
                        (*((unsigned int *)(current_options[option_index].destination))) = atoi(option_argument);
                        break;
                    case OPTION_TYPE_UINT32:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a decimal uint32 argument %s", __FUNCTION__, option_argument);
                        (*((uint32_t *)(current_options[option_index].destination))) = (uint32_t)atoi(option_argument);
                        break;
                    case OPTION_TYPE_UINT64:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a decimal uint64 argument %s", __FUNCTION__, option_argument);
                        (*((uint64_t *)(current_options[option_index].destination))) = (uint64_t)atoll(option_argument);
                        break;
                    case OPTION_TYPE_HEXUINT8:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a hexadecimal uint8 argument %s", __FUNCTION__, option_argument);
                        (*((uint8_t *)(current_options[option_index].destination))) = strtoul(option_argument, NULL, 16);
                        break;
                    case OPTION_TYPE_HEXUINT16:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a hexadecimal uint16 argument %s", __FUNCTION__, option_argument);
                        (*((uint16_t *)(current_options[option_index].destination))) = strtoul(option_argument, NULL, 16);
                        break;
                    case OPTION_TYPE_HEXUINT32:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a hexadecimal uint32 argument %s", __FUNCTION__, option_argument);
                        (*((uint32_t *)(current_options[option_index].destination))) = strtoul(option_argument, NULL, 16);
                        break;
                    case OPTION_TYPE_HEXUINT64:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a hexadecimal uint64 argument %s", __FUNCTION__, option_argument);
                        (*((uint64_t *)(current_options[option_index].destination))) = strtoul(option_argument, NULL, 16);
                        break;
                    case OPTION_TYPE_FUNC_PTR:
                        console_print_debug(LOGGING_LEVEL_1, "%s: Found a function pointer", __FUNCTION__);
                        function_pointer_argument = current_options[option_index].destination;
                        break;
                    case OPTION_TYPE_NONE:
                    default:
                        console_print_error(LOGGING_LEVEL_0, "%s: Unexpected argument type %d. Aborting.", __FUNCTION__, current_options[option_index].option_type);
                        fatal_error = true;
                        break;
                }
            }
            else /* We didn't get a good result */
            {
                fatal_error = true;
                break;
            }

            if (!fatal_error)
            {
                /* Set the option as defined */
                if (current_options[option_index].is_defined_ptr)
                {

                    *current_options[option_index].is_defined_ptr = true;
                    console_print_debug(LOGGING_LEVEL_1, "%s: Option \"%s\" set as defined through its pointer @ 0x%p.", __FUNCTION__, current_options[option_index].name, current_options[option_index].is_defined_ptr);
                }
                else
                {
                    console_print_debug(LOGGING_LEVEL_1, "%s: Option \"%s\" does not have a defined flag variable associated! Cannot set to defined state.", __FUNCTION__, current_options[option_index].name);
                }
            }

            /* If a function pointer was detected as an argument or help was requested, let's get out of here */
            if (function_pointer_argument || help_wanted)
            {
                break;
            }
        }

        /* Exit after fatal errors */
        if (fatal_error)
        {
            exit(1);
        }

        /*** Iteration enablement ***/
        /* Keep track of these options as the latest ones to have been parsed */
        last_options_parsed = current_options;
        /* Set all options as parsed so that we don't repeat. For command line
         * options, we assume that all options are parsed as optional parameters
         * with default values are assumed when not being passed in. */
        args_set_all_parsed(current_options, true);

        /* Continuation of function pointer argument and help detection */
        if (function_pointer_argument || help_wanted)
        {
            break;
        }
    }

    /* If we have help enabled and we don't have a function pointer arg, we are at the terminal option parsing. Make
    sure that all options were recognized. */
    if (enable_help && !function_pointer_argument)
    {
        for (int argv_index = 1; argv_index < argc; argv_index++)
        {
            if (!arg_ledger[argv_index])
            {
                console_print_error(LOGGING_LEVEL_0, "%s: Fatal error: \"%s\" is not a recognized option!", __FUNCTION__, argv[argv_index]);
                help_wanted = true;
            }
        }
    }

    /* If help was requested, print it if we've allowed it. */
    if (help_wanted && enable_help)
    {
        /* If a function pointer was detected as an argument, we'll postpone the help for later */
        if (!function_pointer_argument)
        {
            /* If a function pointer was passed in, the option help printer will tailor the help specific to that function.
            If not, then the main program help will be displayed. */
            args_print_help(function);
            /* Exit nicely (FR_OK) */
            exit(FR_OK);
        }
    }

    /* Return the function pointer if we got one or NULL if not */
    return function_pointer_argument;
}
