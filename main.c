#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <strings.h>

#define SignedSTRLEN long long
#define STRLEN unsigned SignedSTRLEN
#define STRLENSpecifier "%lld"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define Error(format, ...) printf("Error : "format".\n", __VA_ARGS__)
#define IndexOutOfRangeError(index) Error("index '%lld' out of bound", index)
#define MemoryAllocationError Error("couldn't allocate memory, in '%s'", __func__)
#define InvalidArgument(message) Error("%s", message);
#define THROW(error)  error; exit(1)

typedef struct {
    STRLEN len;
    char *chars;
} string;

/* returns the length of str
 *
 * function: len 
 * param: str, the desired string
 * return STRLEN (unsigned long long), the length of the string
 * */
STRLEN len(string str){
    return str.len;
}

/* returns character at index
 *
 * function: get
 * param: str, the desired string
 * param: index, the index of the character; if <0 indexing starts from the end (-1 corresponds to last character)
 * exit if: index >= length or index < - length
 * return: char, the character
 * */
char get(const string str, STRLEN index){
    if (index >= str.len || index < - str.len){
        THROW(IndexOutOfRangeError(index));
    }
    return str.chars[index >= 0 ? index : str.len + index];
}

/* sets character at index to c
 *
 * function: set
 * param: str, string
 * param: index, index of character; if < 0 start indexing from end, if == str.len append c to str
 * param: c, replacement
 * exit if: index > str.len || index < - str.len 
 * return: void
 * */
void set(string str, STRLEN index, char c){
    if (index > str.len || index < - str.len){
        THROW(IndexOutOfRangeError(index));
    }
    if (index == str.len || index == -1){
        char *tmp = realloc(str.chars, len(str) + 1 + 1);
        if (tmp == NULL){
            THROW(MemoryAllocationError);
        } else {
            str.chars = tmp;
        }
        str.chars[str.len] = c; str.chars[str.len + 1] = '\0'; str.len++;
        return;
    }
    str.chars[index] = c;
}

/* returns a string, where the part of str1 at index with str2
 *
 * function: replaceFrom
 * param: main, original string
 * param: replacement, the replacement
 * - replacement doesn't fit in left over space : extend size to fit
 * param: index, the index from which to start
 * - index > main length : exit
 * - index < 0 : start indexing from end
 * return: string, the resulting string
 * */
string replaceFrom(const string str1, const char *str2, STRLEN index){
    if (index >= str1.len){
        THROW(IndexOutOfRangeError(index));
    }
    if (index < 0){
        index = str1.len + index;
    }
    int len2 = strlen(str2);
    STRLEN tmp = str1.len - (index + len2);
    string result = {.len = index + len2 + (tmp > 0 ? tmp : 0)};
    result.chars = malloc(result.len + 1);
    STRLEN i;
    for (i = 0; i < result.len; i++){
        if (i - index >= 0 && i - index < len2){
            result.chars[i] = str2[i - index]; continue;
        }
        if (i < str1.len){
            result.chars[i] = str1.chars[i];
        }
    }
    return result;
}

string replaceStrFrom(const string str1, const string str2, STRLEN index){
    return replaceFrom(str1, str2.chars, index);
}

void strfree(int num, ...){
    va_list args;
    va_start(args, num);
    string str;
    for (int i = 0; i < num; i++){
        str = va_arg(args, string);
        free(str.chars); str.chars = NULL;
    }
    va_end(args);
}

string new(const char *chars){
    if (chars == NULL){
        string result = (string){.len = 0, .chars = malloc(1)};
        result.chars[0] = '\0';
        return result;
    }
    int len = strlen(chars);
    string str = {.len = len, .chars = malloc(len + 1)};
    strcpy(str.chars, chars);
    return str;
}

string strjoin(int num, ...){
    va_list args;
    va_start(args, num);
    string str, result = {.len = 0, .chars = malloc(1)}; result.chars[0] = '\0';
    char *tmp = NULL;
    for (int i = 0; i < num; i++){
        str = (string)va_arg(args, const string);
        if (str.chars == NULL){
            continue;
        }
        result.len += str.len;
        tmp = realloc(result.chars, result.len + 1);
        if (tmp == NULL){
            THROW(MemoryAllocationError);
        }
        result.chars = tmp; strcat(result.chars, str.chars);
    }
    return result;
}

void strprintMany(const char *delimiter, int num, ...){
    if (delimiter == NULL){
        delimiter = "";
    }
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num - 1; i++){
        printf("%s%s", ((const string)va_arg(args, string)).chars, delimiter);
    }
    printf("%s\n", ((string)va_arg(args, string)).chars);
    va_end(args);
}

void strprint(const string str){
    printf("%s\n", str.chars);
}

