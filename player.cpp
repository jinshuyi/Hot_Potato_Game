/*
designed by Shuyi Jin in 2025.3.1
*/
#include <vector>

#include "client.h"
#include "helper.h"
#include "potato.h"
#include "server.h"

/**
 * Handle potato according to the game rules.
 * If the potato has 0 hops remaining, send it back to the ringmaster.
 * Otherwise, decrement the hop count and pass the potato to a random neighbor.
 * If the player is the last to handle the potato, print a message indicating that "I'm it".
 *
 * @param potato The potato object received from a neighbor.
 * @param player_id The ID of the current player.
 * @param num_players The total number of players in the game.
 * @param right_fd The file descriptor for player's right neighbor.
 * @param left_fd The file descriptor for player 's left neighbor.
 * @param master_fd The file descriptor for the ringmaster.
 */
void handlePotato(int player_id, int num_players, int fd_array[]) {
  Potato potato;
  fd_set readfds;
  srand((unsigned int)time(NULL) + player_id);
  int nfds = 1 + std::max(fd_array[0], fd_array[1]);

  while (1) {
    FD_ZERO(&readfds);
    for (int i = 0; i < NumFDs; i++) {
      FD_SET(fd_array[i], &readfds);
    }
    select(nfds, &readfds, NULL, NULL, NULL);
    int rec;
    for (int i = 0; i < NumFDs; i++) {
      if (FD_ISSET(fd_array[i], &readfds)) {
        rec = receive_message(fd_array[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
      }
    }

    // must check if rec == 0
    // otherwise other player will recieve msg
    if (potato.num_hops == 0 || rec == 0) {
      break;
    }
    else {
      potato.num_hops--;
      std::cout << potato.num_hops << std::endl;
      potato.path[potato.cnt++] = player_id;
      if (potato.num_hops == 0) {
        int sd = send_message(fd_array[2], &potato, sizeof(potato), 0);
        std::cout << "I'm it" << std::endl;
      }
      else {
        int random = rand() % 2;
        int id = (random == 0) ? (player_id + 1) % num_players
                               : (player_id + num_players - 1) % num_players;
        send_message(fd_array[random], &potato, sizeof(potato), 0);
        std::cout << "Sending potato to " << id << std::endl;
      }
    }
  }
}

int main(int argc, char * argv[]) {
  if (argc != 3) {
    std::cout << "The client should be invoked as: " << std::endl;
    std::cout << "player <machine_name> <port_num>" << std::endl;
    std::cout << "example: ./player vcm-xxxx.vm.duke.edu 1234" << std::endl;
    return 1;
  }

  int player_id;
  int num_players;

  //Player as client: communicate with ringmaster
  Client player2master;
  player2master.initStatus(argv[1], argv[2]);
  player2master.createSocket();
  receive_message(player2master.socket_fd, &player_id, sizeof(player_id), 0);
  receive_message(player2master.socket_fd, &num_players, sizeof(num_players), 0);
  //cout << "player client success" << endl;

  //Player as server: can communicate with left
  Server player2left;
  player2left.initStatus(NULL, "");
  player2left.createSocket();
  int port = player2left.getPort();
  send_message(player2master.socket_fd, &port, sizeof(port), 0);

  std::cout << "Connected as player " << player_id << " out of " << num_players
            << " total players" << std::endl;

  // with the player to the right
  int nextPort;
  char nextIP[NextIPSize];
  receive_message(player2master.socket_fd, &nextIP, sizeof(nextIP), MSG_WAITALL);
  receive_message(player2master.socket_fd, &nextPort, sizeof(nextPort), MSG_WAITALL);

  char _port[9];
  sprintf(_port, "%d", nextPort);

  Client Player2right;
  Player2right.initStatus(nextIP, _port);
  Player2right.createSocket();
  int right_fd = Player2right.socket_fd;

  //get id of the player to the left
  std::string tmp;
  player2left.acceptConnection(tmp);
  int left_fd = player2left.client_connection_fd;

  int fd_array[] = {right_fd, left_fd, player2master.socket_fd};

  handlePotato(player_id, num_players, fd_array);

  close(player2master.socket_fd);
  close(player2left.socket_fd);
  close(Player2right.socket_fd);
  return 0;
}
