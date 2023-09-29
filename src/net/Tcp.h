#ifndef TRIPPIN_TCP_H
#define TRIPPIN_TCP_H

#include <string>
#include "SDL_net.h"

namespace trippin {
    class Tcp {
    private:
        TCPsocket sock;
        static std::string toString(Uint32 ip);
    public:
        Tcp(const std::string &host, int port);
        ~Tcp();
        void send(const std::string &request) const;
        std::string receive(int maxSize) const;
        TCPsocket get();
    };
}

#endif
