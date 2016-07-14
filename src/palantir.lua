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

-- event handler
local function trigger(source, event, param)
  return pcall(_G[source .. '_' .. event:lower()], param)
end

-- client main loop
local function client(host, port)

  while true do
    if xpcall(palantir.connect, palantir.handler, host, port) then

      while true do
        palantir.send('HELO', string.format('%s@%s:%s ', palantir.info()))

        local command, param = palantir.recv()

        if trigger('client', command, param) then

        elseif command == 'PATH' then
          palantir.info(param)

        elseif command == 'EXEC' then
          palantir.send('TEXT', palantir.load(param))

        elseif command == 'EXIT' then
          os.exit()
        end
      end

    else palantir.sleep(palantir.timeout) end
  end
end

-- server main loop
local function server(host, port)
  while not xpcall(palantir.listen, palantir.handler, host, port) do
    palantir.sleep(palantir.timeout)
  end

  while true do
    if xpcall(palantir.accept, palantir.handler) then

      while true do

        local command, param = palantir.recv()

        if trigger('server', command, param) then

        elseif command == 'HELO' then
          local line = palantir.prompt(param)

          if trigger('server', 'input', line) then
          
          elseif line:lower():match('^cd%s+') then
            palantir.send('PATH', line:sub(4))

          elseif line:lower():match('^--%s*exit$') then
            palantir.send('EXIT')
            break

          elseif line:lower():match('^--%s*halt$') then
            return

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

if xpcall(main, palantir.handler, palantir.host, palantir.port) then
  io.write('Palantir exit\n')
end
