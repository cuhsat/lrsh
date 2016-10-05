# Palantir
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
shell and return the results where `strerr` will be mapped to `stdout`.

## Environment
An user specific profile can be place under `~/.palantir.lua`.

Here is an example profile:
```
-- greet client
function client_connected()
  return 'Hello\n'
end

-- debug
print('Profile loaded')
```

### Globals
A new global table named `palantir` will be defined which contains all shell
specific functions and properties.

#### Constants
* `MODE`    The command line option `-d`
* `HOST`    The command line argument `HOST`
* `PORT`    The command line argument `PORT`
* `DEBUG`   The debug flag if compiled with `-DDEBUG=1`
* `VERSION` The semantic version number

#### Functions

##### `error(message)`
Prints the error `message`.

##### `event(source, event, param)`
Calls the function `<source>_<event>(<param>)` if exists.

##### `load(chunk)`
Returns the output of the executed `chunk` (global environment).

#### Network

##### `net.server(host, port)`
Starts on `host` and `port` in _active mode_.

##### `net.client(host, port)`
Starts on `host` and `port` in _passive mode_.

##### `net.connect(host, port)`
Connects to `host` and `port`.

##### `net.listen(host, port)`
Listens on `host` and `port`.

##### `net.accept()`
Accepts incoming connections. _(blocking)_

##### `net.recv()`
Returns the received `command` and `param`. _(blocking)_

##### `net.send(command, param)`
Sends the given `command` and `param`. _(blocking)_

#### Operating System

##### `os.env(path)`
Returns the `user`, `host` and `path` variables, sets the `path` if given.

##### `os.execute(command)`
Returns the `command` output executed by the users default shell.

##### `os.readline(prompt)`
Prints `prompt` and returns the users input.

##### `os.sleep(milliseconds)`
Sleeps for the given `milliseconds`.

### Callbacks
The default shell functionality can be extended by creating custom event
callbacks in the users profile. There are four different event sources:

* `client_connected` called when the client connects
* `client_<command>` called when the client receives a `<command>`
* `server_<command>` called when the server receives a `<command>`
* `server_prompt`    called when the server processes a prompt

All callbacks except `client_connect` must return a `boolean`. In case `true`
is returned, all further processing will be prevented. The `client_connected`
callback must return a `string`, which will be displayed by the client.

The `<command>` names will be converted to lowercase.

Here is an example on how to implement an simple echo server:
```
function client_connected()
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
return shell('echo hello')
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
$ make all test install
```

#### Dependancies
The following libraries are required/supported:

* [Lua 5.3](https://www.lua.org)
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

> The `readline` support is optional and can be turned on by calling `make`
> with the `USE_READLINE` flag set:
>
> ```
> $ make all test install USE_READLINE=1
> ```

## License
Licensed under the terms of the [MIT License](LICENSE).