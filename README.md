![Palantir](doc/palantir.png)

Palantir is a Lua scriptable, portable, tiny reverse shell, using a human
readable protocol written in C and Lua.

# Build ![Build](https://img.shields.io/travis/cuhsat/palantir.svg)
```
$ mkdir build && cd build
$ cmake .. [-DDEBUG=ON]
$ make [VERBOSE=1]
```

Required:
* [Lua 5.1](https://www.lua.org)

Supported:
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

# Usage
```
$ palantir [-dhlv] [-a TOKEN] [-c COMMAND] [-f FILE] HOST PORT
```

## Options
* `d` Starts in passive mode _(listen)_
* `h` Shows the usage information
* `l` Shows the license
* `v` Shows the version
* `a` Authentication token
* `c` Executes the command
* `f` Executes the file

## Commands
* `-- exit` Shutdown server
* `-- halt` Shutdown client

All input will be evaluated and execute as Lua commands. The internal function
`shell` will execute system commands by using the users default shell and
return the results where `strerr` will be mapped to `stdout`.

## Keyboard
* <kbd>Ctrl</kbd>+<kbd>n</kbd> inserts a new line
* <kbd>Tab</kbd> autocompletes keywords, functions, globals and commands

> Only available if compiled with `readline` support.

# Environment
A new global table named `_P` will be defined which contains all shell
specific functions and properties.

## Modules
Extension [modules](https://www.github.com/cuhsat/palantir-modules) can be
placed under `~/.palantir/`.

## Profile
An user specific profile can be placed under `~/.palantir.lua`.

Here is an example profile:
```
-- greet client
function _P.client_ready()
  return 'Hello\n'
end

-- debug
print('Profile loaded')
```

## Stack
The input stack can be specified by the command line options `f` and `c`. The
option `f` has precedence over the option `c`. If an input stack is provided,
the commands will be processed line by line. The shell will not exit
automatically after all commands are processed.

## Constants
* `MODE`    The command line option `d`
* `TOKEN`   The command line argument `a`
* `STACK`   The command line argument `c` or `f`
* `HOST`    The command line argument `HOST`
* `PORT`    The command line argument `PORT`
* `DEBUG`   The debug flag if compiled in debug mode
* `BUILD`   The build system (only if debug flag is set)
* `VERSION` The semantic version number

## Functions
Mainly for internal use.

### Common
* `error(message)`
* `event(source, event, param)`
* `input(source)`
* `load(chunk)`

### Network
* `net.server(host, port)`
* `net.client(host, port)`
* `net.connect(host, port)`
* `net.listen(host, port)`
* `net.accept()`
* `net.recv()`
* `net.send(command, param)`

### Operating System
* `os.env(path)`
* `os.execute(command)`
* `os.prompt(prompt)`
* `os.sleep(milliseconds)`

### Alias
The alias `shell` is provided as an shortcut of `os.execute`:
```
return shell('echo hello')
```

## Callbacks
The default shell functionality can be extended by creating custom event
callbacks in the users profile. There are four different event sources:

* `client_ready()`          Called when the client is connected
* `client_<command>(param)` Called when the client receives a `<command>`
* `server_<command>(param)` Called when the server receives a `<command>`
* `server_prompt(line)`     Called when the server processes a prompt

All callbacks except `client_ready` must return a `boolean`. In case `true`
is returned, all further processing will be prevented. The `client_ready`
callback must return a `string`, which will be displayed by the client.

The `<command>` names will be converted to lowercase.

Here is an example on how to implement an simple _Echo Server_:
```
function _P.client_ready()
  return 'This is an echo server'
end
```
```
function _P.client_echo(param)
  _P.net.send('ECHO', param)
  return true
end
```
```
function _P.server_echo(param)
  io.write(param .. '\n')
  return true
end
```
```
function _P.server_prompt(line)
  _P.net.send('ECHO', line)
  return true
end
```

# Protocol
The Palantir protocol consists of two layers:

1. Network Layer (_transportation_ handled by `C`)
2. Command Layer (_interpretation_ handled by `Lua`)

## Network Layer
A network frame is build according to the following format:
```
CHECKSUM (4 bytes) | SIZE (4 bytes) | DATA (n bytes)
```
The `CHECKSUM` is a _bitwise CRC-32_ over the `DATA` field only.

### Authentication
If an authentication `TOKEN` is provided, the network frames checksum will be
pre-feed with the _CRC-32_ of the token before calculation.

## Command Layer
A command is build according to the following format:
```
COMMAND (4 bytes) | BLANK (1 byte) | PARAM (n bytes)
```
Each command consists of a `5` byte command header followed by `0` to `n`
bytes of `param`. A command header will end with a single blank ` ` character
for better readability.

If an unknown command is received, no error will be raised, instead it will be
ignored by the client and server.

### Server Commands
* `HELO <user>@<host>:<path>`
* `TEXT <text>`

### Client Commands
* `EXEC <command>`
* `PATH <path>`
* `HALT`

### Examples
```
Client: HELO root@localhost:/
```
```
Server: PATH var
```

```
Client: HELO root@localhost:/var
```
```
Server: EXEC return shell('echo hello')
```
```
Client: TEXT hello
```

```
Client: HELO root@localhost:/var
```
```
Server: EXIT
```

# License
Licensed under the terms of the [MIT License](LICENSE).
