/*
designed by Shuyi Jin in 2025.3.1
*/
#include <vector>

#include "helper.h"
#include "potato.h"
#include "server.h"

/**
 * This function establishes connections with all players and gets their IP, port,
 * and file descriptor in order to communicate with them.
 * @param ringmaster a Server object representing the ringmaster
 * @param num_players number of players in the game
 * @param fd file descriptors for each player
 * @param ip IP addresses for each player
 * @param port port numbers for each player
 */
void establishConnections(Server & ringmaster,
                          int num_players,
                          std::vector<int> & fd,
                          std::vector<std::pair<std::string, int> > & ipPort) {
  int i = 0;
  while (i < num_players) {
    //get ip
    ringmaster.acceptConnection(ipPort[i].first);
    fd.push_back(ringmaster.client_connection_fd);

    // send player_id to client
    send_message(fd[i], &i, sizeof(i), 0);

    // send num_players to client
    send_message(fd[i], &num_players, sizeof(num_players), 0);
    // receive port from client
    receive_message(fd[i], &ipPort[i].second, sizeof(ipPort[i].second), MSG_WAITALL);

    std::cout << "Player " << i << " is ready to play " << std::endl;
    ++i;
  }
}

/**
 * This function sends each player the IP address and port number of the next player in the ring
 * so that they can pass the potato to them.
 * @param num_players number of players in the game
 * @param ip IP addresses for each player
 * @param port port numbers for each player
 * @param fd file descriptors for each player
 */
void sendNextPlayerInfo(int num_players,
                        const std::vector<std::pair<std::string, int> > & ipPort,
                        const std::vector<int> & fd) {
  int i = 0;
  while (i < num_players) {
    int nextID = 0;
    if (i != num_players - 1) {
      nextID = i + 1;
    }
    int nextPort = ipPort[nextID].second;
    char nextIP[NextIPSize];
    memset(nextIP, 0, sizeof(nextIP));
    strcpy(nextIP, ipPort[nextID].first.c_str());

    // send next player ip to client
    send_message(fd[i], &nextIP, sizeof(nextIP), 0);
    // send next player port to client
    send_message(fd[i], &nextPort, sizeof(nextPort), 0);
    ++i;
  }
}

/**
 * This function sends the potato to a random player and waits for it to be passed around the ring
 * until it reaches the designated hop count.
 * @param ringmaster a Server object representing the ringmaster
 * @param num_players number of players in the game
 * @param fd  file descriptors for each player
 * @param potato Potato object that will be passed
 */
void sendAndReceivePotato(Server & ringmaster,
                          int num_players,
                          const std::vector<int> & fd,
                          Potato & potato) {
  srand((unsigned int)time(NULL) + num_players);
  int random = rand() % num_players;

  std::cout << "Ready to start the game, sending potato to player " << random
            << std::endl;

  //On success, these calls return the number of bytes sent.
  int sd = send_message(fd[random], &potato, sizeof(potato), 0);

  fd_set readfds;
  FD_ZERO(&readfds);
  int i = 0;
  while (i < num_players) {
    FD_SET(fd[i], &readfds);
    ++i;
  }

  select(ringmaster.client_connection_fd + 1, &readfds, NULL, NULL, NULL);

  i = 0;
  while (i < num_players) {
    if (FD_ISSET(fd[i], &readfds)) {
      int rec = receive_message(fd[i], &potato, sizeof(potato), MSG_WAITALL);
      break;
    }
    ++i;
  }
}

void shutDownGame(int num_players, const std::vector<int> & fd, const Potato & potato) {
  int i = 0;
  while (i < num_players) {
    send_message(fd[i], &potato, sizeof(potato), 0);
    ++i;
  }
}

void printIpPort(std::vector<std::pair<std::string, int> > ipPort) {
  std::cout << "===========IP-----------Port===============" << std::endl;
  std::vector<std::pair<std::string, int> >::iterator it = ipPort.begin();
  while (it != ipPort.end()) {
    std::cout << it->first << "               " << it->second << std::endl;
  }
  std::cout << "===========IP-----------Port===============" << std::endl;
}

int main(int argc, char * argv[]) {
  if (argc != 4) {
    std::cout << "The server should be invoked as: " << std::endl;
    std::cout << "ringmaster<port_num> <num_players> <num_hops>" << std::endl;
    std::cout << "example: ./ringmaster 1234 3 100" << std::endl;
    return 1;
  }

  int num_players = atoi(argv[2]);
  if (num_players < MinNumPlayer) {
    std::cout << "Number of players must be greater than " << MinNumPlayer << std::endl;
    return 1;
  }

  int num_hops = atoi(argv[3]);
  if (num_hops < MinNumHops || num_hops > MaxNumHops) {
    std::cout << "Number of hops must be greater than or equal to " << MinNumHops
              << "\n and less than or equal to " << MaxNumHops << std::endl;
    return 1;
  }

  std::vector<int> fd;
  std::vector<std::pair<std::string, int> > ipPort(num_players);

  std::cout << "Potato Ringmaster" << std::endl;
  std::cout << "Players = " << num_players << std::endl;
  std::cout << "Hops = " << num_hops << std::endl;

  Server ringmaster;
  ringmaster.initStatus(NULL, argv[1]);
  ringmaster.createSocket();

  //Establish N network socket connections with N number of players
  establishConnections(ringmaster, num_players, fd, ipPort);

  //and provide relevant information to each player
  sendNextPlayerInfo(num_players, ipPort, fd);

  //Create a “potato” object
  Potato potato;
  potato.num_hops = num_hops;

  //Randomly select a player and send the
  //“potato” to the selected player
  if (num_hops > 0) {
    sendAndReceivePotato(ringmaster, num_players, fd, potato);
  }
  //Shut the game down by sending a message to each player
  shutDownGame(num_players, fd, potato);

  if (num_hops > 0) {
    //print a trace of the potato to the screen
    std::cout << "Trace of potato:" << std::endl;
    for (int i = 0; i < potato.cnt; i++) {
      std::cout << potato.path[i];
      char out = (i == potato.cnt - 1) ? '\n' : ',';
      std::cout << out;
    }
  }

  for (int i = 0; i < num_players; i++) {
    close(fd[i]);
  }
  close(ringmaster.socket_fd);

  return 0;
}
