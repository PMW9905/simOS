# simOS
Simulated Operating System and PC that can run Assembly/Machine Code


## Overview
simOS is a Operating System and PC simulator written in C. It is broken into 2 main parts: the server and the client.

The server utilizes multithreading and semaphores to handle client requests/programs, manage memory, execute programs via the simulated cpu, and much more. To interact with clients, the server listens on a port. It has the capacity to listen to numberous clients all at once.

The client is how a person can interact with simOS. By running the client with arguments containing the server's ip and port, a user can connect to the server, submit code, recieve output from the server's console, and execute basic funcitons.

The framework of this program was given to me as a semester-long project in my senior OS class. Over the course of 3 months, I put extensive hours into turning the large skeleton we were given into a fully functioning program.

