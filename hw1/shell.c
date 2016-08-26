/*
 * shell.c
 *
 *  Created on: 26 Aug 2016
 *      Author: vmuser
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define INPUT_STRING_SIZE 80

#include "io.h"
#include "parse.h"
#include "process.h"
#include "shell.h"

int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_help(tok_t arg[]);

/* Command Lookup Table Structures */
typedef int cmd_fun_t (tok_t args[]); // cmd functions take token array and return int
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;
// add more commands to this lookup table!
fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_cd, "cd", "change working directory"},
  {cmd_wait, "wait", "waiting for child processes to finish"},
  {cmd_fg, "fg", "putting process in foreground"},
  {cmd_bg, "bg", "putting process in background"},
};

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

void init_shell()
{
  // Check if we are running interactively
  shell_terminal = STDIN_FILENO;

  // Note that we cannot take control of the terminal if the shell is not interactive
  shell_is_interactive = isatty(shell_terminal);

  if(shell_is_interactive){

    // force into foreground
    while(tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp()))
      kill( - shell_pgid, SIGTTIN);

    shell_pgid = getpid();
    // Put shell in its own process group
    if(setpgid(shell_pgid, shell_pgid) < 0){
      perror("Couldn't put the shell in its own process group");
      exit(1);
    }

    // Take control of the terminal
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
  }

  /** YOUR CODE HERE */
  // ignore signals
  SET_SIGNALS( SIG_IGN );
}

/**
 * Add a process to our process list
 */
void add_process(process* p)
{
  /** YOUR CODE HERE */
	if( first_process == NULL ){ // insertion into an empty list
	    first_process = p;
	    p->next = p;
	    p->prev = p;
	  }
	  else {
	    first_process->prev->next = p; // insert at the end of a non-empty list
	    p->prev = first_process->prev;
	    first_process->prev = p;
	    p->next = first_process;
	  }
}

/**
 * Creates a process given the tokens parsed from stdin
 *
 */
process* create_process(tok_t* tokens)
{
  /** YOUR CODE HERE */
	  int outfile = STDOUT_FILENO; // by default, output goes to STDOUT
	  // check if we need to redirect output for this processs
	  char *out_redirect = strstr( tokens, ">" );
	  if( out_redirect != NULL ){
	    // redirect output
	    *out_redirect = NUL; // prune 'inputString'
	    char *output = out_redirect + 1; // cut off the portion with the file name
	    // open a file for writing
	    char *file = getToks(output)[0];
	    outfile = open( file, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
	     // check that the file was opened
	     if( outfile < 0 ) {
	       perror( file );
	       return NULL;
	     }
	  }

	  int infile = STDIN_FILENO;

	  char *in_redirect = strstr(tokens, "<");
	  if(in_redirect != NULL){
		  *in_direct = NUL;
		  char *input = in_indirect+1;
		  char*file = getToks(input)[0];
		  infile = open(file, O_RDONLY);

		  if(infile < 0){
			  perror(file);
			  return NULL;
		  }
	  }

	  bool is background = false;
	  char *bg = strchr(tokens,'&');
	  if(bg!=NULL){
		  is_background=true;
		  bg=NL;
	  }
	  tok_t *t= getToks(inputString);
	  int fundex =lookup(t[0]);
	  if(fundex >=0){
		  cmd_table[fundex.fun(&t[1])];
		  return NULL;
	  }
	  else{
		  process* p = (process*) calloc(1, sizeof(process));
		  p->stdin = infile;
		  p->stdout = outfile;
		  p->stderr = STDERR_FILENO;
		  p->background = is_background;
		  p->next = p->prev =NULL;

		  int argc = 0;
		  for(tok_t *tok =t;  *tok != NUL; ++tok)
			  ++argc;

		  p->argc = argc;
		  p->argv =t;

		  add_process(p);
		  return p;
	  }
  return NULL;

}



int shell (int argc, char *argv[]) {
  pid_t pid = getpid();		 // get current process's PID
  pid_t ppid = getppid();	 // get parent's PID
  pid_t cpid;                // use this to store a child PID

  char *s = malloc(INPUT_STRING_SIZE+1); // user input string
  tok_t *t;			                     // tokens parsed from input
  // if you look in parse.h, you'll see that tokens are just c-style strings

  while((s=freadln(stdin))){
	  process *p = create_process(s);
	  if(p!=NULL){
		 cpid =fork();
		 if(cpid==0){
			 p->pid= getpid();
			 launch_process(p);
		 }
		 else if(cpid==0){
			 p->pid = cpid;
			 if(shell_is_interactive){
				 if(setgpid(cpid, cpid)<0){
					 printf("Failed to put the child into its own process group.\n");
				 }
				 if(p->background){
					 put_in_background(p,false);
				 }
				 else{
					 put_in_foreground(p,false);
				 }
			 }
			 else{
				 wait_for_process(p);
			 }
		 }
		 else{
			 printf("fork() failed\n");
		 }
	  }
	  print_prompt();
  }

  return EXIT_SUCCESS;
}
