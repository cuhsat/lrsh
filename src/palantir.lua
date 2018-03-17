-- Copyright (c) 2016-2018 Christian Uhsat <christian@uhsat.de>
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
io.write(string.format('%s (%s with %s)\n', VERSION, BUILD, _VERSION))

-- Home directory
HOME = os.getenv('HOME') or os.getenv('USERPROFILE') or '.'

-- User profile
local profile = HOME .. '/.profile.lua'

-- Raw protocol
local raw_recv = net.recv
local raw_send = net.send

-- Lua 5.x compatibility
if not loadstring then
  function loadstring(chunk, chunkname)
    return load(chunk, chunkname, 't')
  end
end

-- Local error handler
-- @param error message
local function error(message)
  if message ~= 'Success' and message ~= 'No error' then
    io.stderr:write(string.format('Palantir error: %s\n', message))

    if DEBUG then
      io.stderr:write(debug.traceback())
    end
  end
end

-- Local callback trigger
-- @param event source
-- @param event name
-- @param event param
-- @return callback result
local function trigger(source, event, param)
  local status, result = pcall(_G[source .. '_' .. event:lower()], param)

  if status then
    return result
  end

  return false
end

-- Local execute chunk
-- @param chunk to load
-- @return result or error
local function execute(chunk)
  local command, result = loadstring(chunk, 'prompt')

  if command then
    return tostring(command() or '')
  end

  return string.format('Palantir error: %s\n', result)
end

-- OS execute shell command
-- @param command to execute
-- @return result or error
function os.shell(command)
  local handle = io.popen(command .. ' 2>&1')
  local result = handle:read('*a')
  handle:close()
  return result
end

-- Net receive frame
-- @return command and param
function net.recv()
  local frame = raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- Net send frame
-- @param command
-- @param param
-- @return result
function net.send(command, param)
  local frame = command .. ' ' .. (param or '')
  return raw_send(frame)
end

-- Net client main loop
-- @param host address
-- @param port number
function net.client(host, port)
  while not xpcall(function() return net.listen(host, port) end, error) do
    os.sleep(1000)
  end

  while true do
    if xpcall(net.accept, error) then

      while true do
        local status, command, param = xpcall(net.recv, error)

        if not status then break end

        if trigger('client', command, param) then

        -- HELO command
        elseif command == 'HELO' then
          local line = os.prompt(param)

          if trigger('client', 'prompt', line) then

          elseif line:lower():match('^cd%s+') then
            net.send('PATH', line:sub(4))

          elseif line:lower():match('^--%s*halt%s*$') then
            net.send('HALT')
            return

          elseif line:lower():match('^--%s*exit%s*$') then
            return

          else
            net.send('EXEC', line)
          end

        -- TEXT command
        elseif command == 'TEXT' then
          if param:sub(-1) ~= '\n' then
            param = param .. '\n'
          end

          if param:match('%S+') then
            io.write(param)
          end
        end
      end
    end
  end
end

-- Net server main loop
-- @param host address
-- @param port number
function net.server(host, port)
  while true do
    if xpcall(function() return net.connect(host, port) end, error) then

      while true do
        net.send('HELO', string.format('%s@%s:%s ', os.path()))

        local command, param = net.recv()

        if trigger('server', command, param) then

        -- PATH command
        elseif command == 'PATH' then
          os.path(param)

        -- EXEC command
        elseif command == 'EXEC' then
          net.send('TEXT', execute(param))

        -- HALT command
        elseif command == 'HALT' then
          os.exit()
        end
      end

    else os.sleep(1000) end
  end
end

-- Load user profile
pcall(dofile, profile)

-- Run shell
while not xpcall(function()
  local main = SERVER and net.server or net.client
  return main(HOST, PORT)
end, error) do end

-- Exit
io.write('Palantir exit\n')
