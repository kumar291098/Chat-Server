#pragma once
#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <string>
#include <winsock2.h>

using namespace std;

void HandleClient(SOCKET clientSocket);

#endif // CLIENT_HANDLER_H