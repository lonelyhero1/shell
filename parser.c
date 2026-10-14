#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

AST *parse_command(char **tokens) {
    if (!tokens || !tokens[0]) return NULL;

    int argc = 0;
    while (tokens[argc] != NULL) argc++;
    // (&)
    for (int i = argc - 1; i >= 0; i--) {
        if (strcmp(tokens[i], "&") == 0) {
            AST *node = ast_new();
            node->tag = AST_LIST;
            node->data.AST_LIST.AST_LIST_TYPE = 2;
            
            tokens[i] = NULL;
            node->data.AST_LIST.left = parse_command(tokens);
            if (tokens[i + 1] != NULL) {
                node->data.AST_LIST.right = parse_command(&tokens[i + 1]);
            } else {
                node->data.AST_LIST.right = NULL;
            }
            return node;
        }
    }
    // (Pipe)
    for (int i = argc - 1; i >= 0; i--) {
        if (strcmp(tokens[i], "|") == 0) {
            AST *node = ast_new();
            node->tag = AST_PIPE;
            
            tokens[i] = NULL;
            node->data.AST_PIPE.left = parse_command(tokens);
            node->data.AST_PIPE.right = parse_command(&tokens[i + 1]);
            return node;
        }
    }

    // (< 與 >)
    for (int i = argc - 1; i >= 0; i--) {
        if (strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "<") == 0) {
            AST *node = ast_new();
            node->tag = AST_REDIRECTION;
            
            if (strcmp(tokens[i], ">") == 0) {
                node->data.AST_REDIRECTION.AST_REDIRECTION_TYPE = 3;
            } else {
                node->data.AST_REDIRECTION.AST_REDIRECTION_TYPE = 2;
            }

            if (tokens[i + 1] == NULL) return NULL;
            node->data.AST_REDIRECTION.file = strdup(tokens[i + 1]);

            tokens[i] = NULL;
            node->data.AST_REDIRECTION.command = parse_command(tokens);
            return node;
        }
    }

    // 處理一般指令
    AST *cmd_node = ast_new();
    cmd_node->tag = AST_COMMAND;
    cmd_node->data.AST_COMMAND.executable = strdup(tokens[0]);
    cmd_node->data.AST_COMMAND.argc = argc;
    cmd_node->data.AST_COMMAND.arguments = calloc(argc, sizeof(AST*));

    for (int i = 0; i < argc; i++) {
        AST *arg_node = ast_new();
        arg_node->tag = AST_ARGUMENT;
        arg_node->data.AST_ARGUMENT.value = strdup(tokens[i]);
        cmd_node->data.AST_COMMAND.arguments[i] = arg_node;
    }

    return cmd_node;
}