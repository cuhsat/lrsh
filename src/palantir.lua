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
print(string.format('Palantir %s (%s) ', VERSION, _VERSION))

-- global config
local host = HOST
local port = PORT
local idle = 5000

-- pretty protocol
local raw_send = send
local raw_recv = recv

function send(command, data)
  local frame = command .. ' ' .. (data or '')
  return raw_send(frame)
end

function recv()
  local frame = raw_recv()
  return frame:sub(1, 4), frame:sub(6)
end

-- client mode
function client(host, port)

  while true do
    if pcall(connect, host, port) then

      while true do
        send('INIT', string.format('%s@%s:%s: ', info()))

        local command, data = recv()

        -- change directory
        if command == 'PATH' then
          info(data)
        end

        -- execute Lua code
        if command == 'EVAL' then
          local result = assert(load('return ' .. data)())
          send('TEXT', result)
        end

        -- execute by shell
        if command == 'EXEC' then
          local handle = io.popen(data .. ' 2>&1')
          local result = handle:read('*a')
          send('TEXT', result)
          handle:close()
        end

        -- shutdown client
        if command == 'HALT' then
          do return end
        end
      end

    else sleep(idle) end
  end
end

-- server mode
function server(host, port)
  listen(host, port)

  while true do
    if pcall(accept) then

      while true do
        local command, data = recv()

        -- show prompt
        if command == 'INIT' then
          io.write(data)
        
          local line = io.read()

          -- error
          if not line then
            error('broken')
          
          -- change directory
          elseif line:lower():match('^cd%s+') then
            send('PATH', line:sub(4))

          -- evaluate Lua code
          elseif line:lower():match('^/eval') then
            send('EVAL', line:sub(7))
          
          -- shutdown server
          elseif line:lower():match('^/exit') then
            return

          -- shutdown client
          elseif line:lower():match('^/halt') then
            send('HALT')
            break

          -- execute by shell
          else       
            send('EXEC', line)
          end
        end

        -- print text
        if command == 'TEXT' then
          io.write(data)
        end
      end
    end
  end
end

-- call user config
pcall(dofile, os.getenv('HOME') .. '/.palantirrc')

-- exit
function exit(status)
  if not status == 'Success' then
    io.write(string.format('Lua error: %s\n', status))
  end
end

-- main
if xpcall(MODE and client or server, exit, host, port) then
  print('Palantir exit')
end
