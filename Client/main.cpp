#include "Client.hpp"

int main() {
    const std::string serverIp = "127.0.0.1";
    const int serverPort = 8081;
    Client client(serverIp, serverPort);
    client.runInteractiveMode();
    system("pause");

    if (client.testClient()) {
        return 0;
    }
    else {
        return 1;
    }
}
