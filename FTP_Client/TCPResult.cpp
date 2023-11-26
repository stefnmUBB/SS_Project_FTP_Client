#include "TCPResult.h"

#include <winsock.h>
#include "tcp_exception.h"


bufferf TCPResult::get_error_message()
{    
    wchar_t msgbuf[256];
    msgbuf[0] = '\0';

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        msgbuf,
        sizeof(msgbuf),
        NULL);

    return bufferf("Socket error %i: %S", error_code, msgbuf);
}

void TCPResult::validate_send(size_t desired_size)
{
    if (!ok)
        throw tcp_exception(get_error_message());
    if (bytes_count != desired_size)
        throw tcp_exception(bufferf("Not all bytes were sent (%i/%i)", bytes_count, desired_size));
}

void TCPResult::validate_recv(size_t desired_size)
{
    if (!ok)
        throw tcp_exception(get_error_message());
    if (bytes_count == 0)
        throw tcp_exception(bufferf("Connection interrupted during recv"));
    if (bytes_count != desired_size)
        throw tcp_exception(bufferf("Not all bytes were received (%i/%i)", bytes_count, desired_size));
}