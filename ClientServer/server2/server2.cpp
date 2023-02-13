// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
// #undef UNICODE

#define WIN32_LEAN_AND_MEAN // To speed the build process: https://stackoverflow.com/questions/11040133/what-does-defining-win32-lean-and-mean-exclude-exactly

#include <iostream>
#include <windows.h>
// #include <winsock2.h>
#include <ws2tcpip.h> // WSADATA type; WSAStartup, WSACleanup functions and many more
#include <conio.h>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015" // a port is a logical construct that identifies a specific process or a type of network service - https://en.wikipedia.org/wiki/Port_(computer_networking)

#define PAUSE 50

#include <ctime>


int s_money = 0;
int c_money = 0;
SOCKET s;

char map[20][20] =

{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
};

DWORD WINAPI timer(void* t)
{
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD tm = { 25, 5 };
	
	do
	{
		Sleep(1000);
		SetConsoleCursorPosition(h, tm);
		cout << "    ";
		SetConsoleCursorPosition(h, tm);
		cout << *(int*)(t);
		*(int*)(t) -= 1;
	} while (*(int*)(t) != 0);
	return 0;
}

DWORD WINAPI rec(void* p)
{
	SOCKET s = *(SOCKET*)(p);
	
	COORD cmoney = { 2, 27 };

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pers = { 1, 2 };
	SetConsoleCursorPosition(h, pers);
	SetConsoleTextAttribute(h, 9);

	while (true)
	{
		char message[4];

		(recv(s, message, 4, 0));
		if (message[2] == 0)
		{
			SetConsoleCursorPosition(h, pers);
			cout << " ";
			pers.X = message[0];
			pers.Y = message[1];
			SetConsoleCursorPosition(h, pers);
			SetConsoleTextAttribute(h, 9);
			cout << (char)1;
		}

		if (message[2] == 1)
		{
			SetConsoleCursorPosition(h, pers);
			cout << " ";
			pers.X = message[0];
			pers.Y = message[1];

			map[pers.Y][pers.X] = 0;

			SetConsoleCursorPosition(h, cmoney);
			s_money++;
			cout << s_money;
		}
	}
}

DWORD WINAPI Sender(void* p)
{

	SOCKET local = *((SOCKET*)p);
	char crd2[4];
	crd2[3] = 0;

	COORD money = { 2, 25 };
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pers2 = { 1, 3 };
	SetConsoleCursorPosition(h, pers2);
	SetConsoleTextAttribute(h, 12);

	while (true)
	{

		int direct = _getch();
		if (direct == 224)
			direct = _getch();

		SetConsoleCursorPosition(h, pers2);
		cout << " ";

		if (direct == 77)
		{
			if ((map[pers2.Y][pers2.X + 1]) != '@')
				pers2.X++;
		}
		else if (direct == 75)
		{
			if ((map[pers2.Y][pers2.X - 1]) != '@')
				pers2.X--;
		}
		else if (direct == 72)
		{
			if ((map[pers2.Y - 1][pers2.X]) != '@')
				pers2.Y--;
		}
		else if (direct == 80)
		{
			if ((map[pers2.Y + 1][pers2.X]) != '@')
				pers2.Y++;
		}

		char crd2[4];
		crd2[0] = pers2.X;
		crd2[1] = pers2.Y;
		crd2[2] = 0; // монета подбирается
		//crd2[3] = 0;

		// добавить перемещение вверх и вниз) 72 80

		SetConsoleCursorPosition(h, pers2);
		SetConsoleTextAttribute(h, 12);
		cout << (char)1;

		send(local, crd2, 4, 0);

		if (map[pers2.Y][pers2.X] == 2)
		{
			crd2[2] = 1;
			crd2[3]++;
			send(local, crd2, 4, 0);
			map[pers2.Y][pers2.X] = 0;
			SetConsoleCursorPosition(h, money);
			c_money++;
			cout << c_money;
		}
		crd2[2] = 0;

		SetConsoleCursorPosition(h, pers2);
		cout << (char)1;
	}

}


