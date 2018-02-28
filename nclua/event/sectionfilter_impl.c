/* nclua.event.http_soup -- Non-blocking HTTP opens.
   Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

This file is part of NCLua.

NCLua is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

NCLua is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  */

#include <config.h>
#include <string.h>
#include "aux-glib.h"
#include "aux-lua.h"

#include "callback.h"

/* Registry key for the sectionfilter_impl metatable.  */
#define SECTIONFILTER_IMPL "nclua.event.sectionfilter_impl"

/*-
 * sectionfilter.cycle ()
 *
 * Cycles the sectionfilter engine once
 */
static int
l_sectionfilter_cycle (unused (lua_State *L))
{
  printf ("l_sectionfilter_cycle\n");
  return 0;
}

/*-
 * sectionfilter:open
 */
static int
l_sectionfilter_open (lua_State *L)
{
  printf ("l_sectionfilter_open\n");
  return 0;
}


static const struct luaL_Reg funcs[] = {
  {"cycle", l_sectionfilter_cycle},
  {"open", l_sectionfilter_open},
  {NULL, NULL}
};

int luaopen_nclua_event_sectionfilter_impl (lua_State *L);

int
luaopen_nclua_event_sectionfilter_impl (lua_State *L)
{
  G_TYPE_INIT_WRAPPER ();
  luax_newmetatable (L, SECTIONFILTER_IMPL);
  luaL_setfuncs (L, funcs, 0);
  return 1;
}
