# Palantir
Palantir is a minimal, Lua scriptable reverse shell using a simple human 
readable protocol.

## Usage
```
$ palantir [-chlv] IP PORT
```

### Options:
* `-c` Start in client mode (daemon)
* `-h` Shows the usage information
* `-l` Shows the license
* `-v` Shows the version

### Commands:
* `/eval` Evaluate Lua code
* `/exit` Shutdown server
* `/halt` Shutdown client

All other commands will be execute be the clients default system shell.

> A user specific configuration file can be place under `~/.palantirrc`.

## Protocol
The used protocol consists of two layers:

1. Network Layer
2. Command Layer

### Network Layer
A typical network frame is build by the following format:
```
[ CHECKSUM (4 bytes) | LENGTH (4 bytes) | COMMAND (n bytes) ]
```
> The `CHECKSUM` is a bitwise CRC-32 over the `COMMAND` field only.

### Command Layer
Each command consists of a `5` byte command header followed by `0` to `n` 
bytes of data. A command header will end with a single blank ` ` character 
for better readability.

#### Commands issued by the server (as requests):
* `EVAL` Evaluate Lua code
* `EXEC` Execute by shell
* `HALT` Shutdown client
* `PATH` Change directory

#### Commands issued by the client (as responses):
* `INIT` Show prompt
* `TEXT` Print text

### Examples
```
Client: INIT root@localhost:/
```
```
Server: EXEC echo hello
```
```
Client: TEXT hello
```

## License
Licensed under the terms of the [MIT License](LICENSE).
