# simOS
Simulated Operating System and PC that can run Assembly/Machine Code


## Overview
simOS is a Operating System and PC simulator written in C. It is broken into 2 main parts: the server and the client.

The server utilizes multithreading and semaphores to handle client requests/programs, manage memory, execute programs via the simulated cpu, and much more. To interact with clients, the server listens on a port. It has the capacity to listen to numberous clients all at once.

The client is how a person can interact with simOS. By running the client with arguments containing the server's ip and port, a user can connect to the server, submit code, recieve output from the server's console, and execute basic funcitons.

The framework of this program was given to me as a semester-long project in my senior OS class. Over the course of 3 months, I put extensive hours into turning the large skeleton we were given into a fully functioning program.

# Individual File Descriptions / My Contributions

## Server .c Files

### Admin.c:

Recieves and processes commands sent by the client via the AdminUI. 

I modified this file so that it would write all output to the writePipe, which is sent back to the client. Additionally, I created a signal catcher that sets an interrupt for admin commands. This allows the user to send admin commands while other programs are running in the background. Finally, I altered some of the funcitonallity of one_admin_command so that it would no longer take single executions or run machine code. This is instead handled by adminUI.c

### AdminUI.c

Contains the main method of simOS and is what is innitally run when ./admin.exe is run. Forks a child process. The parent reads admin commands, sends signals to simOS that a message has been sent, sends pipe information to simOS, waits for a response, and then prints out response. This loops until simOS exits. The child process is what runs simOS. It initally creates communication pipes between parent and child, and then begins to initialize simOS. When simOS closes, pipes are closed.

I programmed the entirety of this file.

### clock.c

Manages and schuedles all of simOS, from the number of CPU cycles, to scheduling interuptions, etc. Events are stored in eventNode structs. Allows for simOS to handle multiple programs at once (it switches back and forth between them based on a value known as timeQuantum).

I did not contribute to this file, but its impact allowed me to shape code in other places.

### cpu.c

Simulates the funcitonality of a literal cpu. Fetches instructions from user's code, processes instructions, executes instrucitons, and handles interuptions. 

List of instructions: load, add, mul, ifgo, store, print, sleep, exit.

I programmed the handle_interrupt function, the execute_instruction function, and altered the fetch_instruction to properly handle "goto" instructions.

### idle.c

Simply allows simOS to idle when there are no processes. 

I did not contribute to this file.

### memory.c

Manages the memory of simOS, storing user files in pages.

I did not contribute to this file.

### process.c

Manages the process flow of simOS. Stores user processes, loads in/out processes to allow for multiprocessing, 
initializes and manages PCB, initializes process manager, and more.

Often, calls to this file will be from separate threads that the main child process (multiple users consoles requiring output/interaction). To manage this, I employed a semaphore solution using sem_t. It is very reminiscent of the baker's problem.

### submit.c

Manages the connection between server and client. Opens and listens on a port for client interaction via the select() function. Is able to manage multiple clients at once. Uses a queue data structure to manage client submissions called cNode, which I created. 

I created the entirety of this file.

### system.c

Manages the system startup, initialization, and exit. Is run by the child process in AdminUI.

I redesigned it to be properly configured with AdminUI, and redirected pipes so that output would be sent back to the client console and not the server console. 

## Server .o files 

While I did not contribute to these, I utilized their functions whilst programming the rest of simOS.

### loader.o 

Handles the loading of memory.

### paging.o

Handles the page-structured memory format.

### swap.o

Handles the swapping of memory. Utilizes a queue data structure.

## Server .h file 

### simos.h

Contains the majority of all function headers, and is included by all server .c files. 

I contributed various functions and data structures to this file as I programmed. Most can be found at the bottom.

## Client Files

### client.c 

Compiles into client.exe, which allows users to directly connect to the simOS server via IP and port. Client.c attempts to connect to the server. Upon success, the user is then able to communicate directly to the simOS console, sending files, writing commands, etc. Simos can handle multiple client connections at a time, and will keep track of who sent what, providing accurate output via the pipe connection. Whenever they wish, the user may terminate the connection via "T." 

I programmed the entirety of this.
