# Chat Server Application

A multi-user chat server application that supports private messaging, group creation, group management, and role-based permissions. Built using C++ with multi-threading and socket programming.

---

## Features

1. **User Authentication**:
   - Users can join the chat server by providing a unique username.

2. **Private Messaging**:
   - Users can send private messages to other users using the `@username message` command.

3. **Group Management**:
   - Users can create groups, join public groups, and manage group members.
   - Admins can add/remove members, promote/demote admins, and set group privacy.

4. **Role-Based Permissions**:
   - Admins have special privileges, such as adding/removing members, granting/revoking message permissions, and deleting groups.
   - Regular users can only send messages in groups if they have the required permissions.

5. **Group Privacy**:
   - Groups can be set as public or private.
   - Public groups allow any user to join, while private groups require an invitation from an admin.

6. **Message Permissions**:
   - Admins can grant or revoke message permissions for users in a group.

7. **Thread-Safe Design**:
   - The server uses mutexes to ensure thread safety when handling multiple clients and shared resources.

---

## Commands

### General Commands
- **`@username message`**: Send a private message to a user.
- **`/create_group groupname`**: Create a new private group (you become the admin).
- **`/join_group groupname`**: Join a public group.
- **`/group groupname message`**: Send a message to a group.

### Admin-Only Commands
- **`/add_member groupname user`**: Add a user to the group.
- **`/remove_member groupname user`**: Remove a user from the group.
- **`/make_admin groupname user`**: Promote a user to admin.
- **`/remove_admin groupname user`**: Remove a user from admin.
- **`/grant_message groupname user`**: Grant message permission to a user.
- **`/revoke_message groupname user`**: Revoke message permission from a user.
- **`/set_group_public groupname`**: Make the group public.
- **`/set_group_private groupname`**: Make the group private.
- **`/delete_group groupname`**: Delete a group.

---

## Setup Instructions

### Prerequisites
- **Compiler**: GCC or any C++ compiler that supports C++11 or later.
- **Libraries**: Winsock2 (for Windows) or equivalent socket libraries for other platforms.

### Steps to Run the Server
1. Clone the repository or download the source code.
2. Compile the server code:
   ```bash
   g++ main.cpp client_handler.cpp message_handler.cpp group_handler.cpp -o chat_server -lws2_32
