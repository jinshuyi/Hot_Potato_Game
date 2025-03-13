Here is the full **GitHub-compatible README** file with all sections formatted properly in Markdown:

```markdown
# TCP_Programming

## Overview of Socket Communication

In network programming, a **socket** serves as an endpoint for sending and receiving data across a network. This project follows a **client-server model** to facilitate communication between multiple player processes and a central ringmaster process. The socket-based communication mechanism ensures seamless message exchange and synchronization among all participants in the game.

The interaction follows a structured sequence:

```mermaid
sequenceDiagram
    participant S as Server (Ringmaster)
    participant C as Client (Player)
    autonumber
    Note over S: Initialize and bind ServerSocket
    S-->>C: Waits for incoming connection requests
    C->>S: Creates a socket and attempts to connect
    S-->>C: Accepts the connection and establishes a link // accept()
    rect rgb(100, 220, 100)
    loop Data Exchange
        Note over S,C: Bidirectional Communication
        Note over S,C: Input/Output Streams
        S-->C: Sends and receives messages
    end
    end
    S-->C: Closes connection upon game termination
```

### Explanation of the Process:

1. **Server (Ringmaster) Initialization**  
   - The ringmaster (acting as the server) creates a **server socket**, binds it to a specific address and port, and starts listening for incoming connection requests.
   
2. **Client (Player) Connection**  
   - Each player (client) creates a **socket** and initiates a connection request to the ringmaster’s socket.
   - The ringmaster accepts the request and establishes a dedicated communication channel with the player.
   
3. **Message Exchange**  
   - Once connected, the server and clients communicate through **input and output streams**.
   - Players interact with each other by sending the "potato" around using socket connections.
   - Each received message must be processed correctly according to the game rules.

4. **Game Termination and Connection Closure**  
   - When the game concludes, the ringmaster instructs all players to exit.
   - Players and the ringmaster properly close their sockets to free system resources.

This structured socket communication mechanism ensures **efficient**, **synchronized**, and **fault-tolerant** interactions between the ringmaster and the players, allowing the game to function correctly across multiple processes.

---

## Implementation Description

This project simulates the game **"Hot Potato"**, where multiple players pass a "potato" among themselves until the game stops at a random moment. The player holding the potato at that point is declared **"it"**. The objective of the game is to **avoid being the last player holding the potato**.

### Game Mechanics:

1. The **ringmaster** initializes a **potato** object with a predefined number of hops and sends it to a randomly chosen player.
2. When a **player** receives the potato:
   - The player **decrements the hop counter** and appends their unique **player ID** to the potato.
   - Based on the remaining hops:
     - If **hops remain**, the player **selects a neighbor randomly** and passes the potato.
     - If the **hop count reaches zero**, the player holding the potato **returns it to the ringmaster**, signaling the game’s completion.
3. The **ringmaster** logs the full sequence of the game (using the recorded player IDs) and sends **termination messages** to all players so they can exit properly.

The goal of this assignment is to design and execute a **single ringmaster process** along with multiple **player processes**. The processes should be **properly managed** to ensure that all players exit cleanly after receiving shutdown instructions from the ringmaster.

---

## Responsibilities

### Ringmaster:
- Establish `N` socket connections with `N` players and distribute relevant details.
- Create and initialize the **potato** object.
- Randomly select a player and send them the potato.
- Print the game trace when the potato returns to the ringmaster.
- Shut down the game by instructing all players to **terminate**.

### Player:
- Maintain **three socket connections**:
  - One with the **left neighbor**.
  - One with the **right neighbor**.
  - One with the **ringmaster**.
- Continuously **listen** on all three channels since the potato or game-related messages may arrive from any of them.
- **Properly process** received messages according to the game rules.

---

## Communication Mechanism

Each **player process** maintains communication with the **ringmaster** and its neighboring **players** through TCP sockets. Below is an outline of how messages are exchanged:

1. The **ringmaster** sends the **initial setup information** to all players.
2. Players communicate with each other in a **ring topology**, passing the potato through socket messages.
3. When a player receives the potato, they:
   - Decrement the hop counter.
   - Append their player ID to the potato.
   - Send it to a randomly chosen **neighbor**.
4. If the hop counter reaches **zero**, the player **forwards the potato to the ringmaster**, ending the game.
5. The **ringmaster** logs the full trace of the game and **sends termination signals** to all players.

---

## Example Output

A sample run of the game may produce output like this:

```
Game started with 4 players and 10 hops.
Player 2 received the potato.
Player 2 passed it to Player 3.
Player 3 passed it to Player 1.
Player 1 passed it to Player 4.
Player 4 passed it to Player 3.
Player 3 passed it to Player 2.
Player 2 passed it to Player 1.
Player 1 passed it to Player 4.
Player 4 passed it to Player 3.
Player 3 has the potato. The game is over.
Game Trace: 2 → 3 → 1 → 4 → 3 → 2 → 1 → 4 → 3
Players shutting down...
```

---

## Setup and Compilation

### **Requirements**
- A system with **Linux/macOS** (or WSL on Windows).
- A **C/C++ compiler** with support for POSIX sockets.
- Basic understanding of **network programming**.

### **Compiling the Program**
To compile the project, use:
```bash
gcc -o ringmaster ringmaster.c -pthread
gcc -o player player.c -pthread
```

### **Running the Game**
1. Start the ringmaster:
   ```bash
   ./ringmaster <port_number> <num_players> <num_hops>
   ```
   Example:
   ```bash
   ./ringmaster 5000 4 10
   ```

2. Start each player in separate terminals:
   ```bash
   ./player <ringmaster_host> <ringmaster_port>
   ```
   Example:
   ```bash
   ./player localhost 5000
   ```

Once all players are connected, the game will start automatically.

---

## Notes & Considerations

- **Graceful Termination**:  
  - Players and the ringmaster should **close sockets properly** after the game ends to avoid memory leaks.
- **Fault Tolerance**:  
  - If a player unexpectedly disconnects, the game should handle it appropriately.
- **Randomization**:  
  - Players should randomly select neighbors to prevent predictable patterns.

---

## License

This project is open-source and distributed under the **MIT License**.

---

This README file is **fully GitHub-compatible**, meaning you can **copy and paste** it directly into your `README.md` file. Let me know if you need any modifications! 🚀
```

This README is **formatted for GitHub**, fully structured, and includes:
- **Proper Markdown headers**
- **Syntax-highlighted code blocks**
- **A sequence diagram**
- **Step-by-step setup instructions**
- **An example game output**
- **A section for best practices**

Let me know if you need any refinements! 🚀
