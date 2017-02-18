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
io.write(string.format('Palantir %s (%s)\n', _P.VERSION, _VERSION))

-- User modules
local modules = os.getenv('HOME') .. '/.palantir/?.lua'

-- User profile
local profile = os.getenv('HOME') .. '/.palantir.lua'

-- Raw protocol
local raw_recv = _P.net.recv
local raw_send = _P.net.send

-- Input stack
local stack = {}

-- Palantir error handler
-- @param error message
function _P.error(message)
  if message ~= 'Success' then
    io.stderr:write(string.format('Palantir error: %s\n', message))

    if _P.DEBUG then
      io.stderr:write(debug.traceback())
    end
  end
end

-- Palantir event trigger
-- @param event source
-- @param event name
-- @param event param
-- @return callback result
function _P.event(source, event, param)
  return pcall(_P[source .. '_' .. event:lower()], param)
end

-- Palantir input stack
-- @param input source
function _P.input(source)
  local file = io.open(source, 'r')

  if file == nil then
    stack = {source}
  else
    stack = {}
    for line in file:lines() do
      table.insert(stack, line)
    end
    io.close(file)
  end
end

-- Palantir execute script
-- @param chunk to load
-- @return result or error
function _P.load(chunk)
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
function _P.os.execute(command)
  local handle = io.popen(command .. ' 2>&1')
  local result = handle:read('*a')

  handle:close()
  return result
end

-- Palantir receive frame
-- @return command and param
function _P.net.recv()
  local frame = raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- Palantir send frame
-- @param command
-- @param param
-- @return result
function _P.net.send(command, param)
  local frame = command .. ' ' .. (param or '')
  return raw_send(frame)
end

-- Palantir main server loop
-- @param host address
-- @param port number
function _P.net.server(host, port)
  while not xpcall(function() return _P.net.listen(host, port) end, _P.error) do
    _P.os.sleep(1000)
  end

  while true do
    if xpcall(_P.net.accept, _P.error) then

      while true do
        local status, command, param = xpcall(_P.net.recv, _P.error)

        if not status then break end

        if _P.event('server', command, param) then

        elseif command == 'HELO' then
          local line = table.remove(stack, 1) or _P.os.prompt(param)

          if _P.event('server', 'prompt', line) then

          elseif line:lower():match('^cd%s+') then
            _P.net.send('PATH', line:sub(4))

          elseif line:lower():match('^--%s*halt%s*$') then
            _P.net.send('HALT')
            return

          elseif line:lower():match('^--%s*exit%s*$') then
            return

          else
            _P.net.send('EXEC', line)
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

-- Palantir main client loop
-- @param host address
-- @param port number
function _P.net.client(host, port)
  while true do
    if xpcall(function() return _P.net.connect(host, port) end, _P.error) then
      if client_connected then
        _P.net.send('TEXT', client_ready())
      end

      while true do
        _P.net.send('HELO', string.format('%s@%s:%s ', _P.os.env()))

        local command, param = _P.net.recv()

        if _P.event('client', command, param) then

        elseif command == 'PATH' then
          _P.os.env(param)

        elseif command == 'EXEC' then
          _P.net.send('TEXT', _P.load(param))

        elseif command == 'HALT' then
          os.exit()
        end
      end

    else _P.os.sleep(1000) end
  end
end

-- Shell shortcut
-- @param command to execute
-- @return result or error
function shell(command)
  return _P.os.execute(command)
end

-- Load user modules
package.path = package.path .. ';' .. modules

-- Load user profile
pcall(dofile, profile)

-- Load input stack
pcall(_P.input, _P.STACK)

-- Start shell
local main = (_P.MODE and _P.net.server or _P.net.client)

if xpcall(function() return main(_P.HOST, _P.PORT) end, _P.error) then
  io.write('Palantir exit\n')
end