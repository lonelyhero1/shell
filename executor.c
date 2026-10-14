#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "executor.h"

#define MAX_PATHS 100
char *search_paths[MAX_PATHS] = {"/bin", NULL};

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void execute_node(AST *ast) {
    if (!ast) exit(1);

    if (ast->tag == AST_REDIRECTION) {
        struct AST_REDIRECTION redir = ast->data.AST_REDIRECTION;
        int fd;
        
        if (redir.AST_REDIRECTION_TYPE == 3) { // (>)
            //唯寫 不存在建立 存在清空 權限0644
            fd = open(redir.file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) { print_error(); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        } 
        else if (redir.AST_REDIRECTION_TYPE == 2) { // (<)
            fd = open(redir.file, O_RDONLY);
            if (fd < 0) { print_error(); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        execute_node(redir.command);
    }
    else if (ast->tag == AST_COMMAND) {
        struct AST_COMMAND cmd = ast->data.AST_COMMAND;
        char **args = malloc((cmd.argc + 1) * sizeof(char*));
        for (int i = 0; i < cmd.argc; i++) {
            args[i] = cmd.arguments[i]->data.AST_ARGUMENT.value;
        }
        args[cmd.argc] = NULL;

        char full_path[1024];
        for (int i = 0; search_paths[i] != NULL; i++) {
            snprintf(full_path, sizeof(full_path), "%s/%s", search_paths[i], cmd.executable);
            if (access(full_path, X_OK) == 0) {
                execv(full_path, args);
                exit(1);
            }
        }

        print_error();
        exit(1);
    }
    else if (ast->tag == AST_PIPE) {
        int pipefd[2];
        if (pipe(pipefd) == -1) { print_error(); exit(1); }

        // 左邊子行程
        pid_t pid_left = fork();
        if (pid_left < 0) { print_error(); exit(1); }
        if (pid_left == 0) {
            dup2(pipefd[1], STDOUT_FILENO);

            close(pipefd[0]);
            close(pipefd[1]);

            execute_node(ast->data.AST_PIPE.left);
            exit(1);
        }

        // 右邊子行程
        pid_t pid_right = fork();
        if (pid_right < 0) { print_error(); exit(1); }
        if (pid_right == 0) {
            dup2(pipefd[0], STDIN_FILENO);

            close(pipefd[0]);
            close(pipefd[1]);

            execute_node(ast->data.AST_PIPE.right);
            exit(1);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid_left, NULL, 0);
        waitpid(pid_right, NULL, 0);
        exit(0);
    }
    else if (ast->tag == AST_LIST) {
        if (ast->data.AST_LIST.AST_LIST_TYPE == 2) { // (&)
            pid_t pid_left = -1, pid_right = -1;

            // 左邊子行程
            if (ast->data.AST_LIST.left) {
                pid_left = fork();
                if (pid_left < 0) { print_error(); exit(1); }
                if (pid_left == 0) {
                    execute_node(ast->data.AST_LIST.left);
                    exit(0); // 執行完畢後確保子行程結束
                }
            }

            // 右邊子行程
            if (ast->data.AST_LIST.right) {
                pid_right = fork();
                if (pid_right < 0) { print_error(); exit(1); }
                if (pid_right == 0) {
                    execute_node(ast->data.AST_LIST.right);
                    exit(0);
                }
            }

            if (pid_left > 0) waitpid(pid_left, NULL, 0);
            if (pid_right > 0) waitpid(pid_right, NULL, 0);
            
            exit(0);
        }
    }
    exit(1);
}

void execute_ast(AST *ast, char **raw_tokens) {
    if (!ast || raw_tokens[0] == NULL) return;

    if (strcmp(raw_tokens[0], "exit") == 0) {
        exit(0);
    } 
    else if (strcmp(raw_tokens[0], "cd") == 0) {
        if (raw_tokens[1] == NULL || raw_tokens[2] != NULL) {
            print_error();
        } else if (chdir(raw_tokens[1]) != 0) {
            print_error();
        }
        return;
    }
    else if (strcmp(raw_tokens[0], "path") == 0) {
        for (int j = 0; search_paths[j] != NULL; j++) {
            search_paths[j] = NULL;
        }
        int i = 0;
        while (raw_tokens[i + 1] != NULL && i < MAX_PATHS - 1) {
            search_paths[i] = strdup(raw_tokens[i + 1]);
            i++;
        }
        search_paths[i] = NULL;
        return;
    }

    // fork 
    pid_t pid = fork();
    if (pid < 0) {
        print_error();
    } 
    else if (pid == 0) {
        execute_node(ast);
        exit(1);
    } 
    else {
        int status;
        waitpid(pid, &status, 0);
    }
}