![Palantir](res/logo_text.png)

Palantir is a Lua scriptable, extendable, tiny reverse shell, using a human 
readable protocol written in C and Lua.

[Read the environment documentation](doc/environment.md)
[Read the protocol documentation](doc/protocol.md)

![Build](https://img.shields.io/travis/cuhsat/palantir.svg)

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

## Commands
* `-- exit` Shutdown server
* `-- halt` Shutdown client

All input will be evaluated and execute as Lua commands. The internal function
`shell` will execute system commands by using the users default shell and
return the results where `strerr` will be mapped to `stdout`.

## Build
```
$ cd build && cmake ..
```

Required:
* [Lua 5.1](https://www.lua.org)

Supported:
* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)

## License
Licensed under the terms of the [MIT License](LICENSE).