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
local TRACE = tests.trace
local TRACE_SEP = tests.trace_sep


local x=20
local b=43
local disp = 5
ASSERT(bit32.bnot(x) == (-1 - x) % 2^32)
ASSERT(bit32.lrotate(x, disp) == bit32.lrotate(x, disp % 32))
ASSERT(bit32.lshift(b, disp) == (b * 2^disp) % 2^32)
ASSERT(bit32.rrotate(x, disp) == bit32.rrotate(x, disp % 32))
ASSERT(bit32.rshift(b, disp) == math.floor(b % 2^32 / 2^disp))

-- bit32.arshift (x, disp)

ASSERT(bit32.arshift(0x12345678, 0) == 0x12345678)
ASSERT(bit32.arshift(0x12345678, 1) == 0x12345678 / 2)
ASSERT(bit32.arshift(0x12345678, -1) == 0x12345678 * 2)
ASSERT(bit32.arshift(-1, 1) == 0xffffffff)
ASSERT(bit32.arshift(-1, 24) == 0xffffffff)
ASSERT(bit32.arshift(-1, 32) == 0xffffffff)
ASSERT(bit32.arshift(-1, -1) == bit32.band(-1 * 2, 0xffffffff))

-- bit32.band (···)

ASSERT(bit32.band(-1) == 0xffffffff)
--ASSERT(bit32.band((bit32.arshift(1, -33) - 1) == 0xffffffff)
--   TRACE('arshift', (bit32.arshift(1, -33) - 1))
-- ASSERT(bit32.band(-(1 << 33) - 1) == 0xffffffff)
-- ASSERT(bit32.band((1 << 33) + 1) == 1)
-- ASSERT(bit32.band(-(1 << 33) + 1) == 1)
-- ASSERT(bit32.band(-(1 << 40)) == 0)
-- ASSERT(bit32.band(1 << 40) == 0)
-- ASSERT(bit32.band(-(1 << 40) - 2) == 0xfffffffe)
-- ASSERT(bit32.band((1 << 40) - 4) == 0xfffffffc)

--
--bit32.bnot()
ASSERT(bit32.bnot(x) == (-1 - x) % 2^32)


--bit32.bor()

--if 2.0^50 < 2.0^50 + 1.0 and 2.0^50 < (-1 >> 1) then
  ASSERT(bit32.bor(2.0^32 - 5.0) == 0xfffffffb)
  ASSERT(bit32.bor(-2.0^32 - 6.0) == 0xfffffffa)
  ASSERT(bit32.bor(2.0^48 - 5.0) == 0xfffffffb)
  ASSERT(bit32.bor(-2.0^48 - 6.0) == 0xfffffffa)
--end

local c = {0, 1, 2, 3, 10, 0x80000000, 0xaaaaaaaa, 0x55555555,
0xffffffff, 0x7fffffff}

for _, b in pairs(c) do
  ASSERT(bit32.band(b) == b)
  ASSERT(bit32.band(b, b) == b)
  ASSERT(bit32.band(b, b, b, b) == b)
  ASSERT(bit32.btest(b, b) == (b ~= 0))
  ASSERT(bit32.band(b, b, b) == b)
  --ASSERT(bit32.band(b, b, b, ~b) == 0)
  ASSERT(bit32.btest(b, b, b) == (b ~= 0))
  ASSERT(bit32.band(b, bit32.bnot(b)) == 0)
  ASSERT(bit32.bor(b, bit32.bnot(b)) == bit32.bnot(0))
  ASSERT(bit32.bor(b) == b)
  ASSERT(bit32.bor(b, b) == b)
  ASSERT(bit32.bor(b, b, b) == b)
  --ASSERT(bit32.bor(b, b, 0, ~b) == 0xffffffff)
  ASSERT(bit32.bxor(b) == b)
  ASSERT(bit32.bxor(b, b) == 0)
  ASSERT(bit32.bxor(b, b, b) == b)
  ASSERT(bit32.bxor(b, b, b, b) == 0)
  ASSERT(bit32.bxor(b, 0) == b)
  ASSERT(bit32.bnot(b) ~= b)
  ASSERT(bit32.bnot(bit32.bnot(b)) == b)
 -- ASSERT(bit32.bnot(b) == (1 << 32) - 1 - b)
  ASSERT(bit32.lrotate(b, 32) == b)
  ASSERT(bit32.rrotate(b, 32) == b)
  ASSERT(bit32.lshift(bit32.lshift(b, -4), 4) == bit32.band(b, bit32.bnot(0xf)))
  ASSERT(bit32.rshift(bit32.rshift(b, 4), -4) == bit32.band(b, bit32.bnot(0xf)))
end

ASSERT(bit32.extract(0x12345678, 0, 4) == 8)
ASSERT(bit32.extract(0x12345678, 4, 4) == 7)
ASSERT(bit32.extract(0xa0001111, 28, 4) == 0xa)
ASSERT(bit32.extract(0xa0001111, 31, 1) == 1)
ASSERT(bit32.extract(0x50000111, 31, 1) == 0)
ASSERT(bit32.extract(0xf2345679, 0, 32) == 0xf2345679)

ASSERT(not pcall(bit32.extract, 0, -1))
ASSERT(not pcall(bit32.extract, 0, 32))
ASSERT(not pcall(bit32.extract, 0, 0, 33))
ASSERT(not pcall(bit32.extract, 0, 31, 2))



ASSERT(bit32.band() == bit32.bnot(0))
ASSERT(bit32.btest() == true)
ASSERT(bit32.bor() == 0)
ASSERT(bit32.bxor() == 0)

ASSERT(bit32.band() == bit32.band(0xffffffff))
ASSERT(bit32.band(1,2) == 0)


ASSERT(bit32.lrotate(0, -1) == 0)
ASSERT(bit32.lrotate(0, 7) == 0)
ASSERT(bit32.lrotate(0x12345678, 0) == 0x12345678)
ASSERT(bit32.lrotate(0x12345678, 32) == 0x12345678)
ASSERT(bit32.lrotate(0x12345678, 4) == 0x23456781)
ASSERT(bit32.rrotate(0x12345678, -4) == 0x23456781)
ASSERT(bit32.lrotate(0x12345678, -8) == 0x78123456)
ASSERT(bit32.rrotate(0x12345678, 8) == 0x78123456)
ASSERT(bit32.lrotate(0xaaaaaaaa, 2) == 0xaaaaaaaa)
ASSERT(bit32.lrotate(0xaaaaaaaa, -2) == 0xaaaaaaaa)
for i = -50, 50 do
  ASSERT(bit32.lrotate(0x89abcdef, i) == bit32.lrotate(0x89abcdef, i%32))
end

ASSERT(bit32.lshift(0x12345678, 4) == 0x23456780)
ASSERT(bit32.lshift(0x12345678, 8) == 0x34567800)
ASSERT(bit32.lshift(0x12345678, -4) == 0x01234567)
ASSERT(bit32.lshift(0x12345678, -8) == 0x00123456)
ASSERT(bit32.lshift(0x12345678, 32) == 0)
ASSERT(bit32.lshift(0x12345678, -32) == 0)
ASSERT(bit32.rshift(0x12345678, 4) == 0x01234567)
ASSERT(bit32.rshift(0x12345678, 8) == 0x00123456)
ASSERT(bit32.rshift(0x12345678, 32) == 0)
ASSERT(bit32.rshift(0x12345678, -32) == 0)
ASSERT(bit32.arshift(0x12345678, 0) == 0x12345678)
ASSERT(bit32.arshift(0x12345678, 1) == 0x12345678 / 2)
ASSERT(bit32.arshift(0x12345678, -1) == 0x12345678 * 2)
ASSERT(bit32.arshift(-1, 1) == 0xffffffff)
ASSERT(bit32.arshift(-1, 24) == 0xffffffff)
ASSERT(bit32.arshift(-1, 32) == 0xffffffff)
ASSERT(bit32.arshift(-1, -1) == bit32.band(-1 * 2, 0xffffffff))


-- some special cases
local c = {0, 1, 2, 3, 10, 0x80000000, 0xaaaaaaaa, 0x55555555,
           0xffffffff, 0x7fffffff}

for _, b in pairs(c) do
  ASSERT(bit32.band(b) == b)
  ASSERT(bit32.band(b, b) == b)
  ASSERT(bit32.band(b, b, b, b) == b)
  ASSERT(bit32.btest(b, b) == (b ~= 0))
  ASSERT(bit32.band(b, b, b) == b)
  --ASSERT(bit32.band(b, b, b, ~b) == 0)
  ASSERT(bit32.btest(b, b, b) == (b ~= 0))
  ASSERT(bit32.band(b, bit32.bnot(b)) == 0)
  ASSERT(bit32.bor(b, bit32.bnot(b)) == bit32.bnot(0))
  ASSERT(bit32.bor(b) == b)
  ASSERT(bit32.bor(b, b) == b)
  ASSERT(bit32.bor(b, b, b) == b)
  -- ASSERT(bit32.bor(b, b, 0, ~b) == 0xffffffff)
  ASSERT(bit32.bxor(b) == b)
  ASSERT(bit32.bxor(b, b) == 0)
  ASSERT(bit32.bxor(b, b, b) == b)
  ASSERT(bit32.bxor(b, b, b, b) == 0)
  ASSERT(bit32.bxor(b, 0) == b)
  ASSERT(bit32.bnot(b) ~= b)
  ASSERT(bit32.bnot(bit32.bnot(b)) == b)
  -- ASSERT(bit32.bnot(b) == (1 << 32) - 1 - b)
  ASSERT(bit32.lrotate(b, 32) == b)
  ASSERT(bit32.rrotate(b, 32) == b)
  ASSERT(bit32.lshift(bit32.lshift(b, -4), 4) == bit32.band(b, bit32.bnot(0xf)))
  ASSERT(bit32.rshift(bit32.rshift(b, 4), -4) == bit32.band(b, bit32.bnot(0xf)))
end

-- for this test, use at most 24 bits (mantissa of a single float)
c = {0, 1, 2, 3, 10, 0x800000, 0xaaaaaa, 0x555555, 0xffffff, 0x7fffff}
for _, b in pairs(c) do
  for i = -40, 40 do
    local x = bit32.lshift(b, i)
    local y = math.floor(math.fmod(b * 2.0^i, 2.0^32))
    ASSERT(math.fmod(x - y, 2.0^32) == 0)
  end
end

ASSERT(not pcall(bit32.band, {}))
ASSERT(not pcall(bit32.bnot, "a"))
ASSERT(not pcall(bit32.lshift, 45))
ASSERT(not pcall(bit32.lshift, 45, print))
ASSERT(not pcall(bit32.rshift, 45, print))

-- testing extract/replace

ASSERT(bit32.extract(0x12345678, 0, 4) == 8)
ASSERT(bit32.extract(0x12345678, 4, 4) == 7)
ASSERT(bit32.extract(0xa0001111, 28, 4) == 0xa)
ASSERT(bit32.extract(0xa0001111, 31, 1) == 1)
ASSERT(bit32.extract(0x50000111, 31, 1) == 0)
ASSERT(bit32.extract(0xf2345679, 0, 32) == 0xf2345679)

ASSERT(not pcall(bit32.extract, 0, -1))
ASSERT(not pcall(bit32.extract, 0, 32))
ASSERT(not pcall(bit32.extract, 0, 0, 33))
ASSERT(not pcall(bit32.extract, 0, 31, 2))

ASSERT(bit32.replace(0x12345678, 5, 28, 4) == 0x52345678)
ASSERT(bit32.replace(0x12345678, 0x87654321, 0, 32) == 0x87654321)
ASSERT(bit32.replace(0, 1, 2) == 2^2)
ASSERT(bit32.replace(0, -1, 4) == 2^4)
-- ASSERT(bit32.replace(-1, 0, 31) == (1 << 31) - 1)
-- ASSERT(bit32.replace(-1, 0, 1, 2) == (1 << 32) - 7)


-- testing conversion of floats

ASSERT(bit32.bor(3.0) == 3)
ASSERT(bit32.bor(-4.0) == 0xfffffffc)

-- large floats and large-enough integers?
-- if 2.0^50 < 2.0^50 + 1.0 and 2.0^50 < (-1 >> 1) then
  ASSERT(bit32.bor(2.0^32 - 5.0) == 0xfffffffb)
  ASSERT(bit32.bor(-2.0^32 - 6.0) == 0xfffffffa)
  ASSERT(bit32.bor(2.0^48 - 5.0) == 0xfffffffb)
  ASSERT(bit32.bor(-2.0^48 - 6.0) == 0xfffffffa)
-- end