char *toarray(const string str){
    return str.chars;
}

int strcompare(const string str1, const char *str2){
    return strcmp(str1.chars, str2);
}

bool strequals(const string str1, const char *str2){
    return strcompare(str1, str2) == 0;
}

bool equalsAt(const char *main, const char *target, size_t index){
    size_t len = strlen(target);
    if (strlen(main) < index + len - 1){
        return false;
    }
    return strncmp(main + index, target, len) == 0;
}

bool strequalsAt(const string main, const char *target, STRLEN index){
    return equalsAt(main.chars, target, index);
}

size_t find(const char *main, const char *target){
    for (size_t i = 0; i < strlen(main); i++){
        if (equalsAt(main, target, i)){
            return i;
        }
    }
    return -1;
}

SignedSTRLEN strfind(const string main, const char *target){
    STRLEN i = 0;
    while (i < main.len){
        if (strequalsAt(main, target, i)){
            return i;
        }
    }

    return -1;
}

char *substr(const char *str, long start, long end){
    char *result;
    size_t strLen = strlen(str), resultLen;
    if (end >= 0 && end < start){
        THROW(InvalidArgument("invalid range for subscripting."));
    }
    if (end < 0){
        resultLen = (strLen + end) - start;
    } else {
        resultLen = min(end - start, strLen - start);
    }

    result = malloc(resultLen + 1);
    for (int i = 0; i < resultLen; i++){
        result[i] = str[i + start];
    }
    result[resultLen] = '\0';
 
    return result;
}

string strsub(const string str, SignedSTRLEN start, SignedSTRLEN end){
    string result;
    if (end >= 0 && end < start){
        THROW(InvalidArgument("invalid range for subscripting."));
    }

    if (end < 0){
        result.len = (str.len + end) - start;
    } else {
        result.len = min(end - start, str.len - start);
    }

    result.chars = malloc(result.len + 1);
    for (int i = 0; i < result.len; i++){
        result.chars[i] = str.chars[i + start];
    }
    result.chars[result.len] = '\0';
 
    return result;
}

/* returns copy of main where target is replaced by replacement
 *
 * function: replace
 *
 * param: main - the string to operate on
 *
 * param: target - the target string
 *
 * param: replacement - the string to replace with
 *
 * returns: char * - result of replacement
 * */
char *replace(const char *main, const char *target, const char *replacement){
    char *result;
    if (strlen(main) == 0){
        result = malloc(1);
        result[0] = '\0';
        return result;
    }

    SignedSTRLEN i = find(main, target); 

    if (i == -1){
        result = malloc(strlen(main) + 1);
        strcpy(result, main);
        return result;
    }

    char *after = replace(main + i + strlen(target), target, replacement);

    size_t lenReplacement = strlen(replacement);
    result = malloc(i + lenReplacement + strlen(after) + 1);
    strncpy(result, main, i); strncpy(result + i, replacement, lenReplacement); strcpy(result + i + lenReplacement, after);
    free(after);

    return result;
}

/* returns copy of main where target is replaced by replacement
 *
 * function: strreplace
 *
 * param: main - the string to operate on
 *
 * param: target - the target string
 *
 * param: replacement - the string to replace with
 *
 * returns: string - result of replacement
 * */
string strreplace(const string main, const char *target, const char *replacement){
    char *result = replace(main.chars, target, replacement);
    return (string){.len = strlen(result), .chars = result};
}

bool strcontains(const string main, const char *target){
    for (STRLEN i = 0; i < len(main); i++){
        if (strequalsAt(main, target, i)){
            return true;
        }
    }
    return false;
}

bool contains(const char *haystack, const char *needle){
    for (size_t i = 0; i < strlen(haystack); i++){
        if (equalsAt(haystack, needle, i)){
            return true;
        }
    }
    return false;
}

/* returns lower case str
 * 
 * function: lower
 *
 * param: str, the string to be converted
 *
 * return: char *, pointer to lower case string
 * */
char *lower(const char *str){
    char *result = malloc(strlen(str) + 1);
    size_t i;
    for (; str[i] != '\0'; i++){
        if (str[i] >= 'A' && str[i] <= 'Z'){
            result[i] = str[i] - 'A' + 'a'; continue;
        }
        result[i] = str[i];
    }
    result[i] = '\0';

    return result;
}

/* returns a lower case str
 * 
 * function: strlower
 *
 * param: str, the string to convert
 *
 * return: string, a struct containing the resulting string
 * */ 
string strlower(const char *str){
    return (string){.len = strlen(str), .chars = lower(str)};
}

/* returns upper case str
 *
 * function: upper
 *
 * param: str, the string to convert
 *
 * return: char *, the resulting string
 * */
