/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _SIGNAL_H
#define _SIGNAL_H

typedef unsigned long sigset_t;

struct sigaction 
{
	void (*sa_handler)(int);
	unsigned long sa_mask;
	int	sa_flags;
};

#define SA_NOCLDSTOP	1	/* only value supported now for sa_flags */

#define SIG_SETMASK		0	/* set mask with sigprocmask() */
#define SIG_BLOCK		1	/* set of signals to block */
#define SIG_UNBLOCK		2	/* set of signals to, well, unblock */
#define SIG_SUSPEND		8	/* Change the set of blocked signals to SET,
                                         * and wait for signal */

#define SIG_DFL		(void (*)(int))0	/* default signal action */
#define SIG_IGN		(void (*)(int))1	/* ignore signal */
#define SIG_SGE		(void (*)(int))3	/* signal gets error */
#define SIG_ACK		(void (*)(int))4	/* acknowledge */
#define SIG_ERR		(void (*)(int))-1	/* signal error value */

/* Signals.  */
#define	SIGHUP		1	/* Hangup (POSIX).  */
#define	SIGINT		2	/* Interrupt (ANSI).  */
#define	SIGQUIT		3	/* Quit (POSIX).  */
#define SIGILL      4	/* Illegle instruction (POSIX). */
#define	SIGTRAP		5	/* Trace trap (POSIX).  */
#define SIGABRT     6   /* Abort sigbal (POSIX). */
#define SIGBUS		7	/* Bus error (POSIX). */
#define SIGFPE		8   /* Floting point device error (POSIX). */
#define	SIGKILL		9	/* Kill, unblockable (POSIX).  */
#define	SIGUSR1		10	/* User-defined signal 1 (POSIX).  */
#define SIGSEGV		11	/* Address access error (POSIX). */
#define	SIGUSR2		12	/* User-defined signal 2 (POSIX).  */
#define	SIGPIPE		13	/* Broken pipe (POSIX).  */
#define	SIGALRM		14	/* Alarm clock (POSIX).  */
#define SIGTERM		15  /* Termination (POSIX). */
#define SIGSTKFLT	16  /* Stack fault (POSIX). */
#define	SIGCHLD		17	/* Child status has changed (POSIX).  */
#define	SIGCONT		18	/* Continue (POSIX).  */
#define	SIGSTOP		19	/* Stop, unblockable (POSIX).  */
#define	SIGTSTP		20	/* Keyboard stop (POSIX).  */
#define	SIGTTIN		21	/* Background read from tty (POSIX).  */
#define	SIGTTOU		22	/* Background write to tty (POSIX).  */
#define	SIGDBG		23	/* Debug stop (HOPEN).  */

#define SIGIO		29
//#define NSIG		24	/* signal 0 implied */
#define NSIG		32	/* signal 0 implied */

#ifdef __cplusplus
extern "C" {
#endif

int sigaddset (sigset_t *set, int signo);
int sigdelset (sigset_t *set, int signo);
int sigemptyset (sigset_t *set);
int sigfillset (sigset_t *set);
int sigismember (sigset_t *set, int signo);

void (*signal(int sig, void (*handler)(int)))(int);
int sigpending (sigset_t *set);
int sigsuspend (const sigset_t *set);

int sigprocmask (int how, const unsigned long *a, unsigned long *b);

/* The first argument to kill should be pid_t.  Right now
   <sys/types.h> always defines pid_t to be int.  If that ever
   changes, then we will need to do something else, perhaps along the
   lines of <machine/types.h>.  */
int kill (int, int);
int sigaction (int, const struct sigaction *, struct sigaction *);

int raise (int sig);

#ifdef __cplusplus
}
#endif

#endif /* _SIGNAL_H */
