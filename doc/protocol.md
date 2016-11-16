# Protocol
The Palantir protocol consists of two layers:

1. Network Layer (_transportation_ handled by `C`)
2. Command Layer (_interpretation_ handled by `Lua`)

## Network Layer
A network frame is build according to the following format:
```
CHECKSUM (4 bytes) | SIZE (4 bytes) | DATA (n bytes)
```
The `CHECKSUM` is a bitwise CRC-32 over the `DATA` field only.

### Authentication
If an authentication `TOKEN` is provided, the network frames checksum will be 
pre-feed with the CRC-32 of the token before calculation.

## Command Layer
A command is build according to the following format:
```
COMMAND (4 bytes) | BLANK (1 byte) | PARAM (n bytes)
```
Each command consists of a `5` byte command header followed by `0` to `n`
bytes of `param`. A command header will end with a single blank ` ` character
for better readability.

If an unknown command is received, no error will be raised, instead it will be
ignored by the server and client.

### Server Commands

#### HELO [user]@[host]:[path]
Shows the command prompt.

#### TEXT [text]
Prints the text.

### Client Commands

#### EXEC [command]
Executes the command.

#### PATH [path]
Changes the path.

#### HALT
Halts the client.

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
