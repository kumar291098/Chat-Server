#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>
#include <map>
#include <mutex>
#include <winsock2.h>

using namespace std;

// Declare the variables as extern
extern map<string, SOCKET> clients;
extern mutex clientsMutex;

void SendMessageToUser(const string& username, const string& message);

#endif // MESSAGE_HANDLER_H