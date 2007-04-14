/*
 * Initng, a next generation sysvinit replacement.
 * Copyright (C) 2006 Jimmy Wennlund <jimmy.wennlund@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>							/* fcntl() */

#include "initng.h"
#include "initng_global.h"
#include "initng_load_module.h"
#include "initng_toolbox.h"
#include "initng_signal.h"

#include "initng_plugin_callers.h"
#include "initng_plugin.h"
#include "initng_static_event_types.h"

#include "initng_fd.h"


/*
 * This function delivers what read to plugin,
 * through the hooks.
 * If no hook is found, or no return TRUE, it will
 * be printed to screen anyway.
 */
static void initng_fd_plugin_readpipe(active_db_h * service,
									  process_h * process, pipe_h * pi,
									  char *buffer_pos)
{
	s_event event;
	s_event_buffer_watcher_data data;

	S_;

	event.event_type = &EVENT_BUFFER_WATCHER;
	event.data = &data;
	data.service = service;
	data.process = process;
	data.pipe = pi;
	data.buffer_pos = buffer_pos;

	if (initng_event_send(&event) != TRUE)
	{
		/* make sure someone handled this */
		fprintf(stdout, "%s", buffer_pos);
	}
}

/* if there is data incoming in a pipe, tell the plugins */
static int initng_fd_pipe(active_db_h * service, process_h * process,
						  pipe_h * pi)
{
	s_event event;
	s_event_pipe_watcher_data data;

	data.service = service;
	data.process = process;
	data.pipe = pi;

	event.event_type = &EVENT_PIPE_WATCHER;
	event.data = &data;

	if (initng_event_send(&event) == HANDLED)
		return (TRUE);

	return (FALSE);
}