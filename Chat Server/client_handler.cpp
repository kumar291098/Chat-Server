#include "client_handler.h"
#include "message_handler.h"
#include "group_handler.h"
#include <iostream>
#include <sstream>
#include <exception>

using namespace std;

void HandleClient(SOCKET clientSocket) {
    char buffer[4096];
    string username;

    try {
        int recvLength = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvLength > 0) {
            buffer[recvLength] = '\0';
            username = string(buffer);
            {
                lock_guard<mutex> lock(clientsMutex);
                clients[username] = clientSocket;
            }
            cout << username << " joined the chat." << endl;
            // Send instructions to the user
            string instructions = "\n--- Chat Commands ---\n";
            instructions += "@username message  -> Send a private message\n";
            instructions += "/create_group groupname  -> Create a private group\n";
            instructions += "/set_group_public groupname  -> Make group public\n";
            instructions += "/set_group_private groupname  -> Make group private\n";
            instructions += "/join_group groupname  -> Join a public group\n";
            instructions += "/add_member groupname user  -> Add a member (Admin only)\n";
            instructions += "/remove_member groupname user  -> Remove a member from the group (Admin only)\n";
            instructions += "/make_admin groupname user  -> Promote user to admin (Admin only)\n";
            instructions += "/remove_admin groupname user  -> Remove user from admin (Admin only)\n";
            instructions += "/grant_message groupname user  -> Grant message permission to a user (Admin only)\n";
            instructions += "/revoke_message groupname user  -> Revoke message permission from a user (Admin only)\n";
            instructions += "/group groupname message  -> Send message to group\n";
            instructions += "/delete_group groupname  -> Delete a group (Admin only)\n";
            SendMessageToUser(username, instructions);
        }
        else {
            closesocket(clientSocket);
            return;
        }

        while (true) {
            recvLength = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (recvLength <= 0) {
                cout << username << " disconnected." << endl;
                {
                    lock_guard<mutex> lock(clientsMutex);
                    clients.erase(username);
                }
                closesocket(clientSocket);
                return;
            }

            buffer[recvLength] = '\0';
            string msg(buffer);
            if (msg.empty()) continue;

            cout << username << ": " << msg << endl;

            if (msg[0] == '@') {
                size_t spacePos = msg.find(' ');
                if (spacePos != string::npos) {
                    string targetUser = msg.substr(1, spacePos - 1);
                    string privateMessage = msg.substr(spacePos + 1);
                    SendMessageToUser(targetUser, username + " (private): " + privateMessage);
                }
            }
            else if (msg.rfind("/create_group ", 0) == 0) {
                string groupName = msg.substr(14);
                CreateGroup(groupName, username);
            }
            else if (msg.rfind("/add_member ", 0) == 0) {
                istringstream iss(msg.substr(12));
                string groupName, newMember;
                iss >> groupName >> newMember;
                AddMemberToGroup(groupName, username, newMember);
            }
            else if (msg.rfind("/remove_member ", 0) == 0) {
                istringstream iss(msg.substr(15));
                string groupName, memberToRemove;
                iss >> groupName >> memberToRemove;
                RemoveMemberFromGroup(groupName, username, memberToRemove);
            }
            else if (msg.rfind("/make_admin ", 0) == 0) {
                istringstream iss(msg.substr(12));
                string groupName, newAdmin;
                iss >> groupName >> newAdmin;
                MakeAdmin(groupName, username, newAdmin);
            }
            else if (msg.rfind("/remove_admin ", 0) == 0) {
                istringstream iss(msg.substr(14));
                string groupName, adminToRemove;
                iss >> groupName >> adminToRemove;
                RemoveAdmin(groupName, username, adminToRemove);
            }
            else if (msg.rfind("/set_group_public ", 0) == 0) {
                string groupName = msg.substr(18);
                SetGroupPublic(groupName, username);
            }
            else if (msg.rfind("/set_group_private ", 0) == 0) {
                string groupName = msg.substr(19);
                SetGroupPrivate(groupName, username);
            }
            else if (msg.rfind("/join_group ", 0) == 0) {
                string groupName = msg.substr(12);
                JoinGroup(groupName, username);
            }
            else if (msg.rfind("/grant_message ", 0) == 0) {
                istringstream iss(msg.substr(15));
                string groupName, targetUser;
                iss >> groupName >> targetUser;
                GrantMessagePermission(groupName, username, targetUser);
            }
            else if (msg.rfind("/revoke_message ", 0) == 0) {
                istringstream iss(msg.substr(16));
                string groupName, targetUser;
                iss >> groupName >> targetUser;
                RevokeMessagePermission(groupName, username, targetUser);
            }
            else if (msg.rfind("/group ", 0) == 0) {
                istringstream iss(msg.substr(7));
                string groupName;
                iss >> groupName;
                string groupMessage;
                getline(iss, groupMessage);
                SendGroupMessage(groupName, username, groupMessage);
            }
			else if (msg.rfind("/delete_group ", 0) == 0) {
				string groupName = msg.substr(14);
				DeleteGroup(groupName, username);
			}
			else {
				SendMessageToUser(username, "Invalid command. Please check the instructions and try again.");
			}
        }
    }
    catch (const exception& e) {
        cerr << "Exception in client handler: " << e.what() << endl;
    }
    closesocket(clientSocket);
}