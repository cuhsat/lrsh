![Palantir](doc/palantir.png)

Palantir is a Lua scriptable, portable, tiny reverse shell, using a human
readable protocol written in C and Lua.

# Build ![Build](https://img.shields.io/travis/cuhsat/palantir.svg)
```
$ cmake . && make
```

Required:
* [CMake 3.0](https://cmake.org)
* [Lua 5.1](https://www.lua.org)

Supported:
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

# Usage
```
$ palantir [-dhlv] [-a TOKEN] HOST PORT
```

## Options
* `d` Start as server
* `h` Shows the usage information
* `l` Shows the license
* `v` Shows the version
* `a` Authentication token

## Commands
* `-- exit` Shutdown client
* `-- halt` Shutdown server

All input will be evaluated and execute as Lua commands. The internal function
`os.shell` will execute system commands by using the users default shell and
return the results where `strerr` will be mapped to `stdout`.

## Keyboard
* <kbd>Tab</kbd> autocompletes keywords, functions, globals and commands
* <kbd>Ctrl</kbd>+<kbd>n</kbd> inserts a new line
* <kbd>Ctrl</kbd>+<kbd>x</kbd> termintates the shell

> Only available if compiled with `readline` support.

## Profile
A user specific [profile](https://www.github.com/cuhsat/palantir-profile) can 
be placed under `~/.palantir.lua`.

# Environment
New global constants and functions will be defined which contain all shell
specific extensions.

## Constants
* `SERVER`  The command line option `d`
* `TOKEN`   The command line argument `a`
* `HOST`    The command line argument `HOST`
* `PORT`    The command line argument `PORT`
* `HOME`    The stated users home directory
* `BUILD`   The compiled build information
* `DEBUG`   The compiled debug flag
* `VERSION` The compiled version number

## Functions

### Network
The network functions share one socket and are not meant to be re-opened:

* `net.server(host, port)`
* `net.client(host, port)`
* `net.connect(host, port)`
* `net.listen(host, port)`
* `net.accept()`
* `net.recv()`
* `net.send(command, param)`

### Operating System
The operating system functions will extend the Lua build-in `os` library:

* `os.path(path)`
* `os.prompt(prompt)`
* `os.shell(command)`
* `os.sleep(milliseconds)`

## Callbacks
The default shell functionality can be extended by creating custom event
callbacks in the users profile. There are four different event sources:

* `server_ready()`          Called when the server is connected
* `server_<command>(param)` Called when the server receives a `<command>`
* `client_<command>(param)` Called when the client receives a `<command>`
* `client_prompt(line)`     Called when the client processes a prompt

All callbacks except `server_ready` must return a `boolean`. In case `true`
is returned, all further processing will be prevented. The `client_ready`
callback must return a `string`, which will be displayed by the client.

The `<command>` names will be converted to lowercase.

Here is an example on how to implement an simple _Echo Server_:
```
function server_ready()
  return 'This is an echo server'
end
```
```
function server_echo(param)
  net.send('ECHO', param)
  return true
end
```
```
function client_echo(param)
  io.write(param .. '\n')
  return true
end
```
```
function client_prompt(line)
  net.send('ECHO', line)
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

### Server issued
* `HELO <user>@<host>:<path>`
* `TEXT <text>`

### Client issued
* `EXEC <command>`
* `PATH <path>`
* `HALT`

### Examples
```
Server: HELO root@localhost:/
```
```
Client: PATH var
```

```
Server: HELO root@localhost:/var
```
```
Client: EXEC return os.shell('echo hello')
```
```
Server: TEXT hello
```

```
Server: HELO root@localhost:/var
```
```
Client: HALT
```

# License
Licensed under the terms of the [MIT License](LICENSE).
