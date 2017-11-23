#include <iostream>
#include "WSASession.h"
#include "UDPSocket.h"
#include <vector>
#include <bitset>
#include <string>
#include <sstream>

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
	//	buff |= (ack << 4);//usunac 
	//	buff |= (0x0F);// usunac
		rt.push_back(buff);

		return rt;
	}

	bool comparePacets(pakiet pakietOtrzymany)
	{
		if ( 6== pakietOtrzymany.getOperation()&& odpowiedz== pakietOtrzymany.getResponse()&& id==pakietOtrzymany.getId() ) return true;
		return false;
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



//while(true){
//Socket.RecvFrom(buffer, 2, add, 0);
//for (const auto t : buffer) {
//	std::cout << std::bitset<8>(t) << " ";
//}
//std::cout << std::endl;

//}

void main() {
	unsigned int port = 100;

	try {

		//Tworzenie sesji
		WSASession Session;
//		std::cout << "Utowrzenie sesji" << std::endl;

		//Tworze gniazdo i pakiet danych
		UDPSocket Socket;
		pakiet packet(0, 0, 0, 0); // REQUEST  ID 
//		std::cout << "Utworzenie gniazda" << std::endl;

		//Konwersja na stringa do wyslania 
		char buffer[3]; 
		std::string p = packet.convertToSend();

		///Żądanie id
		//Wysylanie wiadomosci  
//		std::cout << "Wyslanie zadania nadania id" << std::endl;
	//	std::cout << packet.getOperation() << " " << packet.getResponse() << " " << packet.getId() << std::endl;
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());  
		
		//Przygotowanie do odebrania wiadomosci
		sockaddr_in add;

		///żądanie id ACK
		//Odebranie wiadomosci
		Socket.RecvFrom(buffer, 2, add, 0);

		//Konwersja na pakiet i wypisanie jego elementow
		packet= pakiet(buffer, 2);

		//Odebranie potwierdzenia ///ODEBRANIE ACK
	//	if(packet.comparePacets(packet))
	//	{
			//std::cout << "Odebranie potwierdzenia nadania id" << std::endl;
	//		std::cout << packet.getOperation() << " " << packet.getResponse() << " " << packet.getId() << std::endl;
	//	}
	//	else
	//	{
	//		std::system_error(WSAGetLastError(), std::system_category(), "ACK Failed");
	//	}
		
		///Odebranie id
		Socket.RecvFrom(buffer, 2, add, 0);
		pakiet packetOtrzymany = pakiet(buffer, 2); 
		//std::cout << packet.getOperation() << " " << packet.getResponse() << " " << packet.getId() << std::endl;

		p=pakiet(7,packetOtrzymany.getResponse(),packetOtrzymany.getId(),0).convertToSend();

		unsigned int ID = packetOtrzymany.getId();
		///Odebranie id ->ACK
		for (int i=0; i<1000000;i++){}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());
		//Socket.SendTo(add, buffer, 2, 0);

		//Konwersja odpowiedzi
	//	packet = pakiet(buffer, 2);
	//	std::cout << "Twoje id to: " << packet.getId() << std::endl;
		
		///Wyslanie liczby L
		std::string liczbaDoWyslania; unsigned short liczbaDoWyslaniaInt;
		std::cout << "Podaj liczbe L do wyslania: ";
		std::getline(std::cin, liczbaDoWyslania);
		//mozna dodac sprawdzanie czy podana liczba rzeczywiscie jest liczba a nie napisem
		std::istringstream iss(liczbaDoWyslania);
		iss >> liczbaDoWyslaniaInt;

		p = pakiet(3,liczbaDoWyslaniaInt, ID, 0).convertToSend();

		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());
		
		///Wyslanie liczby L ACK
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);

		///Wyslanie oczekiwania
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);
		
		///Wyslanie oczekiwania ACK
		p = pakiet(7, packetOtrzymany.getResponse(), ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

		///Odebranie liczby prob
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);
		unsigned int liczbaProb = packetOtrzymany.getResponse();

		///Odebranie liczby prob ACK
		p = pakiet(7, 0, ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

		///Start
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);
		std::cout << "Rozpoczecie rozgrywki!" << std::endl;

		///Start ACK
		p = pakiet(7, packetOtrzymany.getResponse(), ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

		std::cout << "Liczba prob to: - " << liczbaProb << " -" << std::endl;
		//Wybor liczby
		while(true)
	{
		std::cout << "Aktualna liczba prob " << liczbaProb << std::endl;
		std::cout << "Podaj liczbe ktora jest odpowiedzia : ";
		std::getline(std::cin, liczbaDoWyslania);
		//mozna dodac sprawdzanie czy podana liczba rzeczywiscie jest liczba a nie napisem
		std::istringstream sss(liczbaDoWyslania);
		sss >> liczbaDoWyslaniaInt;

			///WYSLANIE ODP
		p = pakiet(5, liczbaDoWyslaniaInt, ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

			///WYSLANIE ODP ACK
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);


			///OTRZYMANIE ODP
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);
		

		///OTRZYMANIE ODP ACKs
		p = pakiet(7, packetOtrzymany.getResponse(), ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

		if (packetOtrzymany.getResponse() == 0) { std::cout << "Przegrana!"; break; }
		if (packetOtrzymany.getResponse() == 1) { std::cout << "Probuj dalej!" << std::endl; liczbaProb--; }
		if (packetOtrzymany.getResponse() == 2) { std::cout << "Wygrales!" << std::endl; break; }

		if (liczbaProb == 0) break;
	}
		if (liczbaProb == 0) { std::cout << "Z powodu braku prob" << std::endl; }

		///Zadnie zakonczenia
		p = pakiet(15, 0, ID, 0).convertToSend();
		for (int i = 0; i<1000000; i++) {}
		Socket.SendTo("25.50.98.211", port, p.c_str(), p.size());

		///Zadanie zakonczenia ACK
		Socket.RecvFrom(buffer, 2, add, 0);
		packetOtrzymany = pakiet(buffer, 2);
		std::cin.ignore(2);

	}
	catch (std::exception& ex) {
		std::cout << ex.what();
	}


}
