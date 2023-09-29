#include <sstream>
#include "Tcp.h"

trippin::Tcp::Tcp(const std::string &host, int port) : sock(nullptr) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1) {
        SDL_Log("resolve host error: %s", SDLNet_GetError());
        return;
    }

    // SDL_Log("resolved host, name=%s, ip=%s", host.c_str(), toString(ip.host).c_str());

    sock = SDLNet_TCP_Open(&ip);
    if (!sock) {
        SDL_Log("tcp open error: %s", SDLNet_GetError());
    }
}

trippin::Tcp::~Tcp() {
    if (sock) {
        SDLNet_TCP_Close(sock);
    }
}

TCPsocket trippin::Tcp::get() {
    return sock;
}

void trippin::Tcp::send(const std::string &request) const {
    int bytes = SDLNet_TCP_Send(sock, request.c_str(), request.size());
    if (bytes < request.size()) {
        SDL_Log("tcp send error: %s\n", SDLNet_GetError());
    }
}

std::string trippin::Tcp::receive(int maxSize) const {
    std::string response;
    int bytes;
    int total = 0;
    do {
        char buf[1'024];
        bytes = SDLNet_TCP_Recv(sock, buf, 1'024);
        if (bytes > 0) {
            response.append(buf, bytes);
            total += bytes;
        }
    } while (total < maxSize && bytes > 0);
    return response;
}

std::string trippin::Tcp::toString(Uint32 ip) {
    std::stringstream stream;
    for (int i = 0; i < 4; i++) {
        stream << (ip & 0xFF);
        if (i < 3)
            stream << ".";
        ip >>= 8;
    }
    return stream.str();
}
