#include <iostream>
#include "WSASession.h"
#include "UDPSocket.h"
#include <vector>
#define WIN32_LEAN_AND_MEAN
#pragma comment (lib,"ws2_32.lib")

struct pakiet {
private:
	unsigned short operacja;
	unsigned short odpowiedz;
	unsigned short id;
	unsigned short ack;
public:
	pakiet(unsigned short opr, unsigned short res, short id, short ack) {
		this->operacja = opr;
		this->odpowiedz = res;
		this->id = id;
		this->ack = ack;
	}

	pakiet(char rawData[], unsigned short size) {
		if (size > 2) throw std::exception("To large buffor!!!");
		
		unsigned short buff = rawData[0];
		operacja = 0 | ((buff >> 4) & 0x0F);
		odpowiedz = 0 | ((buff >> 1) & 0x07);
		id = 0 | ((buff & 0x01) << 3);

		buff = rawData[1];
		id |= ((buff >> 5) & 0x07);
		ack = 0 | ((buff >> 4) & 0x01);
	}

	std::string convertToSend() {
		std::string rt;
		char buff = 0;

		buff |= (operacja << 4);
		buff |= (odpowiedz << 1);
		buff |= (id >> 3);
		rt.push_back(buff);
	
		buff = 0;
		buff |= (id << 5);
		buff |= (ack << 4);
		buff |= (0x0F);
		rt.push_back(buff);

		return rt;
	}

	unsigned short getOperation() const {
		return operacja;
	}

	unsigned short getResponse() const
	{
		return odpowiedz;
	}

	unsigned short getAckFlag() const
	{
		return ack;
	}

	unsigned short getId() const
	{
		return id;
	}
};

int main() {

	try {

		//Tworzenie sesji
		WSASession Session;  
		sockaddr_in from; 

		//Tworze gniazdo i pakiet danych
		UDPSocket Socket;
		pakiet data(0, 0, 0, 0); // REQUEST  ID 
		
		//Konwersja na stringa do wyslania + wypisanie ich
		char buffer[2]; ///Czy na pewno buffer 2 
		std::string p = data.convertToSend(); 

		//Wysylanie wiadomosci 
		Socket.SendTo("127.0.0.1", 100, p.c_str(), p.size());  //ODEBRANA POPRAWNIE
		
		//Czekanie na odpowiedz
		Socket.Bind(100);
		
		//Przygotowanie do odebrania wiadomosci
		sockaddr_in add;
		add.sin_family = AF_INET;
		add.sin_addr.s_addr = htonl(INADDR_ANY);
		add.sin_port = htons(100);

		//Odebranie wiadomosci
		Socket.RecvFrom(buffer, 2, add,  0);
	
		//Konwersja na pakiet i wypisanie jego elementow
		pakiet rt(buffer, 2);
		std::cout << rt.getOperation() << " " << rt.getResponse() << " " << rt.getId() << " " << rt.getAckFlag() << std::endl;
		
	}
	catch (std::exception& ex) {
		std::cout << ex.what();
	}

	return 0;
}
