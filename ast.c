#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST *ast_new() {
    return calloc(1, sizeof(AST)); 
}

void ast_free(AST *ast) {
    if (!ast) return;

    switch (ast->tag) {
        case AST_COMMAND: {
            if (ast->data.AST_COMMAND.executable) {
                free(ast->data.AST_COMMAND.executable);
            }
            for (int i = 0; i < ast->data.AST_COMMAND.argc; i++) {
                ast_free(ast->data.AST_COMMAND.arguments[i]);
            }
            free(ast->data.AST_COMMAND.arguments);
            break;
        }
        case AST_ARGUMENT: {
            if (ast->data.AST_ARGUMENT.value) {
                free(ast->data.AST_ARGUMENT.value);
            }
            break;
        }
        case AST_REDIRECTION: {
            if (ast->data.AST_REDIRECTION.file) {
                free(ast->data.AST_REDIRECTION.file);
            }
            ast_free(ast->data.AST_REDIRECTION.command);
            break;
        }
        case AST_PIPE: {
            ast_free(ast->data.AST_PIPE.left);
            ast_free(ast->data.AST_PIPE.right);
            break;
        }
        case AST_LIST: {
            ast_free(ast->data.AST_LIST.left);
            ast_free(ast->data.AST_LIST.right);
            break;
        }
        default:
            break;
    }
    
    free(ast);
}