Palantir
========
Palantir is a minimal, Lua scriptable reverse shell using a simple human 
readable [protocol](doc/PROTOCOL.md).

Usage
-----
```
$ palantir [-chlv] IP PORT
```

Options:
* `-c` Starts in client mode (daemon)
* `-h` Shows the usage information
* `-l` Shows the license
* `-v` Shows the version

Commands:
* `/eval` Evaluate Lua code
* `/exit` Shutdown server
* `/halt` Shutdown client

All other commands will be execute be the clients default system shell.

> A user specific configuration file can be place under `~/.palantirrc`.

License
-------
Licensed under the terms of the [MIT License](LICENSE).
