#ifndef NET_UNIX_DOMAIN_SOCKET_H_
#define NET_UNIX_DOMAIN_SOCKET_H_

#include "base/base.h"
#include "net/socket/socket.h"

namespace net {

class UnixDomainSocket : public Socket {
public:
    UnixDomainSocket(UnixDomainSocket&& socket) noexcept;
    ~UnixDomainSocket() override;

    UnixDomainSocket& operator=(UnixDomainSocket&& socket) noexcept;

private:
    explicit UnixDomainSocket(SocketDescriptor sd) : Socket(sd) {}

    friend class UnixDomainServerSocket;
    DISALLOW_COPY_AND_ASSIGN(UnixDomainSocket);
};

} // namespace net

#endif // NET_UNIX_DOMAIN_SOCKET_H_
