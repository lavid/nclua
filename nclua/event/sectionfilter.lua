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

local sectionfilter = {}

local assert = assert
local check = require ('nclua.event.check')
local engine = require ('nclua.event.engine')
local sectionfilter_impl = require ('nclua.event.sectionfilter_impl')

do
  sectionfilter = engine:new ()
  sectionfilter.class = 'sectionfilter'
end

-- List of supported sectionfilter event actions.
local action_list = {'start', 'stop'}

---
-- Checks if event EVT is a valid sectionfilter event.
-- Returns EVT is successful, otherwise throws an error.
--
function sectionfilter:check (evt)
   assert (evt.class == sectionfilter.class)
   check.event.option ('action', evt.action, action_list)
   return evt
end

---
-- Builds a sectionfilter event filter according to the given parameters.
-- Returns a new filter if successful, otherwise throws an error.
--
function sectionfilter:filter (class, action)
   assert (class == sectionfilter.class)
   if action ~= nil then
      check.arg.option ('action', action, action_list)
   end
   local filter = {class=sectionfilter.class, action=action}
   return filter
end


---
-- Cycles the sectionfilter engine once.
--

function sectionfilter:cycle ()
  --  while not sectionfilter.INQ:is_empty () do
  --     local evt = sectionfilter.INQ:dequeue ()
  --     assert (evt.class == sectionfilter.class)
  -- --     if evt.type == 'open' then
  --        local status, errmsg = pcall (sectionfilter_impl.open)
  --        if status == false then
  --           evt.class = sectionfilter.class
  --           -- TODO: fulfill event
  --           sectionfilter.OUTQ:enqueue (evt)
  --        end
  --     end
  --  end
   sectionfilter_impl.cycle ()
end

return sectionfilter
