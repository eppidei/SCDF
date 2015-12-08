//
//  WifiUDP.hpp
//  RTPLib
//
//  Created by andrea scuderi on 29/11/15.
//  Copyright © 2015 fuzone. All rights reserved.
//

#ifndef WifiUDP_hpp
#define WifiUDP_hpp

#include "AppleMidi_Settings.h"
#include "AppleMidi_Defs.h"

//#include <inttypes.h>
//#include <stdio.h> // for size_t
//
//#include "WString.h"
//#include "Printable.h"

class Print
{
private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);
protected:
    void setWriteError(int err = 1) { write_error = err; }
public:
    Print() : write_error(0) {}
    
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
    
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
        if (str == NULL) return 0;
        return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
        return write((const uint8_t *)buffer, size);
    }
    
//    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
//    size_t print(const Printable&);
    
//    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
//    size_t println(const Printable&);
    size_t println(void);
};

enum LookaheadMode{
    SKIP_ALL,       // All invalid characters are ignored.
    SKIP_NONE,      // Nothing is skipped, and the stream is not touched unless the first waiting character is valid.
    SKIP_WHITESPACE // Only tabs, spaces, line feeds & carriage returns are skipped.
};

#define NO_IGNORE_CHAR  '\x01' // a char not found in a valid ASCII numeric field

class Stream : public Print
{
protected:
    unsigned long _timeout;      // number of milliseconds to wait for the next char before aborting timed read
    unsigned long _startMillis;  // used for timeout measurement
    int timedRead();    // private method to read stream with timeout
    int timedPeek();    // private method to peek stream with timeout
    int peekNextDigit(LookaheadMode lookahead, bool detectDecimal); // returns the next numeric digit in the stream or -1 if timeout
    
public:
    virtual int available() = 0;
    virtual int read() = 0;
//    virtual int peek() = 0;
    virtual void flush() = 0;
    
    Stream() {_timeout=1000;}
    
    // parsing methods
    
    void setTimeout(unsigned long timeout);  // sets maximum milliseconds to wait for stream data, default is 1 second
    
    bool find(char *target);   // reads data from the stream until the target string is found
    bool find(uint8_t *target) { return find ((char *)target); }
    // returns true if target string is found, false if timed out (see setTimeout)
    
    bool find(char *target, size_t length);   // reads data from the stream until the target string of given length is found
    bool find(uint8_t *target, size_t length) { return find ((char *)target, length); }
    // returns true if target string is found, false if timed out
    
    bool find(char target) { return find (&target, 1); }
    
    bool findUntil(char *target, char *terminator);   // as find but search ends if the terminator string is found
    bool findUntil(uint8_t *target, char *terminator) { return findUntil((char *)target, terminator); }
    
    bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);   // as above but search ends if the terminate string is found
    bool findUntil(uint8_t *target, size_t targetLen, char *terminate, size_t termLen) {return findUntil((char *)target, targetLen, terminate, termLen); }
    
    long parseInt(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);
    // returns the first valid (long) integer value from the current position.
    // lookahead determines how parseInt looks ahead in the stream.
    // See LookaheadMode enumeration at the top of the file.
    // Lookahead is terminated by the first character that is not a valid part of an integer.
    // Once parsing commences, 'ignore' will be skipped in the stream.
    
    float parseFloat(LookaheadMode lookahead = SKIP_ALL, char ignore = NO_IGNORE_CHAR);
    // float version of parseInt
    
    size_t readBytes( char *buffer, size_t length); // read chars from stream into buffer
    size_t readBytes( uint8_t *buffer, size_t length) { return readBytes((char *)buffer, length); }
    // terminates if length characters have been read or timeout (see setTimeout)
    // returns the number of characters placed in the buffer (0 means no valid data found)
    
    size_t readBytesUntil( char terminator, char *buffer, size_t length); // as readBytes with terminator character
    size_t readBytesUntil( char terminator, uint8_t *buffer, size_t length) { return readBytesUntil(terminator, (char *)buffer, length); }
    // terminates if length characters have been read, timeout, or if the terminator character  detected
    // returns the number of characters placed in the buffer (0 means no valid data found)
    
    // Arduino String functions to be added here
    String readString();
    String readStringUntil(char terminator);
    
protected:
    long parseInt(char ignore) { return parseInt(SKIP_ALL, ignore); }
    float parseFloat(char ignore) { return parseFloat(SKIP_ALL, ignore); }
    // These overload exists for compatibility with any class that has derived
    // Stream and used parseFloat/Int with a custom ignore character. To keep
    // the public API simple, these overload remains protected.
    
    struct MultiTarget {
        const char *str;  // string you're searching for
        size_t len;       // length of string you're searching for
        size_t index;     // index used by the search routine.
    };
    
    // This allows you to search for an arbitrary number of strings.
    // Returns index of the target that is found first or -1 if timeout occurs.
    int findMulti(struct MultiTarget *targets, int tCount);
};

