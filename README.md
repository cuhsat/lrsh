# Palantir ![](http://lua-users.org/files/wiki_insecure/lua-icons-png/lua-48x48.png)
Palantir is a [Lua](https://www.lua.org) scriptable, extendable, tiny reverse 
shell, using a human readable protocol written in C and Lua.

## Usage
```
$ palantir [-hlv] [-p] IP PORT
```

### Options:
* `-p` Start in passiv mode (listen)
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
The following global variables will be defined:

* `MODE`    The command line option `-p`
* `HOST`    The command line argument `IP`
* `PORT`    The command line argument `PORT`
* `DEBUG`   If compiled with `DEBUG` flag set
* `TIMEOUT` The time between connection attempts (defaults to `5` seconds)
* `VERSION` The version number

### Functions
The following global functions will be defined:

#### connect(host, port)
Connects to the given `host` and `port`.

#### listen(host, port)
Listens on the given `host` and `port`.

#### accept()
Accepts an incoming connection.

#### recv()
Returns the received `command` and `param`.

#### send(command, param)
Sends the given `command` and `param`.

#### info(path)
Returns `user`, `host`, `path` and sets the `path` if given.

#### sleep(time)
Sleeps for the given `time` (in milliseconds).

#### exec(command)
Returns the executed shell `command` output.

#### eval(command)
Returns the executed Lua `command` output.

#### fail(err)
The default error handler that prints `err`.

#### client(host, port)
Start as client on the given `host` and `port`.

#### server(host, port)
Start as server on the given `host` and `port`.

## Protocol
The protocol consists of two layers:

1. Network Layer (_transportation_)
2. Command Layer (_interpretation_)

### Network Layer
A network frame is build according to the following format:
```
+--------------------+----------------+----------------+
| CHECKSUM (4 bytes) | SIZE (4 bytes) | DATA (n bytes) |
+--------------------+----------------+----------------+
```
The `CHECKSUM` is a bitwise CRC-32 only over the `DATA` field.

### Command Layer
A command is build according to the following format:
```
+-------------------+----------------+-----------------+
| COMMAND (4 bytes) | BLANK (1 byte) | PARAM (n bytes) |
+-------------------+----------------+-----------------+
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
Client: INIT root@localhost:/:
```
```
Server: PATH var
```

```
Client: INIT root@localhost:/var:
```
```
Server: EXEC exec('echo hello') .. ' world'
```
```
Client: TEXT hello world
```

```
Client: INIT root@localhost:/var:
```
```
Server: HALT
```

## License
Licensed under the terms of the [MIT License](LICENSE).
