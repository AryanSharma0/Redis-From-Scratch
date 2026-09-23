# Redis From Scratch

A Redis-like in-memory database server built from scratch in **C++17**.

## Features

* TCP server
* Multiple client connections
* RESP-style command parsing
* String storage
* List storage
* Hash storage
* Key expiration
* Persistence
* Thread synchronization

---

## Build

```bash
make
```

## Run

```bash
./my_redis_server
```

Run on a custom port:

```bash
./my_redis_server 6379
```

Start the client:

```bash
./redis_client
```

---

# Commands

## PING

Check if the server is running.

```text
PING
```

Response:

```text
PONG
```

---

## ECHO

Return a message.

```text
ECHO hello
```

With spaces:

```text
ECHO "hello world"
```

---

# String Commands

## SET

Store a value.

```text
SET name Aryan
```

With spaces:

```text
SET name "Aryan Sharma"
```

## GET

Get a value.

```text
GET name
```

---

# Key Commands

## DEL

Delete a key.

```text
DEL name
```

## UNLINK

Delete a key.

```text
UNLINK name
```

## EXPIRE

Set expiration time in seconds.

```text
EXPIRE name 60
```

## RENAME

Rename a key.

```text
RENAME old_name new_name
```

## KEYS

Show stored keys.

```text
KEYS
```

## TYPE

Check the type of a key.

```text
TYPE name
```

Possible results:

```text
string
list
hash
none
```

## FLUSHALL

Remove all stored data.

```text
FLUSHALL
```

---

# List Commands

## LPUSH

Add elements to the beginning of a list.

```text
LPUSH mylist a
```

Multiple values:

```text
LPUSH mylist a b c
```

## RPUSH

Add elements to the end of a list.

```text
RPUSH mylist a
```

Multiple values:

```text
RPUSH mylist a b c
```

## LLEN

Get the length of a list.

```text
LLEN mylist
```

## LPOP

Remove and return the first element.

```text
LPOP mylist
```

## RPOP

Remove and return the last element.

```text
RPOP mylist
```

## LREM

Remove matching elements.

```text
LREM mylist 2 value
```

* `count > 0` — remove from the beginning
* `count < 0` — remove from the end
* `count = 0` — remove all matching elements

## LINDEX

Get an element by index.

```text
LINDEX mylist 0
```

Last element:

```text
LINDEX mylist -1
```

## LSET

Replace an element at an index.

```text
LSET mylist 0 new_value
```

Negative indexes are supported.

---

# Hash Commands

Hash structure:

```text
key -> field -> value
```

Example:

```text
user
 ├── name -> Aryan
 └── age  -> 23
```

## HSET

Set a field and value inside a hash.

```text
HSET user name Aryan
```

Another field:

```text
HSET user age 23
```

---

## HGET

Get a field value.

```text
HGET user name
```

---

## HEXISTS

Check whether a field exists.

```text
HEXISTS user name
```

Response:

```text
1
```

or:

```text
0
```

---

## HDEL

Delete a field from a hash.

```text
HDEL user name
```

---

## HLEN

Get the number of fields in a hash.

```text
HLEN user
```

---

## HKEYS

Get all fields in a hash.

```text
HKEYS user
```

---

## HVALS

Get all values in a hash.

```text
HVALS user
```

---

## HGETALL

Get all fields and values from a hash.

```text
HGETALL user
```

---

## HMSET

Set multiple field-value pairs.

```text
HMSET user name Aryan age 23 city Patna
```

The format is:

```text
HMSET key field value field value ...
```

Example:

```text
HMSET user name Aryan age 23 city Patna
```

---

# Example Session

```text
PING

SET name Aryan
GET name

RPUSH mylist one two three
LLEN mylist
LINDEX mylist 0
LPOP mylist
RPOP mylist

HSET user name Aryan
HSET user age 23
HGET user name
HLEN user
HKEYS user
HVALS user
HGETALL user

DEL name
```

---

# Project Structure

```text
.
├── include/
│   ├── RedisServer.h
│   ├── RedisDatabase.h
│   ├── CommandHandler.h
│   └── RESPParser.h
│
├── src/
│   ├── main.cpp
│   ├── RedisServer.cpp
│   ├── RedisDatabase.cpp
│   ├── CommandHandler.cpp
│   └── RESPParser.cpp
│
├── Makefile
├── dump.rdb
└── README.md
```

---

# Command Summary

| Category | Commands                                                                        |
| -------- | ------------------------------------------------------------------------------- |
| Basic    | `PING`, `ECHO`                                                                  |
| Key      | `SET`, `GET`, `DEL`, `UNLINK`, `EXPIRE`, `RENAME`, `KEYS`, `TYPE`, `FLUSHALL`   |
| List     | `LPUSH`, `RPUSH`, `LLEN`, `LPOP`, `RPOP`, `LREM`, `LINDEX`, `LSET`              |
| Hash     | `HSET`, `HGET`, `HEXISTS`, `HDEL`, `HLEN`, `HKEYS`, `HVALS`, `HGETALL`, `HMSET` |

---

# Status

### Implemented

* TCP server
* Client connections
* RESP parsing
* String commands
* List commands
* Hash commands
* Key management
* Expiration
* Persistence
* Multi-client support

### In Progress

* Improved persistence
* More testing
