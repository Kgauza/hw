# Homework 1: Shell #

**Due: 26 August 2016, 11:59 PM**

## Introduction

Now that you've been introduced to some of the intricacies of operating systems and programming in C, you can move onto a more interesting assignment.

You task is to program a simple shell, similar to the Bash shell you have been using. The main purpose of a shell is to allow users to run and manage programs. By building your own shell, you'll become
more familiar with these interfaces and learn a bit more about how programs interact.

Do not be intimidated by the scope of this assignment. We have provided the skeleton code and ample time for you to complete it. 
Read the instructions carefully, work through this section by section and you'll be fine.

## 1 Getting started

You need to download the source code we've provided:
```
$ git clone https://github.com/WITS-COMS2001/hw.git
$ cd hw/hw1
```

We have added starter code for your shell and a simple Makefile in the hw1 directory. It includes a string tokenizer, which splits a string into words. In order to run the shell:
```
$ make
$ ./shell
```
In order to terminate the shell after it starts, either type 'quit' or press CTRL-D.

Use the private GitHub repository we assigned you, you should know how to commit and push from the previous homework. DO NOT use a public repository! 

## 2 Built-In Functions

Every shell supports a number of built-in commands, which are functions in the shell itself rather than external programs.
For example, the *quit* command needs to be implemented as a built-in command, because it exits the shell itself. The only two built-ins currently supported in your shell are *?*, which brings up the help menu, and *quit*, which exits the shell.

The skeleton code we have provided for you in 'shell.c' has a dispatcher for built-in commands. Look at the current code provided for *quit* and *?* for an idea on how to integrate the following commands.

### 2.1 pwd

Programs have a working directory when they are executed. You have experienced this whenever you try to open a file in a program, it usually searches for the filename relative to the directory where you executed the program.  
*pwd*, or 'print-working-directory', prints the current working directory to standard output. 

