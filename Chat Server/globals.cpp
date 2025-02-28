#include "message_handler.h"
#include "group_handler.h"

// Define the global variables
map<string, SOCKET> clients;
mutex clientsMutex;

map<string, vector<string>> groups;
map<string, set<string>> groupsAdmin;
map<string, bool> groupPrivacy;
map<string, map<string, bool>> groupMessagePermissions; // groupName -> (username -> hasPermission)
mutex groupsMutex, adminMutex, privacyMutex, messagePermissionsMutex;