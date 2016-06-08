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
io.write(string.format('Palantir %s (%s)\n', VERSION, _VERSION))

-- config
TIMEOUT = 5000

-- pretty protocol
local raw_recv = recv
local raw_send = send

-- recv command
function recv()
  local frame = raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- send command
function send(command, param)
  local frame = command .. ' ' .. (param or '')
  return raw_send(frame)
end

-- execute shell command
function exec(param)
  local handle = io.popen(param .. ' 2>&1')
  local result = handle:read('*a')

  handle:close()
  return result
end

-- execute Lua command
function eval(param)
  local lua, err = load('return ' .. param, 'eval', 't')

  if lua then
    local result = tostring(lua() or '')

    -- force line break
    if result and result:sub(-1) ~= '\n' then
      result = result .. '\n'
    end

    return result
  else
    return string.format('Palantir error: %s\n', err)
  end
end

-- error handler
function fail(err)
  if err ~= 'Success' then
    io.stderr:write(string.format('Palantir error: %s\n', err))
  end
end

-- client mode
function client(host, port)

  while true do
    if xpcall(connect, fail, host, port) then

      while true do
        send('INIT', string.format('%s@%s:%s ', info()))

        local command, param = recv()

        -- change directory
        if command == 'PATH' then
          info(param)
        end

        -- execute code
        if command == 'EXEC' then
          send('TEXT', eval(param))
        end

        -- shutdown client
        if command == 'HALT' then
          os.exit()
        end
      end

    else sleep(TIMEOUT) end
  end
end

-- server mode
function server(host, port)
  while not xpcall(listen, fail, host, port) do
    sleep(TIMEOUT)
  end

  while true do
    if xpcall(accept, fail) then

      while true do
        local command, param = recv()

        -- show prompt
        if command == 'INIT' then
          io.write(param)

          local line = io.read()

          -- error
          if not line then
            error('Input invalid')
          
          -- change directory
          elseif line:lower():match('^cd%s+') then
            send('PATH', line:sub(4))

          -- shutdown server
          elseif line:lower():match('^--%s*exit$') then
            return

          -- shutdown client
          elseif line:lower():match('^--%s*halt$') then
            send('HALT')
            break

          -- execute code
          else       
            send('EXEC', line)
          end
        end

        -- print text
        if command == 'TEXT' then
          io.write(param)
        end
      end
    end
  end
end

-- call user config
pcall(dofile, os.getenv('HOME') .. '/.palantirrc')

-- main
if xpcall(MODE and server or client, fail, HOST, PORT) then
  io.write('Palantir exit\n')
end