class UDP : public Stream {
    
public:
    virtual uint8_t begin(uint16_t) =0;	// initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
//    virtual void stop() =0;  // Finish with the UDP socket
    
    // Sending UDP packets
    
    // Start building up a packet to send to the remote host specific in ip and port
    // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
    virtual int beginPacket(appleMidi::IPAddress ip, uint16_t port) =0;
    // Start building up a packet to send to the remote host specific in host and port
    // Returns 1 if successful, 0 if there was a problem resolving the hostname or port
    virtual int beginPacket(const char *host, uint16_t port) =0;
    // Finish off this packet and send it
    // Returns 1 if the packet was sent successfully, 0 if there was an error
    virtual int endPacket() =0;
    // Write a single byte into the packet
    virtual size_t write(uint8_t) =0;
    // Write size bytes from buffer into the packet
    virtual size_t write(const uint8_t *buffer, size_t size) =0;
    
    // Start processing the next available incoming packet
    // Returns the size of the packet in bytes, or 0 if no packets are available
//    virtual int parsePacket() =0;
    // Number of bytes remaining in the current packet
//    virtual int available() =0;
    // Read a single byte from the current packet
    virtual int read() =0;
    // Read up to len bytes from the current packet and place them into buffer
    // Returns the number of bytes read, or 0 if none are available
    virtual int read(unsigned char* buffer, size_t len) =0;
    // Read up to len characters from the current packet and place them into buffer
    // Returns the number of characters read, or 0 if none are available
    virtual int read(char* buffer, size_t len) =0;
    // Return the next byte from the current packet without moving on to the next byte
//    virtual int peek() =0;
    virtual void flush() =0;	// Finish reading the current packet
    
    // Return the IP address of the host who sent the current incoming packet
    virtual appleMidi::IPAddress remoteIP() =0;
    // Return the port of the host who sent the current incoming packet
    virtual uint16_t remotePort() =0;
protected:
//    uint8_t* rawIPAddress(appleMidi::IPAddress& addr) { return addr.raw_address(); };
};

#define UDP_TX_PACKET_MAX_SIZE 24
namespace scdf{
    class UDPSender;
}
class WiFiUDP : public UDP {
private:
    std::unique_ptr<scdf::UDPSender> udpSocket;
//    uint8_t _sock;  // socket ID for Wiz5100
//    uint16_t _port; // local port to listen on
    
public:
    WiFiUDP();  // Constructor
    virtual uint8_t begin(uint16_t);	// initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
//    virtual void stop();  // Finish with the UDP socket
    
    // Sending UDP packets
    
    // Start building up a packet to send to the remote host specific in ip and port
    // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
    virtual int beginPacket(appleMidi::IPAddress ip, uint16_t port);
    // Start building up a packet to send to the remote host specific in host and port
    // Returns 1 if successful, 0 if there was a problem resolving the hostname or port
    virtual int beginPacket(const char *host, uint16_t port);
    // Finish off this packet and send it
    // Returns 1 if the packet was sent successfully, 0 if there was an error
    virtual int endPacket();
    // Write a single byte into the packet
    virtual size_t write(uint8_t);
    // Write size bytes from buffer into the packet
    virtual size_t write(const uint8_t *buffer, size_t size);
    
    using Print::write;
    
    // Start processing the next available incoming packet
    // Returns the size of the packet in bytes, or 0 if no packets are available
    virtual int parsePacket();
    // Number of bytes remaining in the current packet
    virtual int available();
    // Read a single byte from the current packet
    virtual int read();
    // Read up to len bytes from the current packet and place them into buffer
    // Returns the number of bytes read, or 0 if none are available
    virtual int read(unsigned char* buffer, size_t len);
    // Read up to len characters from the current packet and place them into buffer
    // Returns the number of characters read, or 0 if none are available
    virtual int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
    // Return the next byte from the current packet without moving on to the next byte
//    virtual int peek();
    virtual void flush();	// Finish reading the current packet
    
    // Return the IP address of the host who sent the current incoming packet
    virtual appleMidi::IPAddress remoteIP();
    // Return the port of the host who sent the current incoming packet
    virtual uint16_t remotePort();
    
//    friend class WiFiDrv;
};
#endif /* WifiUDP_hpp */
