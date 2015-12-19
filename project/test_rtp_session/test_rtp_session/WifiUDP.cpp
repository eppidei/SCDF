//
//  WifiUDP.cpp
//  RTPLib
//
//  Created by andrea scuderi on 29/11/15.
//  Copyright © 2015 fuzone. All rights reserved.
//

#include "UDPSender.h"
#include "WifiUDP.hpp"
//#include "WiFiClient.h"
//#include "WiFiServer.h"

appleMidi::_Serial Serial;
void appleMidi::_Serial::print (char*) {}
void appleMidi::_Serial::print (long) {}
void appleMidi::_Serial::print (long, int) {}
void appleMidi::_Serial::println (long, int) {}
void appleMidi::_Serial::println (char*) {}
void appleMidi::_Serial::println (long) {}
void appleMidi::_Serial::println () {}


size_t Print::write(const uint8_t *buffer, size_t size)
{
    size_t n = 0;
    while (size--) {
        if (write(*buffer++)) n++;
        else break;
    }
    return n;
}

appleMidi::IPAddress::IPAddress()
{
    _address.dword = 0;
}

appleMidi::IPAddress::IPAddress(const uint8_t *address)
{
    memcpy(_address.bytes, address, sizeof(_address.bytes));
}

/* Constructor */
WiFiUDP::WiFiUDP() {}//: _sock(NO_SOCKET_AVAIL) {}

/* Start WiFiUDP socket, listening at local port PORT */
uint8_t WiFiUDP::begin(uint16_t port)
{
    try {
        udpSocket.reset(new scdf::UDPSender());
        udpSocket->InitEndpoints(port, 1 , "192.168.1.69");
        udpSocket->Bind(0);
    }
    catch (const std::runtime_error& error)
    {
        LOGD("\nUDP SendTo failed with error: %s\n",error.what());
        return 0;
    }

//    uint8_t sock = WiFiClass::getSocket();
//    if (sock != NO_SOCKET_AVAIL)
//    {
//        ServerDrv::startServer(port, sock, UDP_MODE);
//        WiFiClass::_server_port[sock] = port;
//        _sock = sock;
//        _port = port;
//        return 1;
//    }
    return 1;

}

/* return number of bytes available in the current packet,
 will return zero if parsePacket hasn't been called yet */
int WiFiUDP::available() {
//    if (_sock != NO_SOCKET_AVAIL)
//    {
//        return ServerDrv::availData(_sock);
//    }

    return 1;
}

/* Release any resources being used by this WiFiUDP instance */
//void WiFiUDP::stop()
//{
//    if (_sock == NO_SOCKET_AVAIL)
//        return;
//
//    ServerDrv::stopClient(_sock);
//
//    _sock = NO_SOCKET_AVAIL;
//}

int WiFiUDP::beginPacket(const char *host, uint16_t port)
{
    // Look up the host first
    int ret = 0;
//    IPAddress remote_addr;
//    if (WiFi.hostByName(host, remote_addr))
//    {
//        return beginPacket(remote_addr, port);
//    }
    return ret;
}

int WiFiUDP::beginPacket(appleMidi::IPAddress ip, uint16_t port)
{
//    if (_sock == NO_SOCKET_AVAIL)
//        _sock = WiFiClass::getSocket();
//    if (_sock != NO_SOCKET_AVAIL)
//    {
//        ServerDrv::startClient(uint32_t(ip), port, _sock, UDP_MODE);
//        WiFiClass::_state[_sock] = _sock;
//        return 1;
//    }
    return 0;
}

int WiFiUDP::endPacket()
{
//    return ServerDrv::sendUdpData(_sock);
    return 0;
}

size_t WiFiUDP::write(uint8_t byte)
{
    return write(&byte, 1);
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size)
{
//    ServerDrv::insertDataBuf(_sock, buffer, size);
    return size;
}

int WiFiUDP::parsePacket()
{
    return available();
}

int WiFiUDP::read()
{
    uint8_t b;
    if (available())
    {
        char *pippo=(char*)malloc(256);
        udpSocket->Receive(pippo, 256, 0);
        free(pippo);
        return b;
    }else{
        return -1;
    }
}

int WiFiUDP::read(unsigned char* buffer, size_t len)
{
    if (available())
    {
        uint16_t size=0;
        try{
        size = udpSocket->Receive((char*)buffer, len, 0);
        }
        catch (...){
            return -1;
        }
//        if (!ServerDrv::getDataBuf(_sock, buffer, &size))
//            return -1;
        // TODO check if the buffer is too smal respect to buffer size
        return size;
    }else{
        return -1;
    }
}

//int WiFiUDP::peek()
//{
//    uint8_t b;
//    if (!available())
//        return -1;
//
//    ServerDrv::getData(_sock, &b, 1);
//    return b;
//}

void WiFiUDP::flush()
{
    // TODO: a real check to ensure transmission has been completed
}

appleMidi::IPAddress  WiFiUDP::remoteIP()
{
    uint8_t _remoteIp[4] = {0};
    uint8_t _remotePort[2] = {0};

//    WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
    appleMidi::IPAddress ip(_remoteIp);
    return ip;
}

uint16_t  WiFiUDP::remotePort()
{
    uint8_t _remoteIp[4] = {0};
    uint8_t _remotePort[2] = {0};

//    WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
    uint16_t port = (_remotePort[0]<<8)+_remotePort[1];
    return port;
}
