/*
 * Copyright (C) 2001-2004 Sistina Software, Inc. All rights reserved.
 * Copyright (C) 2004-2006 Red Hat, Inc. All rights reserved.
 *
 * This file is part of LVM2.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License v.2.1.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "tools.h"

static int lvscan_single(struct cmd_context *cmd, struct logical_volume *lv,
			 void *handle __attribute__((unused)))
{
	struct lvinfo info;
	int inkernel, snap_active = 1;
	dm_percent_t snap_percent;     /* fused, fsize; */

	const char *active_str, *snapshot_str;

	if (!arg_count(cmd, all_ARG) && !lv_is_visible(lv))
		return ECMD_PROCESSED;

	inkernel = lv_info(cmd, lv, 0, &info, 0, 0) && info.exists;
	if (lv_is_cow(lv)) {
		if (inkernel &&
		    (snap_active = lv_snapshot_percent(lv, &snap_percent)))
			if (snap_percent == DM_PERCENT_INVALID)
				snap_active = 0;
	}

/* FIXME Add -D arg to skip this! */
	if (inkernel && snap_active)
		active_str = "ACTIVE   ";
	else
		active_str = "inactive ";

	if (lv_is_origin(lv))
		snapshot_str = "Original";
	else if (lv_is_cow(lv))
		snapshot_str = "Snapshot";
	else
		snapshot_str = "        ";

	log_print_unless_silent("%s%s '%s%s/%s' [%s] %s", active_str, snapshot_str,
				cmd->dev_dir, lv->vg->name, lv->name,
				display_size(cmd, lv->size),
				get_alloc_string(lv->alloc));

	return ECMD_PROCESSED;
}

int lvscan(struct cmd_context *cmd, int argc, char **argv)
{
	if (argc) {
		log_error("No additional command line arguments allowed");
		return EINVALID_CMD_LINE;
	}

	return process_each_lv(cmd, argc, argv, 0, NULL,
			       &lvscan_single);
}
