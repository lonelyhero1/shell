#pragma once
#include <stdint.h>

typedef struct AST AST;

struct AST {
  enum {
    AST_COMMAND,
    AST_ARGUMENT,
    AST_REDIRECTION,
    AST_PIPE,
    AST_LIST,
    AST_FD,
    AST_LITERAL,
  } tag;
  
  union {
    struct AST_COMMAND {
      char *executable;
      AST **arguments;
      int32_t argc;
    } AST_COMMAND;
    
    struct AST_ARGUMENT {
      char *value;
    } AST_ARGUMENT;
    
    struct AST_REDIRECTION {
      AST *command;
      char *file;
      enum {
        AST_REDIRECTION_APPEND_LEFT,  // 0
        AST_REDIRECTION_APPEND_RIGHT, // 1
        AST_REDIRECTION_LEFT,         // 2 (<)
        AST_REDIRECTION_RIGHT,        // 3 (>)
      } AST_REDIRECTION_TYPE;
    } AST_REDIRECTION;

    struct AST_PIPE {
      AST *left;
      AST *right;
    } AST_PIPE;

    struct AST_LIST {
      AST *left;
      AST *right;
      enum {
        AST_LIST_AND,        // 0
        AST_LIST_OR,         // 1
        AST_LIST_PARALLEL,   // 2 (&)
        AST_LIST_SEQUENTIAL, // 3
      } AST_LIST_TYPE;
    } AST_LIST;
  } data;
};

AST *ast_new();
void ast_free(AST *ast);