# Palantir ![logo](doc/logo.png)
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
command `execute` will execute system commands by using the users default 
system shell and return the results (`strerr` will be mapped to `stdout`).

## Environment
A user specific configuration file can be place under `~/.palantirrc`.

### Globals
A new global table named `palantir` will be defined which contains all shell 
specific functions and variables:

#### Functions
The following functions will be definded:

##### palantir.execute(chunk)
Returns the `chunk`s output executed by the users default shell.

##### palantir.load(chunk)
Returns the `chunk`s output executed and evaluated by Lua.

##### palantir.recv()
Returns the received `command` and `param`.
 
##### palantir.send(command, param)
Sends the given `command` and `param`.

##### palantir.sleep(time)
The execution will be stopped for the given `time` in milliseconds.

##### palantir.system(path)
Returns the `user`, `host` and `path` system infos. If a `path` is given, the 
current working directory will be changed to it.

#### Variables
The following variables will be definded:

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
* `INIT` Show prompt
* `TEXT` Print text

#### Commands issued by the server:
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
Server: EXEC return palantir.execute('echo hello')
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
