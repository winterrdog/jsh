#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define C_STR char*
#define PVOID void*
#define TOK_BUFSZ 64
#define STR_TOK_DELIM " \t\r\n\a"

// struct to store global data related to the shell's configuration
typedef struct
{
  C_STR line;
  C_STR* args;
} shellConfig;

shellConfig sh_cfg = { NULL, NULL };

// List of builtin shell cmds
C_STR builtin_cmds[] = { "cd", "exit", "help", NULL };

// function prototypes for builtin shell cmds
int32_t
jsh_cd(C_STR args[]);
int32_t
jsh_help(C_STR args[]);
int32_t
jsh_exit(C_STR args[]);

// List of builtin shell cmds' function pointers
int32_t (*builtin_fns[])(C_STR*) = { &jsh_cd, &jsh_exit, &jsh_help, NULL };

// builtin functions implementation

int32_t
jsh_cd(C_STR args[])
{
  if (!args[1]) {
    fprintf(stderr, "jsh: Few arguments were passed to 'cd'\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("jsh");
    }
  }
  return 1;
}

int32_t
jsh_help(C_STR args[])
{
  printf("winterrdog's jSH\n");
  printf("Type program names and arguments then hit ENTER.\n");
  printf("The following are shell builtins: \n");

  uint8_t i = 0;
  for (; builtin_cmds[i] != NULL; printf("\t- %s\n", builtin_cmds[i++]))
    ;

  printf("Use the 'man' command for information about other programs.\n");
  return 1;
}

int32_t
jsh_exit(C_STR args[])
{
  return 0;
}

void
free_mem(PVOID ptr)
{
  if (ptr) {
    free(ptr);
    ptr = NULL;
  }

  return;
}

void
free_shell_mem(void)
{
  free_mem(sh_cfg.args);
  free_mem(sh_cfg.line);
}

C_STR
jsh_read_line(void)
{
  size_t bufsz = 0;
  C_STR line = NULL;

  if (getline(&line, &bufsz, stdin) == -1) {
    free_mem(line);

    if (feof(stdin)) {
      exit(EXIT_SUCCESS);
    } else {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

C_STR*
jsh_splitln(C_STR line)
{
  int32_t bufsz = TOK_BUFSZ;

  C_STR* tokens = calloc(bufsz, sizeof(C_STR));
  if (!tokens) {
    fprintf(stderr, "jsh: memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  int32_t pos = 0;
  C_STR token = NULL;

  for (token = strtok(line, STR_TOK_DELIM); token;
       token = strtok(NULL, STR_TOK_DELIM)) {
    tokens[pos++] = token;

    // expand buffer if it's smaller
    if (pos >= bufsz) {
      bufsz *= 2;

      tokens = realloc(tokens, bufsz * sizeof(C_STR));
      if (!tokens) {
        fprintf(stderr, "jsh: memory allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }

  tokens[pos] = NULL;
  return tokens;
}

int32_t
jsh_launch(C_STR args[])
{
  pid_t pid = 0;
  int32_t status = 0;

  pid = fork();
  if (pid == 0) {
    // child process
    if (execvp(args[0], args) == -1) {
      perror("jsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid <= -1) {
    // error during fork
    perror("jsh");
  } else {
    // parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int32_t
jsh_execute(C_STR args[])
{
  if (!args[0]) {
    // empty cmd was entered
    return 1;
  }

  uint8_t i = 0;
  for (; builtin_cmds[i] != NULL; ++i) {
    if (strncmp(args[0], builtin_cmds[i], strlen(builtin_cmds[i])) == 0) {
      return builtin_fns[i](args);
    }
  }

  return jsh_launch(args);
}

void
jsh_loop(void)
{
  int32_t status = 0;

  do {
    C_STR curr_dir = getenv("PWD");
    if (!curr_dir) {
      return;
    }

    printf("\x1b[36;1;3;7m[%s]\x1b[m> ", curr_dir);
    sh_cfg.line = jsh_read_line();
    sh_cfg.args = jsh_splitln(sh_cfg.line);
    status = jsh_execute(sh_cfg.args);

    free_shell_mem();
  } while (status);
}

int32_t
main(void)
{
  // INITIALIZE: load config files, if any and other initialization required

  // INTERPRET: shell reads and interprets cmds from stdin
  // run command loop
  jsh_loop();

  // TERMINATE: execute shutdown commands and does memory cleanup work
  return EXIT_SUCCESS;
}