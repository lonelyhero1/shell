#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "main.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "executor.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_MAGENTA "\033[1;35m"

void print_banner() {
    printf(COLOR_CYAN);
    printf("=======================================================\n");
    printf("|       _ _                 _                 _       |\n");
    printf("|      (_) | __ _ _ __ ___ (_)_ __   __ _ ___| |__    |\n");
    printf("|      | | |/ _` | '_ ` _ \\| | '_ \\ / _` / __| '_ \\   |\n");
    printf("|      | | | (_| | | | | | | | | | | (_| \\__ \\ | | |  |\n");
    printf("|     _/ |_|\\__,_|_| |_| |_|_|_| |_|\\__, |___/_| |_|  |\n");
    printf("|    |__/                           |___/             |\n");
    printf("=======================================================\n");
    printf(COLOR_RESET);
}

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    int is_interactive = 1;

    if (argc == 2) {
        is_interactive = 0;
        input = fopen(argv[1], "r");
        if (!input) {
            print_error();
            exit(1);
        }
    } else if (argc > 2) {
        print_error();
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    
    if (is_interactive) {
        print_banner();
    }
    while (1) {
        if (is_interactive) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                char *last_dir = strrchr(cwd, '/');
                if (last_dir != NULL && strlen(cwd) > 1) {
                    last_dir++;
                } else {
                    last_dir = cwd;
                }
                printf(COLOR_GREEN PROMPT COLOR_RESET ":" COLOR_CYAN "%s" COLOR_MAGENTA "> " COLOR_RESET, last_dir);
            } else {
                printf(COLOR_GREEN PROMPT COLOR_MAGENTA "> " COLOR_RESET);
            }
            fflush(stdout);
        }

        if (getline(&line, &len, input) == -1) break;

        char **tokens = tokenize(line);
        if (!tokens) continue;

        AST *ast = parse_command(tokens);
        
        execute_ast(ast, tokens);

        ast_free(ast);
        free(tokens);
    }

    free(line);
    if (input != stdin) fclose(input);

    return 0;
}