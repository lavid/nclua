--[[ nclua.event.ncl -- The NCL event class.
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
along with NCLua.  If not, see <https://www.gnu.org/licenses/>.  ]]--

local zip = {}

local assert = assert
local tests = require ('tests')
local check = require ('nclua.event.check')
local engine = require ('nclua.event.engine')
local zip_impl = require ('nclua.event.zip_impl')


local errMsg = "erro ao abrir zip"

do
   zip = engine:new ()
   zip.class = 'zip'
end

-- List of supported zip event types.
local type_list = {'open', 'extract', 'save', 'cancel'}

---
-- Checks if event EVT is a valid zip event.
-- Returns EVT is successful, otherwise throws an error.
--
function zip:check (evt)
   assert (evt.class == zip.class)
   check.event.option ('type', evt.type, type_list)
   return evt
end

---
-- Builds a zip event filter according to the given parameters.
-- Returns a new filter if successful, otherwise throws an error.
--
function zip:filter (class, type)
   assert (class == zip.class)
   if type ~= nil then
      check.arg.option ('type', type, type_list)
   end
   local filter = {class=zip.class, type=type}
   return filter
end


---
-- Cycles the zip engine once.
--

function zip:cycle ()
  
  while not zip.INQ:is_empty () do
  
    local evt = zip.INQ:dequeue ()
    assert (evt.class == zip.class)
  
    if evt.type == 'open' then
  
      local status, errmsg = pcall (zip_impl.open,
                                    evt.path)

      tests.dump('\n\n\n\n')
      tests.dump(status)
  
      if status == false then
  
        evt.class = evt.class
        evt.type  = evt.type
        evt.path  = evt.path
        evt.error = 'nao abriu'
        evt.zip   = nil
        --TODO: fulfill event
        zip.OUTQ:enqueue (evt)

      else

        evt.class = evt.class
        evt.type  = evt.type
        evt.path  = evt.path
        evt.error = nil
        --evt.zip   = zip_impl
        --TODO: fulfill event
        zip.OUTQ:enqueue (evt)

      end
    end
  end

  zip_impl:cycle ()

end

return zip
