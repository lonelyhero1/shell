#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

char **tokenize(char *line) {
    int bufsize = 100, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    if (!tokens) return NULL;

    char *token = strtok(line, " \t\r\n\a");
    while (token != NULL) {
        tokens[position++] = token;
        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}