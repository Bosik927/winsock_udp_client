#include <iostream>
#include "WSASession.h"
#include "UDPSocket.h"
#include <vector>
#include "Packet.h"
#include <WS2tcpip.h>

int main() {

	try {

// START SESSION
//  WSAData data
		WSASession Session; 

		//CREATE UDP SOCKET
		//AND MESSAGE TO SEND
		UDPSocket Socket;
		pakiet data(0, 0, 0, 0); // REQUEST  ID 

		char buffer[2];
		std::string p = data.convertToSend();
		//PORT 100
		Socket.SendTo("127.0.0.1", 100, p.c_str(), p.size());

		sockaddr_in from; // SERVER ADRESS
	//	in_addr serwer = "127.0.0.1";
	//	from.sin_port = htons(100);
	//	from.sin_addr = serwer;


		//WAIT FOR ANSWER
			/// Nie wiem czy mozna tak zrobic ze czekam na jedna odpowiedz
			/// Powinnismy poczekac chwilke bo przeciez moze nie dojsc wiec?
	//		Socket.RecvFrom(buffer, 2 , from, 0);
	

		
		
		pakiet rt(buffer, 2);
		std::cout << rt.getOperation() << " " << rt.getResponse() << " " << rt.getId() << " " << rt.getAckFlag() << std::endl;
		std::cin.ignore(2);
	}
	catch (std::exception& ex) {
		std::cout << ex.what();
	}



	return 0;
}

