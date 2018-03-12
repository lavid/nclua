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
local ASSERT_ERROR = tests.ASSERT_ERROR
local ASSERT_CHECK_OBJECT = tests.ASSERT_CHECK_OBJECT

local canvas = require ('nclua.canvas')
_ENV = nil

-- Sanity checks.
ASSERT_ERROR (canvas.new)
ASSERT_ERROR (canvas.new, {})
ASSERT_ERROR (canvas.new, 0, 'abc')
ASSERT_ERROR (canvas.new, {}, 0)
ASSERT_ERROR (canvas.new, 0, true)

-- Check invalid path.
ASSERT (canvas.new ('nonexistent') == nil)

-- Check invalid dimensions.
ASSERT (canvas.new (-1, 50) == nil)
ASSERT (canvas.new (1, -50) == nil)

-- Check the returned canvas object type.
local c = ASSERT (canvas.new (1, 1))
ASSERT_CHECK_OBJECT (c, 'nclua.canvas')
local c = ASSERT (canvas.new (tests.canvas.get_sample ('apple-red')))
ASSERT_CHECK_OBJECT (c, 'nclua.canvas')
ASSERT (canvas.dump(c,'test-canvas-dump1'))
ASSERT (canvas.dump(c,'test-canvas-dump-q1', 'jpeg',1))
ASSERT (canvas.dump(c,'test-canvas-dump-q45', 'jpeg',45))
ASSERT (canvas.dump(c,'test-canvas-dump-q100', 'jpeg',100))
ASSERT (c:dump('test-canvas-dump2','jpeg'))
local status, erromsg=c:dump('test-canvas-dump3','gif')
ASSERT(status==false, erromsg=='format invalid')

