# Redis From Scratch

A Redis-like in-memory database server built from scratch in C++.

## Current Progress

* TCP socket server
* IPv4 support
* Socket binding
* Port configuration
* Connection listening

## Run

```bash
g++ src/main.cpp src/RedisServer.cpp -o output/main
./output/main  6379
```

Built for learning systems programming, networking, and database internals.
