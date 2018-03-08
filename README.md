# Cld - Cld is a Lightweight Downloader

## Introduction

Cld is a lightweight downloader, which is based on epoll.
The project are now in a early stage with poor sturctures but have been usable.
These features have been implemented:

 * Resolve URL
 * Support HTTP and HTTPS download (Unstable when meet some response)
 * Full non-blocking connect, read and write
 * Follow redirects (Unstable)

## Get

```bash
cmake [project dir]
make
sudo make install
```

(only one executable file (cld) will be installed)

## Usage

Just run

```bash
cld
```

or

```bash
cld --help
```

to get help

## Other

The project is a homework of my Linux course, written in only one week.
So many codes may be confusing and lacking comments because of tight development.
I will keep maintaining it.

