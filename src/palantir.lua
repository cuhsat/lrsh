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
io.write(string.format('Palantir %s (%s)\n', palantir.version, _VERSION))

-- palantir execute
function palantir.execute(chunk)
  local handle = io.popen(chunk .. ' 2>&1')
  local result = handle:read('*a')

  handle:close()
  return result
end

-- palantir load
function palantir.load(chunk)
  local fn, err = load(chunk, 'load', 't')

  if fn then
    return tostring(fn() or '')
  else
    return string.format('Palantir error: %s\n', err)
  end
end

-- palantir recv
function palantir.recv()
  local frame = palantir.raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- palantir send
function palantir.send(command, param)
  local frame = command .. ' ' .. (param or '')
  return palantir.raw_send(frame)
end

-- event handler
local function hook(source, event, arg)
  return pcall(_G[source .. '_' .. event:lower()], arg)
end

-- error handler
local function fail(err)
  io.stderr:write(string.format('Palantir error: %s\n', err))
end

-- client main loop
local function client(host, port)

  while true do
    if xpcall(palantir.connect, fail, host, port) then

      while true do
        palantir.send('INIT', string.format('%s@%s:%s ', palantir.system()))

        local command, param = palantir.recv()

        if hook('client', command, param) then

        elseif command == 'PATH' then
          palantir.system(param)

        elseif command == 'EXEC' then
          palantir.send('TEXT', palantir.load(param))

        elseif command == 'HALT' then
          os.exit()
        end
      end

    else palantir.sleep(palantir.timeout) end
  end
end

-- server main loop
local function server(host, port)
  while not xpcall(palantir.listen, fail, host, port) do
    palantir.sleep(palantir.timeout)
  end

  while true do
    if xpcall(palantir.accept, fail) then

      while true do

        local command, param = palantir.recv()

        if hook('server', command, param) then

        elseif command == 'INIT' then
          io.write(param)

          local line = io.read()

          if hook('server', 'input', line) then
          
          elseif line:lower():match('^cd%s+') then
            palantir.send('PATH', line:sub(4))

          elseif line:lower():match('^--%s*exit$') then
            return

          elseif line:lower():match('^--%s*halt$') then
            palantir.send('HALT')
            break

          else
            palantir.send('EXEC', line)
          end

        elseif command == 'TEXT' then
          io.write(param or '\n')
        end
      end
    end
  end
end

-- user config
pcall(dofile, os.getenv('HOME') .. '/.palantirrc')

-- main
local main = (palantir.mode and server or client)

if xpcall(main, fail, palantir.host, palantir.port) then
  io.write('Palantir exit\n')
end
