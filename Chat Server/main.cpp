#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include "client_handler.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsData;
    WSAStartup(MAKEWORD(2, 2), &wsData);
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed!" << endl;
        return -1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed!" << endl;
        return -1;
    }

    cout << "Server listening on port 54000" << endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed!" << endl;
            continue;
        }
        thread t(HandleClient, clientSocket);
        t.detach();
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}