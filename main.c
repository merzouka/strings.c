#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define SignedSTRLEN long long
#define STRLEN unsigned SignedSTRLEN
#define STRLENSpecifier "%lld"

#define Error(format, ...) char *buf; asprintf(buf, "Error : "format".\n", __VA_ARGS__)
#define IndexOutOfRangeError(index) Error("index '%lld' out of bound", index)
#define MemoryAllocationError Error("couldn't allocate")
#define InvalidArgument(message) Error(message);
#define THROW(error)  error; printf(buf); free(buf); exit(1)

typedef struct {
    STRLEN len;
    char *chars;
} string;

STRLEN len(string str){
    return str.len;
}

char get(string str, STRLEN index){
    if (index >= str.len){
        THROW(IndexOutOfRangeError(index));
    }
    return str.chars[index];
}

void set(string str, STRLEN index, char c){
    if (index > str.len){
        THROW(IndexOutOfRangeError, index);
    }
    if (index == str.len || index == -1){
        char *tmp = realloc(str.chars, len(str) + 1 + 1);
        if (tmp == NULL){
            THROW(MemoryAllocationError, __LINE__);
        } else {
            str.chars = tmp;
        }
        str.chars[str.len] = c; str.chars[str.len + 1] = '\0'; str.len++;
        return;
    }
    str.chars[index] = c;
}

string replaceFrom(string str1, char *str2, STRLEN index){
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

string replaceStrFrom(string str1, string str2, STRLEN index){
    return replaceFrom(str1, str2.chars, index);
}

void freeStr(int num, ...){
    va_list args;
    va_start(args, num);
    string str;
    for (int i = 0; i < num; i++){
        str = va_arg(args, string);
        free(str.chars); str.chars = NULL;
    }
    va_end(args);
}

string new(char *chars){
    if (chars == NULL){
        return (string){.len = 0, .chars = NULL};
    }
    int len = strlen(chars);
    string str = {.len = len, .chars = malloc(len + 1)};
    strcpy(str.chars, chars);
    return str;
}

string join(int num, ...){
    va_list args;
    va_start(args, num);
    string str, result = {.len = 0, .chars = malloc(1)}; result.chars[0] = '\0';
    char *tmp = NULL;
    for (int i = 0; i < num; i++){
        str = (string)va_arg(args, string);
        if (str.chars == NULL){
            continue;
        }
        result.len += str.len;
        tmp = realloc(result.chars, result.len + 1);
        if (tmp == NULL){
            THROW(MemoryAllocationError, __LINE__);
        }
        result.chars = tmp; strcat(result.chars, str.chars);
    }
    return result;
}

void print(char *delimiter, int num, ...){
    if (delimiter == NULL){
        delimiter = "";
    }
    va_list args;
    va_start(args, num);
    for (int i = 0; i < num - 1; i++){
        printf("%s%s", ((string)va_arg(args, string)).chars, delimiter);
    }
    printf("%s\n", ((string)va_arg(args, string)).chars);
    va_end(args);
}

char *toarray(string str){
    char *array = malloc(str.len + 1);
    memcpy(array, str.chars, str.len + 1);
    return array;
}

int compare(string str1, char *str2){
    return strcmp(str1.chars, str2);
}

bool equals(string str1, char *str2){
    return compare(str1, str2) == 0;
}

bool equalsAt(string str1, char *str2, STRLEN index){
    size_t len = strlen(str2);
    if (str1.len < index + len - 1){
        return false;
    }
    return strncmp(str1.chars + index, str2, strlen(str2)) == 0;
}

void replace(string str, char *target, char *replacement){
    string repl = new(replacement);
    string result = new(toarray(str));
}

int find(string str, char *target){
    STRLEN i = 0;
    while (i < str.len){
        if (equalsAt(str, target, i)){
            return i;
        }
    }
    return -1;
}

STRLEN min(STRLEN i1, STRLEN i2){
    return i1 < i2 ? i1 : i2;
}

string substr(string str, long long start, long long end){
    string result;
    if (end >= 0 && end < start){
        THROW(InvalidArgument, "invalid range for subscripting.");
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

int main(void){
    const int l = 4;
    // string arr[] = {new(""), new("hello"), new("dude"), new("fuck")};
    string str = new("hello hell manhell");
    printf("%s\n", equalsAt(str, "hell", 14) ? "true" : "false");
    /*for (int i = 0; i < l; i++){
        freeStr(1, arr[i]);
    }*/
    freeStr(1, str);
    return 0;
}
