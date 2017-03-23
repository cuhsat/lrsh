#!/usr/bin/env lua
-- Copyright (c) 2016-2017 Christian Uhsat <christian@uhsat.de>
-- Permission is hereby granted, free of charge, to any person obtaining a
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation
-- the rights to use, copy, modify, merge, publish, distribute, sublicense,
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
-- THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
-- DEALINGS IN THE SOFTWARE.
local HEADER = 'unsigned char %s[] = {\n'
local FOOTER = '};\n'
local LENGTH = 'unsigned int %s_len = %s;\n'

-- Print usage
if #arg < 1 then
  print(string.format('Usage: %s FILE', arg[0]))
  return 0
end

-- Normalize filename
local filename = arg[1]:lower():match('^.+/(.+)$'):gsub('%W', '_')

-- Read file data
local file = assert(io.open(arg[1], 'rb'), 'File not found')
local data = file:read('*all')
file:close()

-- Generate "xxd -i" compatible output
io.write(HEADER:format(filename))

for v = 1, #data do
  local b = string.format('0x%02x', data:byte(v))

  -- Indent block
  if v % 12 == 1 then
    b = '  ' .. b
  end

  -- Ignore comma for last byte
  if v < #data then
    b = b .. ', '
  end

  -- Add line break to line end
  if (v % 12 == 0) or (v == #data) then
    b = b .. '\n'
  end

  io.write(b)
end

io.write(FOOTER)
io.write(LENGTH:format(filename, #data))