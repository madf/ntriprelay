# NTRIP Relay

This is a relay than can operate both as a client and a server (source). It can take a stream of DGPS correction from one place and submit them into another.

## Building

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Usage

```
ntriprelay -M <source-mountpoint> -L <source-login> -W <source-password> -P <source-port> -S <source-server> -m <dest-mountpoint> -l <dest-login> -w <dest-password> -p <dest-port> -s <dest-server>
```
