#ifndef CORE_CONNECTOR_FD_CONNECTOR_IMPL_H_
#define CORE_CONNECTOR_FD_CONNECTOR_IMPL_H_

#include <cstddef>

#include "core/connector/connector_impl.h"

// FdConnectorImpl takes 2 file descriptors (to read and to write), and use them.
// file descriptors will be closed when the instance is being destructed.
class FdConnectorImpl : public ConnectorImpl {
public:
    FdConnectorImpl(int reader_fd, int writer_fd);
    ~FdConnectorImpl() override;

    bool readExactly(void* buf, size_t size) override;
    bool writeExactly(const void* buf, size_t size) override;
    void flush() override;

private:
    int reader_fd_;
    int writer_fd_;
};

#endif // CORE_CONNECTOR_STDIO_CONNECTOR_IMPL_H_
