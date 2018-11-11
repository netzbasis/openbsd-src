/*	$OpenBSD$	*/

/*
 * Copyright (c) 2018 Benjamin Baier <ben@netzbasis.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>

#define MODELEN 16

static void __dead usage(void);

int
main(int argc, char *argv[])
{
	int ch;
	size_t modelen;
	char *mode;

	mode = calloc(1, MODELEN);
	if (mode == NULL)
		err(1, "calloc");

	while ((ch = getopt(argc, argv, "p:u:m:")) != -1) {
		switch (ch) {
		case 'p':
			if (pledge(NULL, optarg) == -1)
				err(1, "pledge failed");
			break;
		case 'u':
			if (unveil(optarg, mode) == -1)
				err(1, "unveil failed");
			break;
		case 'm':
			modelen = strlcpy(mode, optarg, MODELEN);
			if (modelen >= MODELEN)
				err(1, "mode truncated");
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (unveil(NULL, NULL) == -1)
		err(1, "unveil locking failed");

	
	printf("%s %s\n", argv[0], argv[1]);

	return 0;
}

static void __dead
usage(void)
{
	fputs("usage: pledge [-p "pledges"] [-m mode] [-u unveil] cmd args ...\n", stderr);
	exit(1);
}
