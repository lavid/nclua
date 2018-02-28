--[[ Copyright (C) 2013-2018 PUC-Rio/Laboratorio TeleMidia

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

local tests = require ('tests')
local ASSERT = tests.ASSERT

local zip = require ('nclua.event.zip')

-- Check class-only.
local t = zip:filter ('zip')
ASSERT (tests.objeq (t, {class='zip'}))

-- Check class and type.
local t1 = zip:filter ('zip', 'open')
ASSERT (tests.objeq (t1, {class='zip', type='open'}))

local t2 = zip:filter ('zip', 'extract')
ASSERT (tests.objeq (t2, {class='zip', type='extract'}))

local t3 = zip:filter ('zip', 'save')
ASSERT (tests.objeq (t3, {class='zip', type='save'}))

local t4 = zip:filter ('zip', 'cancel')
ASSERT (tests.objeq (t4, {class='zip', type='cancel'}))