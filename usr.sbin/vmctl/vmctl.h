/*	$OpenBSD: vmctl.h,v 1.27 2018/10/19 10:12:39 reyk Exp $	*/

/*
 * Copyright (c) 2015 Reyk Floeter <reyk@openbsd.org>
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

#ifndef VMCTL_PARSER_H
#define VMCTL_PARSER_H

#define VMCTL_CU	"/usr/bin/cu"

enum actions {
	NONE,
	CMD_CONSOLE,
	CMD_CREATE,
	CMD_LOAD,
	CMD_LOG,
	CMD_RELOAD,
	CMD_RESET,
	CMD_START,
	CMD_STATUS,
	CMD_STOP,
	CMD_STOPALL,
	CMD_PAUSE,
	CMD_UNPAUSE,
	CMD_SEND,
	CMD_RECEIVE,
};

struct ctl_command;

struct parse_result {
	enum actions		 action;
	uint32_t		 id;
	char			*name;
	char			*path;
	char			*isopath;
	long long		 size;
	int			 nifs;
	char			**nets;
	int			 nnets;
	size_t			 ndisks;
	char			**disks;
	int			*disktypes;
	int			 verbose;
	char			*instance;
	unsigned int		 flags;
	unsigned int		 mode;
	struct ctl_command	*ctl;
};

struct ctl_command {
	const char		*name;
	enum actions		 action;
	int			(*main)(struct parse_result *, int, char *[]);
	const char		*usage;
	int			 has_pledge;
};

struct imsgbuf	*ibuf;

#define ALIGN(sz, align)	((sz + align - 1) & ~(align - 1))
#define MIN(a,b)		(((a)<(b))?(a):(b))

/* main.c */
int	 vmmaction(struct parse_result *);
int	 parse_ifs(struct parse_result *, char *, int);
int	 parse_network(struct parse_result *, char *);
int	 parse_size(struct parse_result *, char *, long long);
int	 parse_disktype(const char *, const char **);
int	 parse_disk(struct parse_result *, char *, int);
int	 parse_vmid(struct parse_result *, char *, int);
int	 parse_instance(struct parse_result *, char *);
void	 parse_free(struct parse_result *);
int	 parse(int, char *[]);
__dead void
	 ctl_openconsole(const char *);

/* vmctl.c */
int	 open_imagefile(int, const char *, int,
	    struct virtio_backing *, off_t *);
int	 create_imagefile(int, const char *, const char *, long, const char **);
int	 create_raw_imagefile(const char *, long);
int	 create_qc2_imagefile(const char *, const char *, long);
int	 vm_start(uint32_t, const char *, int, int, char **, int,
	    char **, int *, char *, char *, char *);
int	 vm_start_complete(struct imsg *, int *, int);
void	 terminate_vm(uint32_t, const char *, unsigned int);
int	 terminate_vm_complete(struct imsg *, int *, unsigned int);
void	 pause_vm(uint32_t, const char *);
int	 pause_vm_complete(struct imsg *, int *);
void	 unpause_vm(uint32_t, const char *);
int	 unpause_vm_complete(struct imsg *, int *);
void	 send_vm(uint32_t, const char *);
void	 vm_receive(uint32_t, const char *);
int	 receive_vm_complete(struct imsg *, int *);
int	 check_info_id(const char *, uint32_t);
void	 get_info_vm(uint32_t, const char *, enum actions, unsigned int);
int	 add_info(struct imsg *, int *);
void	 print_vm_info(struct vmop_info_result *, size_t);
void	 terminate_all(struct vmop_info_result *, size_t, unsigned int);
__dead void
	 vm_console(struct vmop_info_result *, size_t);

#endif /* VMCTL_PARSER_H */
