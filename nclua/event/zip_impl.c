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

#include "zip.h"

#include "callback.h"

/* Registry key for the zip_impl metatable.  */
/* Test */
#define ZIP_IMPL "nclua.event.zip_impl"



struct zip_t *file;
 









/*-
 * zip.cycle ()
 *
 * Cycles the zip engine once
 */
static int
l_zip_cycle (unused (lua_State *L))
{

  printf ("l_zip_cycle\n");
  return 0;
}

/*-
 * zip:open
 */
static int
l_zip_open (lua_State *L)
{

  //assert(luaL_checkudata (L, index, ZIP), "not a zip event"); ------------- VERIFICAR!

  char *path = lua_tostring(L, -1);

  if(path){
    printf("%s\n", path);
  }


  printf ("l_zip_open\n");
  return 0;
}

static const struct luaL_Reg funcs[] = {
  {"cycle", l_zip_cycle},
  {"open", l_zip_open},
  {NULL, NULL}
};

int luaopen_nclua_event_zip_impl (lua_State *L);

int
luaopen_nclua_event_zip_impl (lua_State *L)
{
  G_TYPE_INIT_WRAPPER ();
  luax_newmetatable (L, ZIP_IMPL);
  luaL_setfuncs (L, funcs, 0);
  return 1;
}
