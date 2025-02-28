// included guard
#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class Client {
private:
    std::string serverIp;
    int serverPort;
    WSAData wsadata;
    SOCKET clientSocket;

    [[nodiscard]] SOCKET createSocket();
    void connectToServer();

    [[nodiscard]] std::string receiveResponse() const;
    [[nodiscard]] std::string sendRequest(const std::string& input);

    [[nodiscard]] bool checkValidInput(const std::string& clientInput) const;

public:
    Client(const std::string& serverIp, const int serverPort);
    ~Client();

    [[nodiscard]] bool testClient();

    void runInteractiveMode();
};

#endif // CLIENT_H