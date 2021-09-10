///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef REFLECTOR

#include "include/tcp_server.h"
#include <string>
#include <list>
#include <mutex>

using namespace std;

#define forever true

int PORT = 16800;

TcpServer server;
server_observer_t observer;
list<Client> clients_list;
mutex clients_mutex;
	
void onIncomingMsg(const Client & client, const char * msg, size_t size) {
    clients_mutex.lock();

	for (list<Client>::iterator it = clients_list.begin(); it != clients_list.end(); ++it)
	{
		if ((*it) == client)
		{
			continue;
		}
		server.sendToClient((*it), msg, size);
	}
	
	clients_mutex.unlock();
}

void onClientDisconnected(const Client & client) {
    clients_mutex.lock();
	
	clients_list.remove(client);
	
	clients_mutex.unlock();
}

int main(int argc, char *argv[])
{
    if (argc == 2)
	{
		PORT = atoi(argv[1]);
	}
	
	pipe_ret_t startRet = server.start(PORT);
	
	if (!startRet.success)
	{
		cout << "Server setup failed: " << startRet.msg << endl;
        return EXIT_FAILURE;
	}
	
	observer.incoming_packet_func = onIncomingMsg;
	observer.disconnected_func    = nullptr;
	observer.wantedIp             = "";
	
	server.subscribe(observer);
	
	while(forever) {
        Client client = server.acceptClient(0);
		
		clients_mutex.lock();
		
		clients_list.push_back(client);
		
		clients_mutex.unlock();
		
        sleep(1);
    }
	
    return 0;
}

#endif