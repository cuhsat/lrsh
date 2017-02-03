#!/usr/bin/env lua
local HEADER = "unsigned char %s[] = {\n"
local FOOTER = "};\n"
local LENGTH = "unsigned int %s_len = %s;\n"

-- Print usage
if #arg < 1 then
  print(string.format("Usage: %s FILE", arg[0]))
  return 0
end

-- Normalize filename
local filename = arg[1]:lower():gsub("%W", "_")

-- Read file data
local file = assert(io.open(arg[1], "r"))
local data = file:read("*all")
file:close()

-- Generate "xxd -i" compatible output
io.write(HEADER:format(filename))

for v = 1, #data do
  local b = string.format("0x%02x", data:byte(v))

  -- Indent block
  if v % 12 == 1 then
    b = "  " .. b
  end

  -- Ignore comma for last byte
  if v < #data then
    b = b .. ", "
  end

  -- Add line break to line end
  if (v % 12 == 0) or (v == #data) then
    b = b .. "\n"
  end

  io.write(b)
end

io.write(FOOTER)
io.write(LENGTH:format(filename, #data))