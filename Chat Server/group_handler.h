#ifndef GROUP_HANDLER_H
#define GROUP_HANDLER_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <mutex>

using namespace std;

// Declare the variables as extern
extern map<string, vector<string>> groups;
extern map<string, set<string>> groupsAdmin;
extern map<string, bool> groupPrivacy;
extern map<string, map<string, bool>> groupMessagePermissions; // New map for message permissions
extern mutex groupsMutex, adminMutex, privacyMutex, messagePermissionsMutex;

void CreateGroup(const string& groupName, const string& username);
void AddMemberToGroup(const string& groupName, const string& username, const string& newMember);
void MakeAdmin(const string& groupName, const string& username, const string& newAdmin);
void RemoveAdmin(const string& groupName, const string& username, const string& adminToRemove);
void RemoveMemberFromGroup(const string& groupName, const string& username, const string& memberToRemove);
void SetGroupPublic(const string& groupName, const string& username);
void SetGroupPrivate(const string& groupName, const string& username);
void JoinGroup(const string& groupName, const string& username);
void SendGroupMessage(const string& groupName, const string& username, const string& message);
void GrantMessagePermission(const string& groupName, const string& username, const string& targetUser);
void RevokeMessagePermission(const string& groupName, const string& username, const string& targetUser);
bool HasMessagePermission(const string& groupName, const string& username);
bool GroupExists(const string& groupName);

void DeleteGroup(const string& groupName, const string& username);
#endif // GROUP_HANDLER_H