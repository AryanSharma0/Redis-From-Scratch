# Redis From Scratch

A Redis-like in-memory database server built from scratch in C++ for learning systems programming, networking, and database internals.

## Current Progress

* TCP socket server
* IPv4 support
* Socket creation
* Socket binding
* Configurable port
* TCP connection listening
* Client connection acceptance
* Multi-client handling using threads
* Basic command handling
* In-memory key-value storage
* List storage
* Hash storage
* Thread-safe database access
* Basic `DEL` operation
* Key expiration support
* Database persistence using `dump.rdb`
* Database loading from `dump.rdb`
* Automatic periodic database persistence

## Project Structure

```text
Redis-From-Scratch/
├── include/
│   ├── RedisServer.h
│   ├── RedisDatabase.h
│   └── CommandHandler.h
│
├── src/
│   ├── main.cpp
│   ├── RedisServer.cpp
│   ├── RedisDatabase.cpp
│   └── CommandHandler.cpp
│
├── build/
├── dump.rdb
└── Makefile
```

## Build

Using the Makefile:

```bash
make
```

Or compile manually:

```bash
g++ -std=c++17 -Wall -Wextra -pthread \
    src/main.cpp \
    src/RedisServer.cpp \
    src/RedisDatabase.cpp \
    src/CommandHandler.cpp \
    -Iinclude \
    -o my_redis_server
```

## Run

Default port:

```bash
./my_redis_server
```

Custom port:

```bash
./my_redis_server 6379
```

The server starts and listens for TCP client connections on the configured port.

## Purpose

This project is being developed from scratch to understand how an in-memory database such as Redis works internally, including:

* TCP networking
* Socket programming
* Concurrent client handling
* Command processing
* In-memory data structures
* Thread synchronization
* Key expiration
* Persistence
* Database loading and recovery

This is a learning project and is not intended to be a production replacement for Redis.
