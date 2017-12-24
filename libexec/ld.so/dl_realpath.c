/*	$OpenBSD: dl_realpath.c,v 1.6 2017/12/24 01:50:50 millert Exp $ */
/*
 * Copyright (c) 2003 Constantin S. Svintsoff <kostik@iclub.nsu.ru>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/errno.h>
#include <limits.h>
#include "archdep.h"

/*
 * This file was copied from libc/stdlib/realpath.c and modified for ld.so's
 * syscall method which returns -errno.
 */

/*
 * char *dl_realpath(const char *path, char resolved[PATH_MAX]);
 *
 * Find the real name of path, by removing all ".", ".." and symlink
 * components.  Returns (resolved) on success, or (NULL) on failure,
 * in which case the path which caused trouble is left in (resolved).
 */
char *
_dl_realpath(const char *path, char *resolved)
{
	const char *p;
	char *q;
	size_t left_len, resolved_len, next_token_len;
	unsigned symlinks;
	int mem_allocated;
	ssize_t slen;
	char left[PATH_MAX], next_token[PATH_MAX], symlink[PATH_MAX];

	if (path == NULL) {
		return (NULL);
	}

	if (path[0] == '\0') {
		return (NULL);
	}

	if (resolved == NULL) {
		resolved = _dl_malloc(PATH_MAX);
		if (resolved == NULL)
			return (NULL);
		mem_allocated = 1;
	} else
		mem_allocated = 0;

	symlinks = 0;
	if (path[0] == '/') {
		resolved[0] = '/';
		resolved[1] = '\0';
		if (path[1] == '\0')
			return (resolved);
		resolved_len = 1;
		left_len = _dl_strlcpy(left, path + 1, sizeof(left));
	} else {
		if (_dl___getcwd(resolved, PATH_MAX) <= 0) {
			if (mem_allocated)
				_dl_free(resolved);
			else
				_dl_strlcpy(resolved, ".", PATH_MAX);
			return (NULL);
		}
		resolved_len = _dl_strlen(resolved);
		left_len = _dl_strlcpy(left, path, sizeof(left));
	}
	if (left_len >= sizeof(left)) {
		goto err;
	}

	/*
	 * Iterate over path components in `left'.
	 */
	while (left_len != 0) {
		/*
		 * Extract the next path component and adjust `left'
		 * and its length.
		 */
		p = _dl_strchr(left, '/');

		next_token_len = p ? (size_t) (p - left) : left_len;
		_dl_bcopy(left, next_token, next_token_len);
		next_token[next_token_len] = '\0';

		if (p != NULL) {
			left_len -= next_token_len + 1;
			_dl_bcopy(p + 1, left, left_len + 1);
		} else {
			left[0] = '\0';
			left_len = 0;
		}

		if (resolved[resolved_len - 1] != '/') {
			if (resolved_len + 1 >= PATH_MAX) {
				goto err;
			}
			resolved[resolved_len++] = '/';
			resolved[resolved_len] = '\0';
		}
		if (next_token[0] == '\0')
			continue;
		else if (_dl_strcmp(next_token, ".") == 0)
			continue;
		else if (_dl_strcmp(next_token, "..") == 0) {
			/*
			 * Strip the last path component except when we have
			 * single "/"
			 */
			if (resolved_len > 1) {
				resolved[resolved_len - 1] = '\0';
				q = _dl_strrchr(resolved, '/') + 1;
				*q = '\0';
				resolved_len = q - resolved;
			}
			continue;
		}

		/*
		 * Append the next path component and readlink() it. If
		 * readlink() fails we still can return successfully if
		 * it exists but isn't a symlink, or if there are no more
		 * path components left.
		 */
		resolved_len = _dl_strlcat(resolved, next_token, PATH_MAX);
		if (resolved_len >= PATH_MAX) {
			goto err;
		}
		slen = _dl_readlink(resolved, symlink, sizeof(symlink));
		if (slen < 0) {
			switch (slen) {
			case -EINVAL:
				/* not a symlink, continue to next component */
				continue;
			case -ENOENT:
				if (p == NULL)
					return (resolved);
				/* FALLTHROUGH */
			default:
				goto err;
			}
		} else if (slen == 0) {
			goto err;
		} else if (slen == sizeof(symlink)) {
			goto err;
		} else {
			if (symlinks++ > SYMLOOP_MAX) {
				goto err;
			}

			symlink[slen] = '\0';
			if (symlink[0] == '/') {
				resolved[1] = 0;
				resolved_len = 1;
			} else if (resolved_len > 1) {
				/* Strip the last path component. */
				q = _dl_strrchr(resolved, '/') + 1;
				*q = '\0';
				resolved_len = q - resolved;
			}

			/*
			 * If there are any path components left, then
			 * append them to symlink. The result is placed
			 * in `left'.
			 */
			if (p != NULL) {
				if (symlink[slen - 1] != '/') {
					if (slen + 1 >= sizeof(symlink)) {
						goto err;
					}
					symlink[slen] = '/';
					symlink[slen + 1] = 0;
				}
				left_len = _dl_strlcat(symlink, left, sizeof(symlink));
				if (left_len >= sizeof(left)) {
					goto err;
				}
			}
			left_len = _dl_strlcpy(left, symlink, sizeof(left));
		}
	}

	/*
	 * Remove trailing slash except when the resolved pathname
	 * is a single "/".
	 */
	if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
		resolved[resolved_len - 1] = '\0';
	return (resolved);

err:
	if (mem_allocated)
		_dl_free(resolved);
	return (NULL);
}
