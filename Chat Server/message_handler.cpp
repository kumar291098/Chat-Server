#include "message_handler.h"

using namespace std;

void SendMessageToUser(const string& username, const string& message) {
    lock_guard<mutex> lock(clientsMutex);
    if (clients.find(username) != clients.end()) {
        send(clients[username], message.c_str(), message.size(), 0);
    }
}