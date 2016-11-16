# Palantir ![Build](https://img.shields.io/travis/cuhsat/palantir.svg)
Palantir is a [Lua](https://www.lua.org) scriptable, extendable, tiny reverse
shell, using a human readable protocol written in C and Lua.

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

The option `-f` has precedence over the option `-c`. The program will not exit 
after all commands, either specified by `-c` or `-f`, are processed.

### Commands
* `-- exit` Shutdown server
* `-- halt` Shutdown client

All input will be evaluated and execute as Lua commands. The internal function
`shell` will execute system commands by using the users default shell and 
return the results where `strerr` will be mapped to `stdout`.

### Keyboard Shortcuts
* <kbd>Ctrl</kbd>+<kbd>n</kbd> inserts a new line
* <kbd>Tab</kbd> autocompletes keywords, functions, globals and commands

> Only available if `readline` support was enabled.

### Modules
Palantir extension modules can be place under `~/.palantir/`.

### Profile
An user specific profile can be place under `~/.palantir.lua`.

Here is an example profile:
```
-- greet client
function P.client_ready()
  return 'Hello\n'
end

-- debug
print('Profile loaded')
```

## License
Licensed under the terms of the [MIT License](LICENSE).
