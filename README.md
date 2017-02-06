# Palantir ![Build](https://img.shields.io/travis/cuhsat/palantir.svg)
Palantir is a Lua scriptable, [extendable](doc/environment.md), tiny reverse 
shell, using a human readable [protocol](doc/protocol.md) written in C and 
[Lua](https://www.lua.org).

## Usage
```
$ palantir [-dhlv] [-a TOKEN] [-c COMMAND] [-f FILE] HOST PORT
```

### Options
* `-d` Starts in passive mode _(listen)_
* `-h` Shows the usage information
* `-l` Shows the license
* `-v` Shows the version
* `-a` Authentication token
* `-c` Executes the command
* `-f` Executes the file

> The option `-f` has precedence over the option `-c`. The shell will not exit 
> after all commands, either specified by `-c` or `-f`, are processed.

## Commands
* `-- exit` Shutdown server
* `-- halt` Shutdown client

All input will be evaluated and execute as Lua commands. The internal function
`shell` will execute system commands by using the users default shell and 
return the results where `strerr` will be mapped to `stdout`.

## Keyboard
* <kbd>Ctrl</kbd>+<kbd>n</kbd> inserts a new line
* <kbd>Tab</kbd> autocompletes keywords, functions, globals and commands

> Only available if `readline` support was [enabled](doc/build.md).

## License
Licensed under the terms of the [MIT License](LICENSE).