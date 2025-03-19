#include "Client.hpp"
#include <iostream>

int main() {
    const std::string serverIp = "127.0.0.1";
    const int serverPort = 8081;

    try {
        Client client(serverIp, serverPort);
        client.runInteractiveMode();
        system("pause");

        if (client.testClient()) {
            return 0;
        }
        else {
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
}