int main()
{
	setlocale(0, "");
	system("title SERVER SIDE");
	cout << "процесс сервера запущен!\n";
	Sleep(PAUSE);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialize Winsock
	WSADATA wsaData; // The WSADATA structure contains information about the Windows Sockets implementation: https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-wsadata
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); // The WSAStartup function initiates use of the Winsock DLL by a process: https://firststeps.ru/mfc/net/socket/r.php?2
	if (iResult != 0) {
		cout << "WSAStartup failed with error: " << iResult << "\n";
		cout << "подключение Winsock.dll прошло с ошибкой!\n";
		return 1;
	}
	else {
		cout << "подключение Winsock.dll прошло успешно!\n";
		Sleep(PAUSE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct addrinfo hints; // The addrinfo structure is used by the getaddrinfo function to hold host address information: https://docs.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // The Internet Protocol version 4 (IPv4) address family
	hints.ai_socktype = SOCK_STREAM; // Provides sequenced, reliable, two-way, connection-based byte streams with a data transmission mechanism
	hints.ai_protocol = IPPROTO_TCP; // The Transmission Control Protocol (TCP). This is a possible value when the ai_family member is AF_INET or AF_INET6 and the ai_socktype member is SOCK_STREAM
	hints.ai_flags = AI_PASSIVE; // The socket address will be used in a call to the "bind" function

	// Resolve the server address and port
	struct addrinfo* result = NULL;
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cout << "getaddrinfo failed with error: " << iResult << "\n";
		cout << "получение адреса и порта сервера прошло c ошибкой!\n";
		WSACleanup(); // The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll): https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsacleanup
		return 2;
	}
	else {
		cout << "получение адреса и порта сервера прошло успешно!\n";
		Sleep(PAUSE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Create a SOCKET for connecting to server
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		cout << "socket failed with error: " << WSAGetLastError() << "\n";
		cout << "создание сокета прошло c ошибкой!\n";
		freeaddrinfo(result);
		WSACleanup();

		return 3;
	}
	else {
		cout << "создание сокета на сервере прошло успешно!\n";
		Sleep(PAUSE);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // The bind function associates a local address with a socket: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-bind
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed with error: " << WSAGetLastError() << "\n";
		cout << "внедрение сокета по IP-адресу прошло с ошибкой!\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 4;
	}
	else {
		cout << "внедрение сокета по IP-адресу прошло успешно!\n";
		Sleep(PAUSE);
	}

	freeaddrinfo(result);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	iResult = listen(ListenSocket, SOMAXCONN); // The listen function places a socket in a state in which it is listening for an incoming connection: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen
	if (iResult == SOCKET_ERROR) {
		cout << "listen failed with error: " << WSAGetLastError() << "\n";
		cout << "прослушка информации от клиента не началась. что-то пошло не так!\n";
		closesocket(ListenSocket);
		WSACleanup();
		return 5;
	}
	else {
		cout << "начинается прослушка информации от клиента. пожалуйста, запустите клиентское приложение! (client.exe)\n";
		// здесь можно было бы запустить некий прелоадер в отдельном потоке
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Accept a client socket
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL); // The accept function permits an incoming connection attempt on a socket: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept
	if (ClientSocket == INVALID_SOCKET) {
		cout << "accept failed with error: " << WSAGetLastError() << "\n";
		cout << "соединение с клиентским приложением не установлено! печаль!\n";
		closesocket(ListenSocket);
		WSACleanup();
		return 6;
	}
	else {
		cout << "соединение с клиентским приложением установлено успешно!\n";
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// No longer need server socket
	closesocket(ListenSocket);

	int t_time = 30;
	int coin = 8;
	srand(time(0));
	rand();

	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			int r = rand() % 10;
			if (map[i][j] == 0 && r == 0 && coin > 0)
			{
				map[i][j] = 2;
				coin--;
			}
			else if (map[i][j] == 1)
				map[i][j] = '@';
			else if (map[i][j] == 0)
				map[i][j] = ' ';
		}
	}

	coin = 8;

	char mas[20 * 20];

	int k = 0;
	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			mas[k] = map[i][j];
			k++;
		}
	}

	send(ClientSocket, mas, 20 * 20, 0);

	system("cls");

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			cout << map[i][j];
		}
		cout << endl;
	}

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = false;
	cci.dwSize = 100;
	SetConsoleCursorInfo(h, &cci);

	COORD pers = { 1, 2 };
	SetConsoleCursorPosition(h, pers);
	SetConsoleTextAttribute(h, 9);
	cout << (char)1;

	COORD pers2 = { 1, 3 };
	SetConsoleCursorPosition(h, pers2);
	SetConsoleTextAttribute(h, 12);
	cout << (char)1;

	// Receive until the peer shuts down the connection
	CreateThread(0, 0, rec, &ClientSocket, 0, 0);
	CreateThread(0, 0, Sender, &ClientSocket, 0, 0);
	CreateThread(0, 0, timer, &t_time, 0, 0);

	//CreateThread(0, 0, Sender, &ClientSocket, 0, 0);

	while (true)
	{
		if (c_money > coin / 2)
		{
			closesocket(ClientSocket);
			WSACleanup();
			system("cls");
			cout << "Client win";
		}

		else if (s_money > coin / 2)
		{
			closesocket(ClientSocket);
			WSACleanup();
			system("cls");
			cout << "Server win";
		}

		else if (t_time == 0)
		{
			closesocket(ClientSocket);
			WSACleanup();
			system("cls");
			cout << "B R U H";
		}
	}

	Sleep(INFINITE);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND); // The shutdown function disables sends or receives on a socket: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		cout << "упс, разрыв соединения (shutdown) выдал ошибку ((\n";
		closesocket(ClientSocket);
		WSACleanup();
		return 9;
	}
	else {
		cout << "процесс сервера прекращает свою работу! до новых запусков! :)\n";
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}