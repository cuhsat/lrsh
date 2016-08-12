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
function `shell` will execute system commands by using the users default
system shell and return the results.

## Environment
An user specific configuration file can be place under `~/.palantir.lua`.

Here is an example configuration file:
```
-- greet client
function client_connect()
  return 'Hello'
end

-- debug
print('Profile loaded')
```

### Globals
A new global table named `palantir` will be defined which contains all shell
specific functions and properties.

#### Constants
General constants:

##### `MODE`
The command line option `-d`.

##### `HOST`
The command line argument `HOST`.

##### `PORT`
The command line argument `PORT`.

##### `DEBUG`
The `DEBUG` flag (to set compile with `-DDEBUG=1`).

##### `VERSION`
The semantic version number.

#### Functions
General functions:

##### `error(message)`
The default error handler, will print out the given error `message`.

##### `event(source, event, param)`
The default event trigger, will call the function `<source>_<event>(<param>)`
if this function exists.

##### `load(chunk)`
Returns the `chunk`s output evaluated and executed as Lua code. The evaluated
symbols will be added to the global environment (`_G`).

#### Net
Network specific functions:

##### `net.server(host, port)`
Start the main program loop on the given `host` and `port` in _active_ mode.

##### `net.client(host, port)`
Start the main program loop on the given `host` and `port` in _passive_ mode.

##### `net.connect(host, port)`
Connects to the given `host` and `port`.

##### `net.listen(host, port)`
Listens on the given `host` and `port` for incoming connections.

##### `net.accept()`
Accepts incoming connections.

> This is a _blocking_ function.

##### `net.recv()`
Returns the received `command` and `param` as `string`s. If the `param` was
not specified, `nil` is returned instead.

> This is a _blocking_ function.

##### `net.send(command, param)`
Sends the given `command` and `param` as `string`s. If the `param` is not
specified, an empty string (`''`) will be send instead.

> This is a _blocking_ function.

#### OS
Operation system specific functions:

##### `os.env(path)`
Returns the `user`, `host` and `path` system environment as `string`s. If a
`path` is given, the current working directory will be changed to it. If
started in _passive_ mode, the working directory will be set to the _root_
directory (`/`).

##### `os.execute(command)`
Returns the `command`s output executed by the users default system shell as a
`string`.

> `strerr` will be mapped to `stdout`.

##### `os.readline(prompt)`
Shows the given `prompt` and returns the users input as `string`.

##### `os.sleep(time)`
The execution will be stopped for the given `time` in milliseconds. The `time`
must be given as `integer`.

### Callbacks
The default shell functionality can be extended by creating custom event
callbacks in the user specific configuration file. There are four different
event sources:

* `client_connect`   called when the client connects
* `client_<command>` called when the client receives a `<command>`
* `server_<command>` called when the server receives a `<command>`
* `server_prompt`    called when the server processes a prompt

The `<command>` names will be converted to lowercase. All callbacks except
`client_connect` must return a `boolean`. In case `true` is returned, all
further processing will be prevented.

The `client_connect` callback must return a `string`, which will be displayed
by the client.

Here is an example on how to implement an simple echo server:
```
function client_connect()
  return 'This is an echo server'
end
```
```
function client_echo(param)
  palantir.net.send('ECHO', param)
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
function server_prompt(line)
  palantir.net.send('ECHO', line)
  return true
end
```

### Shortcuts
The shortcut `shell` is provided as an alias of `palantir.os.execute`:
```
print(shell('echo hello'))
```

## Protocol
The protocol consists of two layers:

1. Network Layer (_transportation_ handled by `C`)
2. Command Layer (_interpretation_ handled by `Lua`)

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
* `HELO` Shows the command prompt
* `TEXT` Prints the parameter

#### Commands issued by the server:
* `EXEC` Executes the parameter
* `EXIT` Exits the client
* `PATH` Changes the path

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

## Building
```
$ make all test
```

#### Dependancies
The following libraries are required:

* [Lua 5.3](https://www.lua.org)
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

> The `readline` support is optional and can be turned off by removing the
> compiler flag `-DREADLINE` from the [makefile](Makefile).

## License
Licensed under the terms of the [MIT License](LICENSE).
