#include "LCDConnection.h"
#include "LCDException.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>

using namespace std;

LCDConnection::LCDConnection()
{
  _isConnected = false;
  memset(&_addr, 0, sizeof (_addr));
  _sock = socket (AF_INET, SOCK_STREAM, 0 );

  if (!isValid())
  {
    throw LCDException(LCD_SOCKET_CREATION_ERROR);   
  }

  int on = 1;
  if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof (on)) == -1)
  {
    throw LCDException(LCD_SOCKET_CREATION_ERROR);   
  }
}

LCDConnection::LCDConnection(const string &host, const int port)
{
  LCDConnection();
  connect(host, port);
}

LCDConnection::LCDConnection(const LCDConnection &source)
{
  _sock = source._sock;
  _isConnected = source._isConnected;
}

LCDConnection::~LCDConnection()
{
  disconnect();
}

bool LCDConnection::isValid() const
{
  return _sock != -1;
}

void LCDConnection::connect(const string &host, const int port)
{
  if (!isValid())
  {
    throw LCDException(LCD_SOCKET_CREATION_ERROR);
  }

  _addr.sin_family = AF_INET;
  _addr.sin_port = htons(port);

  int status = inet_pton(AF_INET, host.c_str(), &_addr.sin_addr);

  if (errno == EAFNOSUPPORT)
  {
    throw LCDException(LCD_SOCKET_NOT_SUPPORTED);
  }

  status = ::connect(_sock, (sockaddr *)&_addr, sizeof(_addr) );

  if (status != 0)
  {
    throw LCDException(LCD_SOCKET_CONNECTION_ERROR);
  }

  _isConnected = true;
}

void LCDConnection::disconnect()
{
  if (isValid())
  {
    ::close(_sock);
  }
}

void LCDConnection::send(const string &toSend)
{
  string s = toSend + "\r\n";

  cerr << "Sending : " << s << endl;

  if (!_isConnected)
  {
    throw LCDException(LCD_SOCKET_NOT_CONNECTED);
  }

  int total = s.size();
  int offset = 0;
  int sent;

  while (offset != total)
  {
    sent = ::send(_sock, s.c_str() + offset, s.size() - offset, 0);
    if ( ((sent == -1) && (errno != EAGAIN)) || (sent == 0) )
    {
      throw LCDException(LCD_SOCKET_SEND_ERROR);
    }
    else
    {
      offset += sent;
    }
  }
}

string LCDConnection::recv()
{
  if (!_isConnected)
  {
    throw LCDException(LCD_SOCKET_NOT_CONNECTED);
  }

  char buf[LCD_MAXRECV + 1];
  memset (buf, 0, LCD_MAXRECV + 1);
  char *current = buf - 1;
  int status;

  do
  {
    current++;
    status = ::recv(_sock, current, 1, 0);
    if (status == -1)
    {
      throw LCDException(LCD_SOCKET_RECV_ERROR);
    }
  } while ( (*current != '\0' ) && (*current != '\r' ) && (*current != '\n' ) && ((current - buf) < LCD_MAXRECV) );

  *current = '\0';

  string result(buf);

  cerr << "Receiving : " << result << endl;

  return result;
}

LCDConnection & LCDConnection::operator = (const LCDConnection &copy)
{
  disconnect();

  _sock = copy._sock;
  _isConnected = copy._isConnected;
}

const LCDConnection& LCDConnection::operator << (const string &s)
{
  send(s);
  return *this;
}

const LCDConnection& LCDConnection::operator >> (string &s)
{
  s = recv();
  return *this;
}

