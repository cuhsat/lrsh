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
io.write(string.format('Palantir %s (%s)\n', VERSION, _VERSION))

-- Home directory
HOME = os.getenv('HOME') or os.getenv('USERPROFILE') or '.'

-- User profile
local profile = HOME .. '/.palantir.lua'

-- Raw protocol
local raw_recv = net.recv
local raw_send = net.send

-- Local error handler
-- @param error message
local function _error(message)
  if message ~= 'Success' and message ~= 'No error' then
    io.stderr:write(string.format('Palantir error: %s\n', message))

    if DEBUG then
      io.stderr:write(debug.traceback())
    end
  end
end

-- Local event trigger
-- @param event source
-- @param event name
-- @param event param
-- @return callback result
local function _event(source, event, param)
  local status, result = pcall(_G[source .. '_' .. event:lower()], param)

  if status then
    return result
  else
    return false
  end
end

-- Local evaluate chunk
-- @param chunk to load
-- @return result or error
local function _eval(chunk)
  local code, result = load(chunk, 'load', 't')

  if code then
    return tostring(code() or '')
  else
    return string.format('Palantir error: %s\n', result)
  end
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
  while not xpcall(function() return net.listen(host, port) end, _error) do
    os.sleep(1000)
  end

  while true do
    if xpcall(net.accept, _error) then

      while true do
        local status, command, param = xpcall(net.recv, _error)

        if not status then break end

        if _event('client', command, param) then

        elseif command == 'HELO' then
          local line = os.prompt(param)

          if _event('client', 'prompt', line) then

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

        elseif command == 'TEXT' then
          if param:sub(-1) ~= '\n' then
            param = param .. '\n'
          end

          io.write(param)
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
    if xpcall(function() return net.connect(host, port) end, _error) then
      if server_ready then
        net.send('TEXT', server_ready())
      end

      while true do
        net.send('HELO', string.format('%s@%s:%s ', os.path()))

        local command, param = net.recv()

        if _event('server', command, param) then

        elseif command == 'PATH' then
          os.path(param)

        elseif command == 'EXEC' then
          net.send('TEXT', _eval(param))

        elseif command == 'HALT' then
          os.exit()
        end
      end

    else os.sleep(1000) end
  end
end

-- Load user profile
pcall(dofile, profile)

-- Start shell
local main = (SERVER and net.server or net.client)

while not xpcall(function() return main(HOST, PORT) end, _error) do end

io.write('Palantir exit\n')