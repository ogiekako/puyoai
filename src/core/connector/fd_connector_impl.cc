#include "core/connector/fd_connector_impl.h"

#include <glog/logging.h>
#include <unistd.h>

FdConnectorImpl::FdConnectorImpl(int reader_fd, int writer_fd) :
    reader_fd_(reader_fd),
    writer_fd_(writer_fd)
{
}

FdConnectorImpl::~FdConnectorImpl()
{
    if (close(reader_fd_) < 0) {
        PLOG(ERROR) << "failed to close: reader_fd_=" << reader_fd_;
    }
    if (close(writer_fd_) < 0) {
        PLOG(ERROR) << "failed to close: writer_fd_=" << writer_fd_;
    }
}

bool FdConnectorImpl::readExactly(void* buf, size_t size)
{
    while (size > 0) {
        ssize_t s = ::read(reader_fd_, buf, size);
        if (s < 0) {
            if (errno == EAGAIN)
                continue;
            PLOG(ERROR) << "failed to read: reader_fd_=" << reader_fd_;
            return false;
        }
        if (s == 0) {
            PLOG(ERROR) << "unexpected EOF: reader_fd_=" << reader_fd_;
            return false;
        }
        size -= s;
        buf = reinterpret_cast<char*>(buf) + s;
    }

    return true;
}

bool FdConnectorImpl::writeExactly(const void* buf, size_t size)
{
    while (size > 0) {
        ssize_t s = ::write(writer_fd_, buf, size);
        if (s < 0) {
            if (errno == EAGAIN)
                continue;
            PLOG(ERROR) << "failed to write: writer_fd_=" << writer_fd_;
            return false;
        }
        if (s == 0) {
            PLOG(ERROR) << "connection closed: writer_fd_=" << writer_fd_;
        }
        size -= s;
        buf = reinterpret_cast<const char*>(buf) + s;
    }

    return true;
}

void FdConnectorImpl::flush()
{
    // No need to flush here.
}