* Implement *pwd* as a built-in command. (Hint: Use Google; there's a C function to get the working directory)
* Update your shell's read loop to print the current working directory along with the line number. The line number should increment after each command.

For example, say that your current directory is "/home/John/Desktop":
```
$ ./shell
0 /home/John/Desktop: pwd
/home/John/Desktop
1 /home/John/Desktop: quit
```

### 2.2 cd

The *cd* command, "change directory", is another typical built-in command. It changes the current working directory of the shell.
E.g. ```cd /bin``` changes the working directory to '/bin'.

* Implement the *cd* command, which takes in one argument, a directory name, and changes the current working directory to the specified directory.

You should notice how *cd* in Bash handles the special characters tilde '\~', and dash '-'.  
'\~': Expands to the home directory of the current user. For example, say your username is 'John', then ```cd ~``` is the same as ```cd /home/John```, and ```cd ~/Desktop``` is the same as ```cd /home/John/Desktop```.  
'-': Return to the previous working directory. For example:
```
$ ./shell
0 /home/John/Desktop: cd /home/John
1 /home/John: cd /bin
2 /bin: cd -
3 /home/John: quit
```

* Update your *cd* command to handle '\~'.
* Update your *cd* command to handle '-'.

Once you're done, push your code to your private repository.
```
git commit -m "Finished adding basic functionality."
git push https://github.com/WITS-COMS2001/<student_number> master
```
You should commit your code periodically, so you can go back to a previous version of your code if you want to.

## 3 Execute Programs

Currently, if you try to type something into your shell that isn't a built-in command, you'll get a message that the shell doesn't know how to execute programs. Modify your shell so that it can execute programs when they are entered into the shell. The first word of the command is the name of the program. The rest of the words are the command-line arguments to the program.
For this step, you can assume that the first word of the command will be the full path to the program. So instead of running wc, you would have to run /usr/bin/wc. In the next section, you will implement support for simple program names like wc.
You should use the functions defined in tokenizer.c for separating the input text into words. You do not need to support any parsing features that are not supported by tokenizer.c. Once you implement this step, you should be able to execute programs, like this:
```
$ ./shell
0 /home/John/Dekstop/hw/hw1: /usr/bin/wc shell.c
77 262 1843 shell.c
1 /home/John/Dekstop/hw/hw1: quit
```
We'll give you a hint to start off: when your shell needs to execute a program, it should fork a child process, which calls one of the
exec functions such as *execv* (Google them) to run the new program. The parent process should wait until the child process completes and then listen for more commands.

* Implement running executables with the full pathname specified.

In anticipation of future sections where you need to manage multiple processes, you should use the structure we defined for you in 'process.h' to implement launching new processes in 'process.c'.

## 4 Path Resolution

You probably found that it was a pain to test your shell in the previous part, because you had to type the
full pathname of every program. Luckily, every program has access to a set of environment variables,
which is structured as a hashtable of string keys to string values. One of these environment variables is
the PATH variable. You can print the PATH variable of your current environment in Bash:
```
$ echo $PATH
/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:...
```
When a shell executes a program like *wc*, it looks for a program called 'wc' in each directory on
the PATH environment variable and runs the first one that it finds. The directories on the path are separated with a colon.

Modify your shell so that it uses the PATH variable from the environment to resolve program names.
Typing in the full pathname of the executable should still be supported. Do not use the *execvp*, *execvpe*, or *execlp* functions! 
We will search for these in your code, and you won't receive a mark for this section if they are found. You need to implement your own PATH resolution.

* Implement path resolution for running executables.

You should now be able to run any executable in the folders in PATH without specifying the full pathname. For example:
```
$ ./shell
0 /home/John/Dekstop/hw/hw1: wc shell.c
77 262 1843 shell.c
1 /home/John/Dekstop/hw/hw1: quit
```

## 5 Input/Output Redirection

When running programs, it is sometimes useful to provide input from a file or to direct output to a file. 
The syntax "[process] > [file]" tells your shell to redirect the process's standard output to an file. 
Similarly, the syntax "[process] < [file]" tells your shell to feed the contents of a file to the process's standard input. To simplify this task, you may assume that there will always be spaces around special characters '<' and '>'.

* Modify your shell so that it supports redirecting stdin and stdout to files.

For example:
```
$ ./shell
0 /home/John/Dekstop/hw/hw1: wc shell.c > out.txt
1 /home/John/Dekstop/hw/hw1: cat out.txt
77 262 1843 shell.c
2 /home/John/Dekstop/hw/hw1: quit
```

You do **not** need to support redirection for shell built-in commands. You do **not** need to support stderr redirection, nor appending to files with "[process] >> [file]". 

## 6 Signal Handling and Terminal Control

Now for the complicated part. You may have to restructure significant portions of your code to do the final sections if you haven't implemented process handling as we specified in the skeleton code.
Read through everything first and do some planning before you start coding.

Most shells let you stop or pause processes with special key strokes. These special keystrokes, such as
Ctrl-C or Ctrl-Z, work by sending signals to the shell's subprocesses. For example, pressing CTRL-C
sends the SIGINT signal which usually stops the current program, and pressing CTRL-Z sends the SIGTSTP
signal which usually sends the current program to the background. If you try these keystrokes in your shell, the signals are sent directly to the shell process itself. This is not what we want, since for example
attempting to CTRL-Z a subprocess of your shell will also stop the shell itself. We want to have the
signals affect only the subprocesses that our shell creates.

Before we explain how you can achieve this effect, let's discuss some more operating system concepts.

### 6.1 Process Groups

You know that every process has a unique process ID (pid). Every process also has a (possibly non-unique) process group ID (pgid), which by default, is the same as the pgid of its parent process.
Processes can get and set their process group ID with getpgid(), setpgid(), getpgrp(), or setpgrp().
Keep in mind that, when your shell starts a new program, that program might require multiple processes to function correctly. All of these processes will inherit the same process group ID of the
original process. So, it may be a good idea to put each shell subprocess in its own process group, to
simplify your bookkeeping. When you move each subprocess into its own process group, the pgid should
be equal to the pid.

### 6.2 Foreground Terminal

Every terminal has an associated "foreground" process group ID. When you type CTRL-C, your terminal
sends a signal to every process inside the foreground process group. You can change which process group
is in the foreground of a terminal with "tcsetpgrp(int fd, pid_t pgrp)". The fd should be 0 for standard input.

### 6.3 Signals

Signals are asynchronous messages that are delivered to processes. They are identified by their signal
number, but they also have human-friendly names that all start with SIG. Some common ones include:  
* **SIGINT** - Delivered when you type CTRL-C. By default, this stops the program.
* **SIGKILL** - There is no keyboard shortcut for this. This signal stops the program forcibly and cannot be overridden by the program. (Most other signals can be ignored by the program.)
* **SIGTERM** - There is no keyboard shortcut for this either. It behaves the same way as SIGQUIT.
* **SIGQUIT** - Delivered when you type CTRL-\\. By default, this also stops the program, but programs treat this signal more seriously than SIGINT. This signal also attempts to produce a core dump of the program before exiting.
* **SIGTSTP** - Delivered when you type CTRL-Z. By default, this pauses the program. In bash, if you type CTRL-Z, the current program will be paused and bash (which can detect that you paused the current program) will start accepting more commands.
* **SIGCONT** - Delivered when you run 'fg' or 'fg %NUMBER' in Bash. This signal resumes a paused program.
* **SIGTTIN** - Delivered to background processes that are trying to read input from the keyboard. By default, this pauses the program, since background processes cannot read input from the keyboard. When you resume the background process with SIGCONT and put it in the foreground, it can try to read input from the keyboard again.
* **SIGTTOU** - Delivered to background processes that are trying to write output to the terminal console, but there is another foreground process that is using the terminal. Behaves the same as SIGTTIN by default.

You can use ```kill -TERM PID``` to send SIGTERM to the process with process id PID.  
You can use the 'signal' function in C to change how signals are handled by the current process. Your shell
should basically ignore most of these signals, whereas the shell's subprocesses should respond with the
default action. Reading ```man 2 signal``` and ```man 7 signal``` will provide more information. Be sure to check out the SIG_DFL and
SIG_IGN constants. For more information about how signals work, you can view the tutorial at <a href="https://www.usna.edu/Users/cs/aviv/classes/ic221/s14/lec/16/lec.html">this link</a>.
You should ensure that each program you start is in its own process group. When you start a process,
its process group should be placed in the foreground.

* Update your shell to ignore most of the above signals, sending them to the spawned subprocesses instead. 

**Beware**: forked processes will inherit the signal handlers of the original process, so you have to reset to default signal handling for subprocesses.

The SIGINT command should no longer terminate your shell when a subprocess is running. For example:
```
$ ./shell
0 /home/John/Dekstop: sleep 10
CTRL-C
1 /home/John/Dekstop: quit
```

## 7 Background Processing

So far, your shell waits for each program to finish before starting the next one. Many shells allow you run
a command in the background by putting an ampersand, '&', at the end of the command line. After the background
program is started, the shell allows you to start more processes without waiting for background process
to finish.

### 7.1 &

Modify your shell so that it runs commands that end in an '&' in the background. You only need
to support background processing for external programs, not built-in commands. Once you've implemented this
part, you should be able to run programs in the background with a command such as ```sleep 10 &```.
You should also add a new built-in command *wait*, which waits until all background jobs have
terminated before returning to the prompt. You can assume that there will always be a space before the '&' character. You can assume that, if
there is an '&' character, it will be the last token on that line.

* Update your shell to run any external program (not built-in) that ends in an '&' in the background.
* Implement the built-in *wait* command.

### 7.2 Foreground/Background Switching

Now that you can execute processes in the background, we need some way to bring them back to the foreground. Code the following built-in command:  
```fg [pid]```: Move the process with id pid to the foreground. The process should resume if it was
paused. If pid is not specified, then move the most recently launched process to the foreground.
You should ensure that each program you start is in its own process group. The process group id of
a program should be the same as its pid. You should keep a list of all programs you've started, whether
they are in the foreground or background, we have provided such a structure in 'process.h'. Inside this list, the "struct termios"
stores the terminal settings of each program.

* Implement the built-in *fg* command.

## 8 Submission

This assignment is due **26 August 2016, 11:59 PM**. Please ensure your final code is in your private repository by then! 

To submit, as usual, commit your changes and push to your private repository:
```
git commit
git push https://github.com/WITS-COMS2001/<student_number> master
```

If your code doesn't compile or run, you will get 0. If you haven't quite finished the last sections, it's better to submit code where the first parts are in working order than half-complete code where nothing works.

Please ensure that your shell doesn't have any extraneous output for debugging. That is, any time a built-in or a process is run with your shell, only the output of the built-in or process
should be printed. Please do not print anything extra, as this can disrupt our marking and may influence your score.

Lastly, ensure you have separate folders for each homework. All your source code for this assignment should be in a folder called 'hw1' (case sensitive) in the root of your repository.