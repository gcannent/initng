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

#include <signal.h>

#include <initng.h>

void initng_signal_dispatch(void)
{
	for (int i = 0; i < SIGNAL_STACK; i++) {
		if (g.signals_got[i] == -1)
			continue;

		/* reset the signal slot */
		int sig = g.signals_got[i];
		g.signals_got[i] = -1;

		initng_module_callers_signal(sig);

		switch (sig) {
		/* dead children */
		case SIGCHLD:
			initng_signal_handle_sigchild();
			break;
		case SIGALRM:
			initng_handler_run_alarm();
			break;
		default:
			break;
		}
	}
}
