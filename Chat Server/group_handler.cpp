#include "group_handler.h"
#include "message_handler.h"
#include <algorithm>

using namespace std;
bool GroupExists(const string& groupName) {
    lock_guard<mutex> groupsLock(groupsMutex);
    return groups.find(groupName) != groups.end();
}
void CreateGroup(const string& groupName, const string& username) {

    lock_guard<mutex> lock(groupsMutex);
    if (groups.find(groupName) == groups.end()) {
        groups[groupName] = { username };
        {
            lock_guard<mutex> adminLock(adminMutex);
            groupsAdmin[groupName].insert(username);
        }
        {
            lock_guard<mutex> privacyLock(privacyMutex);
            groupPrivacy[groupName] = false;
        }
        {
            lock_guard<mutex> permissionLock(messagePermissionsMutex);
            groupMessagePermissions[groupName][username] = true; // Admin has permission by default
        }
        SendMessageToUser(username, "Group '" + groupName + "' created successfully. You are the admin.");
    }
    else {
        SendMessageToUser(username, "Group '" + groupName + "' already exists.");
    }
}

void AddMemberToGroup(const string& groupName, const string& username, const string& newMember) {
    
    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> groupsLock(groupsMutex);

        // Check if the new member is already in the group
        auto& members = groups[groupName];
        if (find(members.begin(), members.end(), newMember) != members.end()) {
            SendMessageToUser(username, "User '" + newMember + "' is already a member of group '" + groupName + "'.");
            return;
        }
		// Add the new member to the group
        groups[groupName].push_back(newMember);
        {
            lock_guard<mutex> permissionLock(messagePermissionsMutex);
            groupMessagePermissions[groupName][newMember] = true; // New member has permission by default
        }
        SendMessageToUser(newMember, "You have been added to group '" + groupName + "' by admin " + username + ".");
        SendMessageToUser(username, "User '" + newMember + "' has been added to group '" + groupName + "'.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

void GrantMessagePermission(const string& groupName, const string& username, const string& targetUser) {
    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }
    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> permissionLock(messagePermissionsMutex);

        // Check if the target user is a member of the group
        auto& members = groups[groupName];
        if (find(members.begin(), members.end(), targetUser) == members.end()) {
            SendMessageToUser(username, "User '" + targetUser + "' is not a member of the group '" + groupName + "'.");
            return;
        }

        // Check if the target user already has message permission
        if (groupMessagePermissions[groupName][targetUser]) {
            SendMessageToUser(username, "User '" + targetUser + "' already has message permission in group '" + groupName + "'.");
            return;
        }
        // Grant message permission
        groupMessagePermissions[groupName][targetUser] = true;
        SendMessageToUser(targetUser, "You have been granted message permission in group '" + groupName + "' by admin " + username + ".");
        SendMessageToUser(username, "Message permission granted to user '" + targetUser + "' in group '" + groupName + "'.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

void RevokeMessagePermission(const string& groupName, const string& username, const string& targetUser) {
    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }
    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> permissionLock(messagePermissionsMutex);
        // Check if the target user is a member of the group
        auto& members = groups[groupName];
        if (find(members.begin(), members.end(), targetUser) == members.end()) {
            SendMessageToUser(username, "User '" + targetUser + "' is not a member of the group '" + groupName + "'.");
            return;
        }

        // Check if the target user already has message permission revoked
        if (!groupMessagePermissions[groupName][targetUser]) {
            SendMessageToUser(username, "User '" + targetUser + "' already does not have message permission in group '" + groupName + "'.");
            return;
        }

        // Revoke message permission
        groupMessagePermissions[groupName][targetUser] = false;
        SendMessageToUser(targetUser, "Your message permission in group '" + groupName + "' has been revoked by admin " + username + ".");
        SendMessageToUser(username, "Message permission revoked for user '" + targetUser + "' in group '" + groupName + "'.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

bool HasMessagePermission(const string& groupName, const string& username) {
    lock_guard<mutex> permissionLock(messagePermissionsMutex);
    return groupMessagePermissions[groupName][username];
}

void MakeAdmin(const string& groupName, const string& username, const string& newAdmin) {
    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }
    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> groupsLock(groupsMutex);

        // Check if the new admin is already a member of the group
        auto& members = groups[groupName];
        auto it = find(members.begin(), members.end(), newAdmin);

        if (it == members.end()) {
            // If the user is not a member, add them to the group first
            members.push_back(newAdmin);
            SendMessageToUser(newAdmin, "You have been added to group '" + groupName + "' by admin " + username + ".");
        }
        // Check if the user is already an admin
        if (groupsAdmin[groupName].count(newAdmin)) {
            SendMessageToUser(username, "User '" + newAdmin + "' is already an admin of group '" + groupName + "'.");
            return;
        }
        // Promote the user to admin
        groupsAdmin[groupName].insert(newAdmin);
        SendMessageToUser(newAdmin, "You have been promoted to admin of group '" + groupName + "' by " + username + ".");
        SendMessageToUser(username, "User '" + newAdmin + "' has been promoted to admin of group '" + groupName + "'.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}
void RemoveAdmin(const string& groupName, const string& username, const string& adminToRemove) {
    
	if (!GroupExists(groupName)) {
		SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
		return;
	}

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        if (username == adminToRemove) {
            SendMessageToUser(username, "You cannot remove yourself as an admin.");
            return;
        }
        if (groupsAdmin[groupName].erase(adminToRemove)) {
            SendMessageToUser(adminToRemove, "You have been removed as an admin of group '" + groupName + "' by " + username + ".");
        }
        else {
            SendMessageToUser(username, "User '" + adminToRemove + "' is not an admin of this group.");
        }
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

void RemoveMemberFromGroup(const string& groupName, const string& username, const string& memberToRemove) {
	if (!GroupExists(groupName)) {
		SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
		return;
	}

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> groupsLock(groupsMutex);
        auto& members = groups[groupName];
        auto it = find(members.begin(), members.end(), memberToRemove);
        if (it != members.end()) {
            members.erase(it);
            SendMessageToUser(memberToRemove, "You have been removed from group '" + groupName + "' by admin " + username + ".");
        }
        else {
            SendMessageToUser(username, "User '" + memberToRemove + "' is not a member of this group.");
        }
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}
void SetGroupPublic(const string& groupName, const string& username) {

    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {

        lock_guard<mutex> privacyLock(privacyMutex);

        // Check if the group is already public
        if (groupPrivacy[groupName]) {
            SendMessageToUser(username, "The group '" + groupName + "' is already public.");
            return;
        }
        // Set the group to public
        groupPrivacy[groupName] = true;
        SendMessageToUser(username, "Group '" + groupName + "' is now public.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

void SetGroupPrivate(const string& groupName, const string& username) {

	if (!GroupExists(groupName)) {
		SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
		return;
	}

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> privacyLock(privacyMutex);
        // Check if the group is already private
        if (!groupPrivacy[groupName]) {
            SendMessageToUser(username, "The group '" + groupName + "' is already private.");
            return;
        }

        // Set the group to private
        groupPrivacy[groupName] = false; // Set to private
        SendMessageToUser(username, "Group '" + groupName + "' is now private.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}

void JoinGroup(const string& groupName, const string& username) {
    lock_guard<mutex> privacyLock(privacyMutex);
    

    // Check if the group exists
   if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }

    // Check if the user is already a member of the group
    auto& members = groups[groupName];
    if (find(members.begin(), members.end(), username) != members.end()) {
        SendMessageToUser(username, "You are already a member of the group '" + groupName + "'.");
        return;
    }

    // Check if the group is public
    if (!groupPrivacy[groupName]) {
        SendMessageToUser(username, "The group '" + groupName + "' is private. You need an invitation from the group admin to join.");
        return;
    }

    // Add the user to the group
    members.push_back(username);
    SendMessageToUser(username, "You have successfully joined the public group '" + groupName + "'.");
}

void SendGroupMessage(const string& groupName, const string& username, const string& message) {
    lock_guard<mutex> groupsLock(groupsMutex);
    if (groups.find(groupName) != groups.end()) { // Check if group exists
        if (find(groups[groupName].begin(), groups[groupName].end(), username) != groups[groupName].end()) { // Check if sender is in the group
            if (HasMessagePermission(groupName, username)) {

                for (const string& member : groups[groupName]) {
                    if (member != username) {
                        SendMessageToUser(member, "[" + groupName + "] " + username + ": " + message);
                    }
                }
            }
            else {
                SendMessageToUser(username, "You do not have permission to send messages in group '" + groupName + "'.");
            }
        }
        else {
            SendMessageToUser(username, "You are not a member of the group '" + groupName + "'.");
        }
    }
    else {
        SendMessageToUser(username, "Group '" + groupName + "' does not exist.");
    }
}
void DeleteGroup(const string& groupName, const string& username) {
    if (!GroupExists(groupName)) {
        SendMessageToUser(username, "The group '" + groupName + "' does not exist. Please check the group name and try again.");
        return;
    }

    lock_guard<mutex> adminLock(adminMutex);
    if (groupsAdmin[groupName].count(username)) {
        lock_guard<mutex> groupsLock(groupsMutex);

        // Notify all members that the group is being deleted
        auto& members = groups[groupName];
        for (const string& member : members) {
            SendMessageToUser(member, "Group '" + groupName + "' has been deleted by admin " + username + ".");
        }

        // Remove the group from all data structures
        groups.erase(groupName);
        groupsAdmin.erase(groupName);
        groupPrivacy.erase(groupName);
        groupMessagePermissions.erase(groupName);

        SendMessageToUser(username, "Group '" + groupName + "' has been successfully deleted.");
    }
    else {
        SendMessageToUser(username, "You must be an admin of the group to perform this action. Please contact the group admin for assistance.");
    }
}
