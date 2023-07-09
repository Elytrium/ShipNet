<img src="https://elytrium.net/src/img/elytrium.webp" alt="Elytrium" align="right">

# ShipNet

[![Join our Discord](https://img.shields.io/discord/775778822334709780.svg?logo=discord&label=Discord)](https://ely.su/discord)

C++ Network Library inspired by Netty.

## Why ShipNet?

- Zero sub-dependencies. This library uses OS sockets (e.g. epoll on Linux, kqueue on Mac/BSD) directly.
- Highly customizable. It's easy to add your own network stack support.
- Packet/Handler based. Just write your own packets and packet handlers. There's nothing else to be done.
- EventLoop based. Best multithreading strategy for networking. Use any count of threads you need.
