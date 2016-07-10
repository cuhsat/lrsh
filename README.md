# Palantir ![logo](doc/palantir.png)
Palantir is a [Lua](https://www.lua.org) scriptable, extendable, tiny reverse 
shell, using a human readable protocol written in C and Lua.

## Usage
```
$ palantir [-hlv] [-d] HOST PORT
```

### Options:
* `-d` Start in passive mode (listen)
* `-h` Shows the usage information
* `-l` Shows the license
* `-v` Shows the version

### Commands:
* `-- exit` Shutdown client
* `-- halt` Shutdown server

All other input will be evaluated and execute as Lua commands. The internal
command `execute` will execute system commands by using the users default 
system shell and return the results.

## Environment
A user specific configuration file can be place under `~/.palantirrc`.

Here is an example configuration file:
```
-- faster reconnect
palantir.timeout = 1000

-- shortcut
function shell(param)
  return palantir.execute(param)
end

print('Profile loaded')
```

### Globals
A new global table named `palantir` will be defined which contains all shell 
specific functions and variables:

#### Functions
The following functions will be definded for user usage:

##### palantir.execute(chunk)
Returns the `chunk`s output executed by the users default system shell as a 
`string` (`strerr` will be mapped to `stdout`).

##### palantir.load(chunk)
Returns the `chunk`s output evaluated and executed as Lua code. The evaluated 
symbols will be added to the global environment (`_G`).

##### palantir.recv()
Returns the received `command` and `param` as `string`s. If the `param` was 
not specified, `nil` is returned instead.
 
##### palantir.send(command, param)
Sends the given `command` and `param` as `string`s. If the `param` is not 
specified, an empty string (`''`) will be send instead.

##### palantir.sleep(time)
The execution will be stopped for the given `time` in milliseconds. The `time`
must be given as an `integer`.

##### palantir.info(path)
Returns the `user`, `host` and `path` system infos as `string`s. If a `path` 
is given, the current working directory will be changed to it. If started in
_passive_ mode, the working directory will be set to the root directory (`/`).

#### Variables
The following variables will be definded for user usage:

##### palantir.mode
The command line option `-d`.

##### palantir.host
The command line argument `HOST`.

##### palantir.port
The command line argument `PORT`.

##### palantir.debug
The `DEBUG` flag (to set compile with `-DDEBUG=1`).

##### palantir.timeout
The time between connection attempts (default is `5000` milliseconds).

##### palantir.version
The semantic version number.

### Callbacks
The default shell functionality can be extended by creating custom event 
callbacks in the user specific configuration file. There are three different 
events sources:

* `client_<command>` called when the client receives a `<command>`
* `server_<command>` called when the server receives a `<command>`
* `server_input`     called when the server processes an input

The `<command>` names will be converted to lowercase. All callbacks must 
return a `boolean`. In case `true` is returned, all further processing will 
be prevented.

Here is an example on how to implement an simple echo server:
```
function client_echo(param)
  palantir.send('ECHO', param)
  return true
end
```
```
function server_echo(param)
  io.write(param .. '\n')
  return true
end
```
```
function server_input(line)
  palantir.send('ECHO', line)
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

#### Commands issued by the client:
* `HELO` Show prompt
* `TEXT` Print text

#### Commands issued by the server:
* `EXEC` Execute command
* `EXIT` Exit client
* `PATH` Change path

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
Server: EXEC return palantir.execute('echo hello')
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

## Dependancies
The following libraries are required:

* [Lua 5.3](https://www.lua.org)
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

## License
Licensed under the terms of the [MIT License](LICENSE).
