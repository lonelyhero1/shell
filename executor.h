#pragma once
#include "ast.h"

void print_error();
void execute_ast(AST *ast, char **tokens);