char *upper(const char *str){
    char *result = malloc(strlen(str) + 1);
    size_t i = 0;
    for (; str[i] != '\0'; i++){
        if (str[i] >= 'a' && str[i] <= 'z'){
            result[i] = str[i] - 'a' + 'A'; continue;
        }
        result[i] = str[i];
    }
    result[i] = '\0';
    return result;
}

/* returns upper case str
 * 
 * function: strupper
 *
 * param: str - the string to convert
 *
 * return: string - a struct containing the resulting string
 * */ 
string strupper(const char *str){
    return (string){.len = strlen(str), .chars = upper(str)};
}

/* returns a copy of str
 *
 * function: copy
 *
 * param: str - the string to copy
 *
 * return: char * - the copy
 * */
char *copy(const char *str){
    size_t len = strlen(str);
    char *result = malloc(len + 1);
    memcpy(result, str, len + 1);
    return result;
}

/* returns a copy of str
 *
 * function: strcopy
 *
 * param: str - the string to copy
 *
 * return: string - the copy
 * */
string strcopy(string str){
    return (string){.len = str.len, .chars = copy(str.chars)};
}

/* returns the count of target in main
 *
 * function: count
 *
 * param: main - the string to search in 
 *
 * param: target - the string to look for
 *
 * return: int - the count
 * */
int count(const char *main, const char *target){
    int c = 0;
    for(int i = 0; main[i] != '\0'; i++){
        if (equalsAt(main, target, i)){
            c++;
        }
    }
    return c;
}

/* returns the count of target in main
 *
 * function: strcount
 *
 * param: main - the string to search in 
 *
 * param: target - the string to look for
 *
 * return: int - the count
 * */
int strcount(const string main, const char *target){
    return count(main.chars, target);
}

/* returns an array of the parts of main split using separator,
 * the number of elements in the array = num + 1, the last element is NULL.
 *
 * function: split
 *
 * param: main - the string to split
 *
 * param: separator - the string separating the elements
 *
 * param: num - the number of elements in the array:
 *  num = 0 -> returns null 
 *  num < 0 -> ignores num, and splits main by separator
 *  num = 1 -> array only contains copy of main.
 *
 * returns: char ** - the array containing the parts
 * */
char **split(const char *main, const char *separator, int num){
    if (num == 0){
        return NULL;
    }

    int partsCount = count(main, separator) + 1;
    size_t separatorLen = strlen(separator);
    partsCount = num > 0 ? min(partsCount, num) : partsCount;
    // when starting, the program is already in the first part
    num = 1;

    // array is terminated by NULL
    char **result = malloc(sizeof(char *) * (partsCount + 1));
    int start = 0;

    for (int i = 0; main[i] != '\0'; i++){
        if (equalsAt(main, separator, i) && num < partsCount){
            result[num - 1] = substr(main, start, i); num++; i += separatorLen - 1; start = i + 1;
        } 
        if (num >= partsCount){
            break;
        }
    }
    result[partsCount - 1] = substr(main, start, strlen(main)); result[partsCount] = NULL;
    return result;
}

/* returns an array of the parts of main split using separator,
 * the number of elements in the array = num + 1, the last element is NULL.
 *
 * function: split
 *
 * param: main - the string to split
 *
 * param: separator - the string separating the elements
 *
 * param: num - the number of elements in the array:
 *  num = 0 -> returns null 
 *  num < 0 -> ignores num, and splits main by separator
 *  num = 1 -> array only contains copy of main.
 *
 * returns: string * - the array containing the parts
 * */
string *strsplit(const string main, const char *separtor, int num){
    char **parts = split(main.chars, separtor, num);
    int i = 0;
    for (;parts[i] != NULL; i++){}
    string *result = malloc(sizeof(string) * (i + 1));
    for (i = 0; parts[i] != NULL; i++){
        result[i] = (string){.len = strlen(parts[i]), .chars = parts[i]};
    }
    result[i] = (string){.len = 0, .chars = NULL}; free(parts);
    return result;
}

/* returns true if string is null, or contains no characters
 *
 * function: strempty
 *
 * param: str - the string to test 
  *
  * returns: bool - is the string empty?
 * */
bool strempty(const string str){
    return str.len == 0;
}

/* returns true if string is null
 *
 * function: strnull
 *
 * param: str - the string 
 *
 * returns: bool - is null?
 * */
bool strnull(const string str){
    return str.chars == NULL;
}

int main(void){
    string str = new("hello;man");
    string *parts = strsplit(str, ";", -1);
    for (int i = 0; !strnull(parts[i]); i++){
        strprint(parts[i]); strfree(1, parts[i]);
    }
    free(parts);
    strfree(1, str);
    return 0;
}
