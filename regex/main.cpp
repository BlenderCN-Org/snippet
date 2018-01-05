/*
https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

c    matches any literal character c
.    matches any single character
^    matches the beginning of the input string
$    matches the end of the input string
*    matches zero or more occurrences of the previous character
*/

#include <cassert>

//
bool match(char *regexp, char *text)
{
    class Local
    {
    public:
        //
        static bool matchhere(char* regexp, char* text)
        {
            if (regexp[0] == '\0')
            {
                return true;
            }
            else if (regexp[1] == '*')
            {
                return matchstar(regexp[0], regexp + 2, text);
            }
            else if (regexp[0] == '$' && regexp[1] == '\0')
            {
                return (*text == '\0');
            }
            else if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
            {
                return matchhere(regexp + 1, text + 1);
            }
            else
            {
                return false;
            }
        }
        //
        static bool matchstar(int c, char *regexp, char *text)
        {
            /* a * matches zero or more instances */
            do
            {
                if (matchhere(regexp, text))
                {
                    return true;
                }
            } while (*text != '\0' && (*text++ == c || c == '.'));
            return false;
        }
    };
    //
    if (regexp[0] == '^')
    {
        return Local::matchhere(regexp + 1, text);
    }
    do
    {
        //
        if (Local::matchhere(regexp, text))
        {
            return true;
        }
    } while (*text++ != '\0');
    return false;
}

void main()
{
    assert(match("A", "ABC") == true);
    assert(match("B", "ABC") == true);
    assert(match("C", "ABC") == true);
    assert(match("^A", "ABC") == true);
    assert(match("^B", "ABC") == false);
    assert(match("C$", "ABC") == true);
    assert(match("A$", "ABC") == false);
    assert(match("A*C", "ABC") == true);
    assert(match("A*BC", "ABC") == true);
    return;
}
