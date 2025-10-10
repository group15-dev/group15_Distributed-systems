# Implementation plan
---

### overview 

- client side - 1 ui
 - server side - 2 nodes

 - comms protocol - TCP socket(not web socket)

 thought of using C because of it's efficiency, portability, and low-level control.

- client - xml for ui

- server - socket, SOCK_STREAM.

- chat rooms, users can join same chat rooms on different servers that talk to each other


- socket creation , listenig , connectio - Done


### next steps
---
1. define the ideas
- chat app where several clients can connect to several nodes that communicaste via SOCK_STREAM.
2. Implement ide at system level code

- serve node A - 9001
- sever node B - 9002
- internal routing between server nodes
- chat rooms for clients top join
---
>>> ./username chatroom message

- client app - XML UI




