// This helper functions are written to help make i/o easier
#ifndef HELPERS_C
#define HELPERS_C

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<limits.h>
#include<stdint.h>
#include<stdbool.h>
#include<math.h>
#include<float.h>

typedef char *string;

int get_int(string msg);
float get_float(string msg);
string get_string(string msg);
void atend();
static size_t allocations = 0;
static string *strings = NULL;
bool is_int(string);
bool is_float(string);
string get_string(string msg)
{
    // checking if more string can be allocated
    if (allocations == SIZE_MAX / sizeof(string))
    {
        return NULL;
    }

    printf("%s", msg);
    char c;
    size_t pos = 0;
    size_t size = 0;
    string buffer = NULL;
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
    {
        size++;
        // checking if the size of string is too large
        if (size + 1 == SIZE_MAX)
        {
            free(buffer);
            return NULL;
        }
        string temp = realloc(buffer, size);
        buffer = temp;
        buffer[pos++] = c;
    }
    string s = realloc(buffer, size + 1);
    s[pos] = '\0';

    string *temp = realloc(strings, sizeof(string) * (allocations + 1));
    strings = temp;
    strings[allocations] = s;
    allocations++;
    return s;
}

float get_float(string msg)
{
    while (1)
    {
        string digit = get_string(msg);
        if (strlen(digit) > 0 && !isspace(digit[0]))
        {
            char *ptr;
            float num = strtof(digit, &ptr);
            if(*ptr == '\0' && isfinite(num) != 0 && num < DBL_MAX)
            {
                if (strcspn(digit, "XxEePp") == strlen(digit))
                {
                    return num;
                }
            }

        }
    }
}

int get_int(string msg)
{
    while (1)
    {
        char *ptr;
        string digit = get_string(msg);
        if (strlen(digit) > 0 && !isspace(digit[0]))
        {
            long num = strtol(digit, &ptr, 10);
            if (*ptr == '\0' && num >= INT_MIN && num < INT_MAX)
            {
                return (int) num;
            }
        }
    }
}

bool is_int(string str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        if(!isdigit(str[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

bool is_float(string str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        if(!isdigit(str[i]) && str[i] != '.')
        {
            return 0;
        }
        i++;
    }
    return 1;
}

// functions to free all memory of strings
void atend()
{
    if (strings != NULL)
    {
        for (int i = 0; i < allocations; i++)
    {
        free(strings[i]);
    }
    free(strings);
    }
}

#endif
