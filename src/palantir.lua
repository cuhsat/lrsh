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
io.write(string.format('Palantir %s (%s)\n', palantir.VERSION, _VERSION))

-- User profile
local profile = os.getenv('HOME') .. '/.palantir.lua'

-- Raw protocol
local raw_recv = palantir.net.recv
local raw_send = palantir.net.send

-- Input stack
local stack = {}

-- Palantir error handler
-- @param error message
function palantir.error(message)
  if message ~= 'Success' then
    io.stderr:write(string.format('Palantir error: %s\n', message))
  end
end

-- Palantir event trigger
-- @param event source
-- @param event name
-- @param event param
-- @return callback result
function palantir.event(source, event, param)
  return pcall(_G[source .. '_' .. event:lower()], param)
end

-- Palantir execute script
-- @param chunk to load
-- @return result or error
function palantir.load(chunk)
  local f, err = load(chunk, 'load', 't')

  if f then
    return tostring(f() or '')
  else
    return string.format('Palantir error: %s\n', err)
  end
end

-- Palantir execute shell
-- @param command to execute
-- @return result or error
function palantir.os.execute(command)
  local handle = io.popen(command .. ' 2>&1')
  local result = handle:read('*a')

  handle:close()
  return result
end

-- Palantir receive frame
-- @return command and param
function palantir.net.recv()
  local frame = raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- Palantir send frame
-- @param command
-- @param param
-- @return result
function palantir.net.send(command, param)
  local frame = command .. ' ' .. (param or '')
  return raw_send(frame)
end

-- Palantir main server loop
-- @param host address
-- @param port number
function palantir.net.server(host, port)
  while not xpcall(palantir.net.listen, palantir.error, host, port) do
    palantir.os.sleep(1000)
  end

  while true do
    if xpcall(palantir.net.accept, palantir.error) then

      while true do
        local command, param = palantir.net.recv()

        if palantir.event('server', command, param) then

        elseif command == 'HELO' then
          local line = table.remove(stack, 1)

          if line == nil then
            line = palantir.os.readline(param)
          end

          if palantir.event('server', 'prompt', line) then

          elseif line:lower():match('^cd%s+') then
            palantir.net.send('PATH', line:sub(4))

          elseif line:lower():match('^--%s*exit$') then
            palantir.net.send('EXIT')
            break

          elseif line:lower():match('^--%s*halt$') then
            return

          else
            palantir.net.send('EXEC', line)
          end

        elseif command == 'TEXT' then
          io.write(param or '\n')
        end
      end
    end
  end
end

-- Palantir main client loop
-- @param host address
-- @param port number
function palantir.net.client(host, port)
  while true do
    if xpcall(palantir.net.connect, palantir.error, host, port) then
      if client_connected then
        palantir.net.send('TEXT', client_connected())
      end

      while true do
        palantir.net.send('HELO', string.format('%s@%s:%s ', palantir.os.env()))

        local command, param = palantir.net.recv()

        if palantir.event('client', command, param) then

        elseif command == 'PATH' then
          palantir.os.env(param)

        elseif command == 'EXEC' then
          palantir.net.send('TEXT', palantir.load(param))

        elseif command == 'EXIT' then
          os.exit()
        end
      end

    else palantir.os.sleep(1000) end
  end
end

-- Shell shortcut
-- @param command to execute
-- @return result or error
function shell(command)
  return palantir.os.execute(command)
end

-- Load user profile
pcall(dofile, profile)

-- Load input stack
if palantir.STACK then
  local file = io.open(palantir.STACK, 'r')

  if file == nil then
    stack = {palantir.STACK}
  else
    for line in file:lines() do
      table.insert(stack, line)
    end
    io.close(file)
  end
end

-- Start shell
local main = (palantir.MODE and palantir.net.server or palantir.net.client)

if xpcall(main, palantir.error, palantir.HOST, palantir.PORT) then
  io.write('Palantir exit\n')
end
