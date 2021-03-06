/*
 * Initng, a next generation sysvinit replacement.
 * Copyright (C) 2006 Jimmy Wennlund <jimmy.wennlund@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <initng.h>

#include <sys/time.h>
#include <time.h>		/* time() */
#include <fcntl.h>		/* fcntl() */
#include <string.h>		/* memmove() strcmp() */
#include <sys/wait.h>		/* waitpid() sa */
#include <sys/ioctl.h>		/* ioctl() */
#include <stdio.h>		/* printf() */
#include <stdlib.h>		/* free() exit() */
#include <assert.h>
#include <errno.h>

void initng_handler_restart_restarting(void)
{
	active_db_h *current = NULL;

	/* make sure there is no more restarting running or stopping */
	while_active_db(current) {
		if (GET_STATE(current) == IS_STOPPING) {
			if (is(&RESTARTING, current))
				return;
		}
	}

	/* OK, there was no stopping restart marked services. */
	/* now restart all RESTARTING services, that are STOPPED */
	current = NULL;
	while_active_db(current) {
		if (GET_STATE(current) == IS_DOWN) {
			if (is(&RESTARTING, current)) {
				/* remove the restarting flag */
				initng_data_remove(&RESTARTING, current);

				/* start the service */
				initng_handler_start_service(current);
			}
		}
	}
}
