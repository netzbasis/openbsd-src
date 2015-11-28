/*	$OpenBSD: hotplugd.c,v 1.13 2015/11/19 06:05:40 deraadt Exp $	*/
/*
 * Copyright (c) 2004 Alexander Yurchenko <grange@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Devices hot plugging daemon.
 */

#include <sys/types.h>
#include <sys/device.h>
#include <sys/hotplug.h>
#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#define _PATH_DEV_HOTPLUG		"/dev/hotplug"
#define _PATH_ETC_HOTPLUG		"/etc/hotplug"
#define _PATH_ETC_HOTPLUG_ATTACH	_PATH_ETC_HOTPLUG "/attach"
#define _PATH_ETC_HOTPLUG_DETACH	_PATH_ETC_HOTPLUG "/detach"
#define _LOG_TAG			"hotplugd"
#define _LOG_FACILITY			LOG_DAEMON
#define _LOG_OPT			(LOG_NDELAY | LOG_PID)

volatile sig_atomic_t quit = 0;
char *device = _PATH_DEV_HOTPLUG;
int devfd = -1;

void exec_script(const char *, int, char *);

void sigchild(int);
void sigquit(int);
__dead void usage(void);

int
main(int argc, char *argv[])
{
	int ch;
	struct sigaction sact;
	struct hotplug_event he;

	if (pledge("stdio rpath proc exec", NULL) == -1)
		err(1, "pledge");

	while ((ch = getopt(argc, argv, "d:")) != -1)
		switch (ch) {
		case 'd':
			device = optarg;
			break;
		default:
			usage();
			/* NOTREACHED */
		}

	argc -= optind;
	argv += optind;
	if (argc > 0)
		usage();

	if ((devfd = open(device, O_RDONLY)) == -1)
		err(1, "%s", device);

	bzero(&sact, sizeof(sact));
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
	sact.sa_handler = sigquit;
	sigaction(SIGINT, &sact, NULL);
	sigaction(SIGQUIT, &sact, NULL);
	sigaction(SIGTERM, &sact, NULL);
	sact.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sact, NULL);
	sact.sa_handler = sigchild;
	sact.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sact, NULL);

	openlog(_LOG_TAG, _LOG_OPT, _LOG_FACILITY);
	if (daemon(0, 0) == -1)
		err(1, "daemon");

	syslog(LOG_INFO, "started");

	while (!quit) {
		if (read(devfd, &he, sizeof(he)) == -1) {
			if (errno == EINTR)
				/* ignore */
				continue;
			syslog(LOG_ERR, "read: %m");
			exit(1);
		}

		switch (he.he_type) {
		case HOTPLUG_DEVAT:
			syslog(LOG_INFO, "%s attached, class %d",
			    he.he_devname, he.he_devclass);
			exec_script(_PATH_ETC_HOTPLUG_ATTACH, he.he_devclass,
			    he.he_devname);
			break;
		case HOTPLUG_DEVDT:
			syslog(LOG_INFO, "%s detached, class %d",
			    he.he_devname, he.he_devclass);
			exec_script(_PATH_ETC_HOTPLUG_DETACH, he.he_devclass,
			    he.he_devname);
			break;
		default:
			syslog(LOG_NOTICE, "unknown event (0x%x)", he.he_type);
		}
	}

	syslog(LOG_INFO, "terminated");

	closelog();
	close(devfd);

	return (0);
}

void
exec_script(const char *file, int class, char *name)
{
	char strclass[8];
	pid_t pid;

	snprintf(strclass, sizeof(strclass), "%d", class);

	if (access(file, X_OK | R_OK) == -1) {
		if (errno != ENOENT)
			syslog(LOG_ERR, "%s: %m", file);
		return;
	}

	if ((pid = fork()) == -1) {
		syslog(LOG_ERR, "fork: %m");
		return;
	}
	if (pid == 0) {
		/* child process */
		execl(file, basename(file), strclass, name, (char *)NULL);
		syslog(LOG_ERR, "execl %s: %m", file);
		_exit(1);
		/* NOTREACHED */
	}
}

/* ARGSUSED */
void
sigchild(int signum)
{
	struct syslog_data sdata = SYSLOG_DATA_INIT;
	int saved_errno, status;
	pid_t pid;

	saved_errno = errno;

	sdata.log_tag = _LOG_TAG;
	sdata.log_fac = _LOG_FACILITY;
	sdata.log_stat = _LOG_OPT;

	while ((pid = waitpid(WAIT_ANY, &status, WNOHANG)) != 0) {
		if (pid == -1) {
			if (errno == EINTR)
				continue;
			if (errno != ECHILD)
				syslog_r(LOG_ERR, &sdata, "waitpid: %m");
			break;
		}

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0) {
				syslog_r(LOG_NOTICE, &sdata,
				    "child exit status: %d",
				    WEXITSTATUS(status));
			}
		} else {
			syslog_r(LOG_NOTICE, &sdata,
			    "child is terminated abnormally");
		}
	}

	errno = saved_errno;
}

/* ARGSUSED */
void
sigquit(int signum)
{
	quit = 1;
}

__dead void
usage(void)
{
	extern char *__progname;

	fprintf(stderr, "usage: %s [-d device]\n", __progname);
	exit(1);
}
