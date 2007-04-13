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

#include "initng.h"

#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <assert.h>
#include <fnmatch.h>
#include <sys/time.h>
#include <time.h>

#include "initng_global.h"
#include "initng_active_db.h"
#include "initng_process_db.h"
#include "initng_toolbox.h"
#include "initng_common.h"
#include "initng_static_data_id.h"
#include "initng_plugin_callers.h"
#include "initng_string_tools.h"
#include "initng_static_states.h"
#include "initng_depend.h"


/*
 * Walk the active db, searching for services that are down, and been so for a minute.
 * It will remove this entry to save memory.
 * CLEAN_DELAY are defined in initng.h
 */
void initng_active_db_clean_down(void)
{
	active_db_h *current = NULL;
	active_db_h *safe = NULL;

	while_active_db_safe(current, safe)
	{
		assert(current->name);
		assert(current->current_state);
		if (g.now.tv_sec > current->time_current_state.tv_sec + CLEAN_DELAY)
		{
			initng_process_db_clear_freed(current);

			/* count stopped services */
			if (!IS_DOWN(current))
				continue;

			initng_active_db_free(current);
		}
	}
}
