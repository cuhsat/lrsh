# Environment
A new global table named `_P` will be defined which contains all shell 
specific functions and properties.

## Keyboard
* <kbd>Ctrl</kbd>+<kbd>n</kbd> inserts a new line
* <kbd>Tab</kbd> autocompletes keywords, functions, globals and commands

> Only available if compiled with `readline` support.

## Modules
[Extension modules](https://www.github.com/cuhsat/palantir-modules) can be
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

## Constants
* `MODE`    The command line option `-d`
* `TOKEN`   The command line argument `-a`
* `STACK`   The command line argument `-c` or `-f`
* `HOST`    The command line argument `HOST`
* `PORT`    The command line argument `PORT`
* `DEBUG`   The debug flag if compiled with `-DDEBUG=1`
* `BUILD`   The build system (only if debug flag is set)
* `VERSION` The semantic version number

## Functions

### Common

#### error(message)
Prints the error `message`.

#### event(source, event, param)
Calls the function `<source>_<event>(<param>)` if exists.

#### input(source)
Builds the input stack from `source`.

#### load(chunk)
Returns the output of the executed `chunk` (global environment).

### Network

#### net.server(host, port)
Starts as server on `host` and `port`.

#### net.client(host, port)
Starts as client on `host` and `port`.

#### net.connect(host, port)
Connects to `host` and `port`.

#### net.listen(host, port)
Listens on `host` and `port`.

#### net.accept()
Accepts incoming connections.

#### net.recv()
Returns the received `command` and `param`.

#### net.send(command, param)
Sends the given `command` and `param`.

### Operating System

#### os.env(path)
Returns the `user`, `host` and `path` variables, sets the `path` if given.

#### os.execute(command)
Returns the `command` output executed by the users default shell.

#### os.prompt(prompt)
Prints `prompt` and returns the users input.

#### os.sleep(milliseconds)
Sleeps for the given `milliseconds`.

### Alias
The alias `shell` is provided as a shortcut of `os.execute`:
```
return shell('echo hello')
```

## Stack
The input stack can be specified by the command line options `-f` and `-c`.
The option `-f` has precedence over the option `-c`. If an input stack is 
provided, the commands will be processed line by line. The shell will not 
exit automatically after all commands are processed.

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

Here is an example on how to implement an simple _echo server_:
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
