# Palantir ![Logo](logo.png)
Palantir is a [Lua](https://www.lua.org) scriptable, extendable, tiny reverse 
shell, using a human readable protocol written in C and Lua.

## Usage
```
$ palantir [-hlv] [-d] HOST PORT
```

### Options:
* `-d` Start in passiv mode (listen)
* `-h` Shows the usage information
* `-l` Shows the license
* `-v` Shows the version

### Commands:
* `-- halt` Shutdown client
* `-- exit` Shutdown server

All other input will be evaluated and execute as Lua commands. The internal
command `exec` will execute system commands by using the users default 
system shell and return the results (`strerr` will be mapped to `stdout`).

## Environment
A user specific configuration file can be place under `~/.palantirrc`.

### Variables
The following additional global variables will be defined:

* `MODE`    The command line option `-d`
* `HOST`    The command line argument `HOST`
* `PORT`    The command line argument `PORT`
* `DEBUG`   If compiled with `DEBUG` flag set
* `TIMEOUT` The time between connection attempts (defaults to `5` seconds)
* `VERSION` The version number

### Functions
The following additional global functions will be defined:

#### eval(param)
Returns the `param` output executed and evaluated by Lua.

#### exec(param)
Returns the `param` output executed by the users default shell.

#### fail(err)
The default error handler that prints `err`.

#### info(path)
Returns `user`, `host`, `path` and sets the `path` if given.

#### recv()
Returns the received `command` and `param`.

#### send(command, param)
Sends the given `command` and `param`.

#### sleep(time)
Sleeps for the given `time` (in milliseconds).

### Callbacks
The default shell functionality can be extended by creating custom event 
callbacks in the configuration file. There are three different events:

* `client_<command>` will be execute if the client receives a `<command>`
* `server_<command>` will be execute if the server receives a `<command>`
* `server_input`     will be execute if the server processes an input

All callbacks must return either `true` or `false`. In case `true` is
returned, all further processing will be prevented.

Here is an example on how to implement an simple echo server:
```
function client_echo(param)
  send('ECHO', param)
  return true
end

function server_echo(param)
  io.write(param .. '\n')
  return true
end

function server_input(line)
  send('ECHO', line)
  return true
end
```

## Protocol
The protocol consists of two layers:

1. Network Layer (_transportation_)
2. Command Layer (_interpretation_)

### Network Layer
A network frame is build according to the following format:
```
CHECKSUM (4 bytes) | SIZE (4 bytes) | DATA (n bytes)
```
The `CHECKSUM` is a bitwise CRC-32 only over the `DATA` field.

### Command Layer
A command is build according to the following format:
```
COMMAND (4 bytes) | BLANK (1 byte) | PARAM (n bytes)
```
Each command consists of a `5` byte command header followed by `0` to `n` 
bytes of `param`. A command header will end with a single blank ` ` character 
for better readability.

#### Commands issued by the client (as responses):
* `INIT` Show prompt
* `TEXT` Print text

#### Commands issued by the server (as requests):
* `EXEC` Execute command
* `HALT` Halt client
* `PATH` Change path

### Examples
```
Client: INIT root@localhost:/
```
```
Server: PATH var
```

```
Client: INIT root@localhost:/var
```
```
Server: EXEC return exec('echo hello')
```
```
Client: TEXT hello
```

```
Client: INIT root@localhost:/var
```
```
Server: HALT
```

## License
Licensed under the terms of the [MIT License](LICENSE).