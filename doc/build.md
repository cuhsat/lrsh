# Build
```
$ make all test install
```

## Flags
Use these flags when calling [`make`](../Makefile):

* `NO_DAEMON`   Disable daemonize support
* `NO_READLINE` Disable `readline` support

```
$ make all test install <flag>=1
```

## Dependancies
The following libraries are required:

* [Lua 5.1](https://www.lua.org)

The following libraries are supported:

* [Readline](https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)
