// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2001-2003,2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 */

#include "command.h"
#include "input.h"
#include "init.h"
#include "io.h"

static cmdinfo_t imap_cmd;

static int
imap_f(int argc, char **argv)
{
	int		count;
	int		nent;
	int		i;
	__u64		last = 0;
	xfs_inogrp_t	*t;
	xfs_fsop_bulkreq_t bulkreq;

	if (argc != 2)
		nent = 1;
	else
		nent = atoi(argv[1]);

	t = malloc(nent * sizeof(*t));
	if (!t)
		return 0;

	bulkreq.lastip  = &last;
	bulkreq.icount  = nent;
	bulkreq.ubuffer = (void *)t;
	bulkreq.ocount  = &count;

	while (xfsctl(file->name, file->fd, XFS_IOC_FSINUMBERS, &bulkreq) == 0) {
		if (count == 0)
			goto out_free;
		for (i = 0; i < count; i++) {
			printf(_("ino %10llu count %2d mask %016llx\n"),
				(unsigned long long)t[i].xi_startino,
				t[i].xi_alloccount,
				(unsigned long long)t[i].xi_allocmask);
		}
	}
	perror("xfsctl(XFS_IOC_FSINUMBERS)");
	exitcode = 1;
out_free:
	free(t);
	return 0;
}

void
imap_init(void)
{
	imap_cmd.name = "imap";
	imap_cmd.cfunc = imap_f;
	imap_cmd.argmin = 0;
	imap_cmd.argmax = 1;
	imap_cmd.args = _("[nentries]");
	imap_cmd.flags = CMD_NOMAP_OK | CMD_FLAG_ONESHOT;
	imap_cmd.oneline = _("inode map for filesystem of current file");

	if (expert)
		add_command(&imap_cmd);
}
