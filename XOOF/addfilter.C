// -*- C++ -*-
// $RCSfile: addfilter.C,v $
// $Revision: 1.4 $
// $Author: langer $
// $Date: 2005-07-30 20:16:08 $

/* This software was produced by NIST, an agency of the U.S. government,
 * and by statute is not subject to copyright in the United States.
 * Recipients of this software assume all responsibilities associated
 * with its operation, modification and maintenance. However, to
 * facilitate maintenance we ask that before distributing modifed
 * versions of this software, you first contact the authors at
 * oof_manager@ctcms.nist.gov. 
 */


/*
 * FILE *addfilter(filter, file, mode)
 * char *filter;
 * FILE *file;
 * char *mode;
 *
 * Insert the given filter ("filter") into an existing input or
 * output stream ("file").  "mode" is "r" for input and "w" for output.
 * The original stream is closed, and the new one is returned.
 */

#include "charstring.h"
#include <stdio.h>
#include <sys/errno.h>
#include "ctype.h"
#include "stdlib.h"
#include <unistd.h>

static char **makeargs(const CharString&);
static void execerror(char*);
extern int errno;

FILE *addfilter(const CharString &filter, FILE *file, const char *modestr) {
    char **argv;
    int filedes[2];
    char mode = modestr[0];
    int fd = fileno(file);
    FILE *newfile;
    
    if(file == NULL) return NULL;
    if(mode == 'w') fflush(file);
    
    /* Create a pipe */
    if(pipe(filedes) == -1) {
	perror("addfilter: can't open pipe!");
	errno = 0;
	return NULL;
    }

    switch(fork()) {
    case -1:
	perror("addfilter: can't fork!");
	return NULL;

	/* Child */
    case 0:
	if(mode == 'r') {               /* Child is writing to parent */
	    if(dup2(filedes[1], 1) == -1) {     /* Connect stdout to pipe */
		perror("addfilter: Child process can't write to pipe!");
		exit(1);
	    }
	    if(dup2(fd, 0) == -1) {      /* Connect stdin to file */
		perror("addfilter: Child process can't read from file!");
		exit(1);
	    }
	}
	else if(mode == 'w') {          /* Child is reading from parent */
	    if(dup2(filedes[0], 0) == -1) {     /* Connect stdin to pipe */
		perror("addfilter: Child process can't read from pipe!");
		exit(1);
	    }
	    if(dup2(fd, 1) == -1) {            /* Connect stdout to file */
		perror("addfilter: Child process can't write to file!");
		exit(1);
	    }
	}
	close(filedes[0]);
	close(filedes[1]);
	close(fd);

	argv = makeargs(filter);
	execvp(argv[0], argv);
	execerror(argv[0]);
	_exit(1);

	/* Parent */
    default:
	if(mode == 'r') {          /* Parent reads from child */
	    newfile = fdopen(filedes[0], "r");
	    close(filedes[1]);
	}
	else if(mode == 'w') {     /* Parent writes to child */
	    newfile = fdopen(filedes[1], "w");
	    close(filedes[0]);
	}
	fclose(file);
	return newfile;
    }
}


/********************************************************************/

/* 
 * Construct argc and argv from a string. The argv list must end
 * with char *0. This is called only within the child process and
 * before execl(), so arrays can be allocated but don't have to be
 * freed.
 */

#define NULLSTRING ""

static char **makeargs(const CharString &str) {
    char *ptr = str.charstar();
    int argc = 0;
    char **argv = 0;

    while(isspace(*ptr)) ptr++;
    if(*ptr == '\0') return argv;

    for(;;) {            /* Loop over string */
	if(argc == 0)     /* Allocate space for arg */
	    argv = (char **) malloc(sizeof(char *));
	else
	    argv = (char **) realloc((char *) argv,
	        (unsigned)((argc+1)*sizeof(char *)));
	argv[argc] = ptr;
	argc++;
	while(!isspace(*ptr) && *ptr != '\0') ptr++;
	if(isspace(*ptr)) {
	    *ptr = '\0';    /* Terminate arg */
	    ptr++;          /* Beginning of next arg */
	}
	else {             /* At end of string */
	    argv = (char **) realloc((char *) argv,
	        (unsigned)((argc+1)*sizeof(char *)));
	    argv[argc] = (char *) 0;
	    return argv;
	}
    }
}

/*************************************************************/

/* Print execution error message */

#define ERRMSG "addfilter: Can't execute "
#include <string.h>

static void execerror(char *what) {
    size_t len = strlen(ERRMSG) + strlen(what) + 2;
    char *errormsg = (char*) malloc(len);
    sprintf(errormsg, "%s%s!", ERRMSG, what);
    perror(errormsg);
    free(errormsg);
}
