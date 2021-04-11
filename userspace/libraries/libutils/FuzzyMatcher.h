#pragma once

#include <ctype.h>

#include <libutils/String.h>

class FuzzyMatcher
{
private:
public:
    FuzzyMatcher() {}
    ~FuzzyMatcher() {}

    bool match(const String &pattern, const String &string)
    {
        size_t pattern_index = 0;
        size_t string_index = 0;

        while (pattern_index < pattern.length() &&
               string_index < string.length())
        {
            if (tolower(pattern[pattern_index]) == tolower(string[string_index]))
            {
                pattern_index++;
            }

            string_index++;
        }

        return pattern_index == pattern.length();
    }
};