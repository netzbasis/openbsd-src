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

static void __dead usage(void);

int
main(int argc, char *argv[])
{
	int i;
	size_t arglen;
	char **options;

	if (argc < 3)
		usage();

	options = reallocarray(NULL, (argc+1), sizeof *options);
	if (options == NULL)
		err(1, "malloc");

	for (i=0; i<argc; i++) {
		options[i] = strdup(argv[i]);
	}
	options[argc] = NULL;

	if (pledge("stdio exec", options[1]) == -1)
		err(1, "pledge");

	
	printf("%s %s %s\n", options[1], options[2], options[3]);
	if (execl("/bin/cat", "/bin/cat", "/tmp/hello", NULL) == -1)
		err(1, "exec");

	return 0;
}

static void __dead
usage(void)
{
	fputs("usage: pledge \"<pledges>\" cmd <options>\n", stderr);
	exit(1);
}
