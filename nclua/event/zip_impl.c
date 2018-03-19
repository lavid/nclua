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

/* Throws a run-time error.  */
#define error_throw(L, msg)\
  (lua_pushstring ((L), (msg)), lua_error ((L)))


typedef struct {

  struct zip_t *file;


} l_zip_udata;


l_zip_udata zipU;

/*-
 * zip.cycle ()
 *
 * Cycles the zip engine once
 */
static int
l_zip_cycle (unused (lua_State *L))
{

  printf ("l_zip_cycle\n");
  return (g_main_context_iteration (NULL, FALSE), 0);

}

/*-
 * zip:open
 */
static int
l_zip_open (lua_State *L)
{

  l_zip_udata *zipTable;


  const char *path = lua_tostring(L, 1);

  if(path){
    printf("%s\n", path);
  }


  printf ("l_zip_open beggining\n");
    
  zipU.file = zip_open(path, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');

  if(zipU.file ){
    printf("Zip open success\n");
  }
  else{
    printf("Zip open fail. Creating new Zip\n");
   
    zipU.file  = zip_open(path, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

    if(zipU.file ){

      printf("Zip creation success\n");

    }else{

      printf("Zip creation fail\n");
      return error_throw (L, "erro ao abrir zip");

    }
  
  }
  

  /* Create the user data pushing it onto the stack. We also pre-initialize
   * the member of the userdata in case initialization fails in some way. If
   * that happens we want the userdata to be in a consistent state for __gc. */
  
  zipTable        = (l_zip_udata *)lua_newuserdata(L, sizeof(*zipTable));
  zipTable->file  = NULL;
  

  
  /* Add the metatable to the stack. */
  luaL_getmetatable(L, "ZIP_IMPL");
  /* Set the metatable on the userdata. */
  lua_setmetatable(L, -2);
  lua_pushnumber(L, (int)zipU.file);



  //luaL_setmetatable (L, ZIP_IMPL);
  //lua_pushuserdata (file);


  zip_close(zipU.file );

  return 4;
}



static const struct luaL_Reg funcs[] = {
  {"cycle", l_zip_cycle},
  {"open", l_zip_open},
  {NULL, NULL}
};

static const struct luaL_Reg methods[] = {
  {NULL, NULL}
};



int luaopen_nclua_event_zip_impl (lua_State *L);

int
luaopen_nclua_event_zip_impl (lua_State *L)
{
  G_TYPE_INIT_WRAPPER ();
    /* Create the metatable and put it on the stack. */
    luaL_newmetatable(L, "zip");
    /* Duplicate the metatable on the stack (We know have 2). */
    lua_pushvalue(L, -1);
    /* Pop the first metatable off the stack and assign it to __index
     * of the second one. We set the metatable for the table to itself.
     * This is equivalent to the following in lua:
     * metatable = {}
     * metatable.__index = metatable
     */
    lua_setfield(L, -2, "__index");
 
    /* Set the methods to the metatable that should be accessed via object:func */
    luaL_setfuncs(L, funcs, 0);
    
    /*
    Register the object.func functions into the table that is at the top of the
     * stack. */
    //luaL_newlib(L, methods);
    
 
    return 1;
}
