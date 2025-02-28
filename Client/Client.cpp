#include "Client.hpp"
#include <iostream>

Client::Client(const std::string& serverIp, const int serverPort)
    : serverIp(serverIp), serverPort(serverPort), clientSocket(INVALID_SOCKET) {
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        throw std::runtime_error("WSAStartup failed");
    }

    clientSocket = createSocket();
    connectToServer();
}

Client::~Client() {
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }
    WSACleanup();
}

SOCKET Client::createSocket() {
    SOCKET socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        throw std::runtime_error("Socket creation failed");
    }
    return socket;
}

void Client::connectToServer() {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        std::cerr << "Failed to connect to server" << std::endl;
        throw std::runtime_error("Failed to connect to server");
    }

    std::cout << "Connected to server at " << serverIp << ":" << serverPort << std::endl;
}

std::string Client::receiveResponse() const {
    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Failed to receive data from server" << std::endl;
        throw std::runtime_error("Failed to receive data from server");
    }

    return std::string(buffer, bytesReceived);
}

bool Client::checkValidInput(const std::string& clientInput) const {
    if (clientInput.empty()) {
        return false;
    }

    for (char word : clientInput) {
        // ACSII symbols have codes from 0x00 to 0x7F in hex (demical 0-127). 
        // If symbol >= 0x80 (128) - refer to extended encodings.
        if (static_cast<unsigned char>(word) >= 0x80) { 
            return false;
        }

    }

    return true;
}

std::string Client::sendRequest(const std::string& input) {
    if (clientSocket == INVALID_SOCKET) {
        throw std::runtime_error("Socket is not initialized");
    }

    if (send(clientSocket, input.c_str(), input.size(), 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send data to server" << std::endl;
        throw std::runtime_error("Failed to send data to server");
    }

    return receiveResponse();
}

bool Client::testClient() {
    std::vector<std::pair<std::string, std::string>> testCases = {
        {"hello", "hhellllo"},
        {"world", "wworrlldd"},
        {"hpp", "hhpppp"},
        {"cpp", "ccpppp"},

        {"aeiou", "aeiou"},
        {"AEIOU", "AEIOU"},

        {"123!@#", "123!@#"},
        {"hello123", "hhellllo123"},

        {"hello world", "hhellllo wworrlldd"},
        {"  test  ", "  ttesstt  "},

        {"Hello World", "HHellllo WWorrlldd"},
        {"TEST", "TTESSTT"},

        {"hello, world!", "hhellllo, wworrlldd!"},
        {"test-case", "ttesstt-ccasse"}
    };

    for (const auto& testCase : testCases) {
        std::string response = sendRequest(testCase.first);
        if (response != testCase.second) {
            std::cerr << "Test failed for input: " << testCase.first << "\n";
            std::cerr << "Expected: " << testCase.second << ", got: " << response << "\n";
            return false;
        }
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return true;
}

void Client::runInteractiveMode() {
    while (true) {
        std::string input;
        std::cout << "Enter a message to send to the server (or 'exit' to quit): ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        if (!checkValidInput(input)) {
            std::cout << "Invalid input. Please enter a valid message using only Latin characters and basic punctuation." << std::endl;
            continue;
        }

        std::string response = sendRequest(input);
        std::cout << "Server response: " << response << std::endl;
    }
}