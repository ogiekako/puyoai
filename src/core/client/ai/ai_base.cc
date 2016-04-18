#include "core/client/ai/ai_base.h"

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "base/strings.h"
#include "core/client/connector/socket_client_connector.h"
#include "core/client/connector/stdio_client_connector.h"
#include "net/socket/socket_factory.h"
#include "net/socket/unix_domain_client_socket.h"

DEFINE_string(connector, "stdio", "Choose stdio,unix,tcp");

// static
std::unique_ptr<ClientConnector> AIBase::makeConnector()
{
    if (FLAGS_connector == "stdio") {
        return std::unique_ptr<ClientConnector>(new StdioClientConnector);
    }

    if (strings::hasPrefix(FLAGS_connector, "unix:")) {
        std::string path = FLAGS_connector.substr(5);

        net::UnixDomainClientSocket socket(net::SocketFactory::instance()->makeUnixDomainClientSocket());
        CHECK(socket.connect(path.c_str()));
        return std::unique_ptr<ClientConnector>(new SocketClientConnector(std::move(socket)));
    }

    if (strings::hasPrefix(FLAGS_connector, "tcp:")) {
        std::string host_port = FLAGS_connector.substr(4);
        std::vector<std::string> parts = strings::split(host_port, ':');
        CHECK_EQ(parts.size(), 2U) << "tcp:<hostname>:<port> is expected, but " << FLAGS_connector;

        std::string host = parts[0];
        std::string port_str = parts[1];
        CHECK(!port_str.empty() && !host.empty()) << "tcp:<hostname>:<port> is expected, but " << FLAGS_connector;
        CHECK(strings::isAllDigits(port_str)) << "tcp:<hostname>:<port> is expected, but " << FLAGS_connector;

        int port = atoi(port_str.c_str());

        net::TCPClientSocket socket(net::SocketFactory::instance()->makeTCPClientSocket());
        CHECK(socket.setTCPNodelay());
        CHECK(socket.connect(host.c_str(), port));

        return std::unique_ptr<ClientConnector>(new SocketClientConnector(std::move(socket)));
    }

    CHECK(false) << "Unknown connector: " << FLAGS_connector;
    return std::unique_ptr<ClientConnector>();
}
