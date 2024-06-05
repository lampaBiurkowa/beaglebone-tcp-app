#include <iostream> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>

using namespace std;

const int PORT = 12345;
const int MAX_CONNEXTIONS = 3;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

bool pollMessage(int newsockfd, ofstream *outfile) {
    char buffer[256] = {};
    int result = read(newsockfd, buffer, 255);
    if (result < 0) error("ERROR reading from socket");

    printf("Here is the message: %s\n", buffer);
    *outfile << buffer << endl;
    if (buffer[0] == 'x') {
        result = write(newsockfd, "x", 1);
        return false;
    } else {
        result = write(newsockfd, "I got your message", 18);
        if (result < 0) error("ERROR writing to socket");
    }

    return true;
}

void printSavedMessages(const char* filename) {
    ifstream infile(filename);
    if (!infile.is_open()) {
        cerr << "ERROR opening file for reading" << endl;
        return;
    }

    string line;
    while (getline(infile, line)) {
        cout << line << endl;
    }
    infile.close();
}

int main() {
    int sockfd, newsockfd, result;
    socklen_t cli_length;
    char buffer[256];
    const char* MESSAGES_FILE = "messages.txt";
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)  error("ERROR on binding");

    listen(sockfd, MAX_CONNEXTIONS);
    cli_length = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cli_length);
    if (newsockfd < 0) error("ERROR on accept");

    ofstream outfile(MESSAGES_FILE, ios_base::app);
    if (!outfile.is_open()) error("ERROR opening file");

    while (true) {
        if (!pollMessage(newsockfd, &outfile)) break;
    }

    outfile.close();
    close(newsockfd);
    close(sockfd);

    printSavedMessages(MESSAGES_FILE);

    return 0;
}
