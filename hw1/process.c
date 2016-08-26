/*
 * process.c
 *
 *  Created on: 26 Aug 2016
 *      Author: vmuser
 */


#include "process.h"
#include "shell.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <termios.h>

/**
 * Executes the process p.
 * If the shell is in interactive mode and the process is a foreground process,
 * then p should take control of the terminal.
 */
void launch_process(process *p)
{
  /** YOUR CODE HERE */
	SET_SIGNALS(SIG_DFL);
	dup2(p->stdin, STDIN_FILENO);
	dup2(p->stdout, STDOUT_FILENO);
	exec(p->argv[0],p->argv);
	perror("exec");
	exit(EXIT_FAILURE);
}

/**
 * Put a process in the foreground. This function assumes that the shell
 * is in interactive mode. If the cont argument is true, send the process
 * group a SIGCONT signal to wake it up.
 */
void
put_process_in_foreground (process *p, int cont)
{
  /** YOUR CODE HERE */
	if(tcestpgrp(shell_terminal,p->pid)<0)
		printf("Failed to set the process group of the terminal");

	if(resume){
		if(!p->completed)
			p->stopped=false;

		if(kill(-p->pid, SIGCONT)<0)
			perror("kill (SIGCONT)");
	}

	wait_for_process(p);
}

/**
 * Put a process in the background. If the cont argument is true, send
 * the process group a SIGCONT signal to wake it up.
 */
void
put_process_in_background (process *p, int cont)
{
  /** YOUR CODE HERE */
	if(resume){
		if(!p->completed)
			p->stopped = false;

		if(kill(-p->pid, SIGCONT)<0)
			perror("kill (SICONT)");
	}
}
