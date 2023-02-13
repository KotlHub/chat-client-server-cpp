// https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
// #include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define PAUSE 50

// SOCKET ConnectSocket = INVALID_SOCKET;

char map[20][20];

DWORD WINAPI Sender(void* p)
{
	int t_money = 0;
	COORD money = { 2, 25 };
	SOCKET local = *((SOCKET*)p);
	char crd[4];
	crd[3] = 0;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pers = { 0, 2 };
	SetConsoleCursorPosition(h, pers);
	SetConsoleTextAttribute(h, 9);

	while (true)
	{

		int direct = _getch();
		if (direct == 224)
			direct = _getch();

		SetConsoleCursorPosition(h, pers);
		cout << " ";

		if (direct == 77)
		{
			if ((map[pers.Y][pers.X + 1]) != '@')
				pers.X++;
		}
		else if (direct == 75)
		{
			if ((map[pers.Y][pers.X - 1]) != '@')
				pers.X--;
		}
		else if (direct == 72)
		{
			if ((map[pers.Y - 1][pers.X]) != '@')
				pers.Y--;
		}
		else if (direct == 80)
		{
			if ((map[pers.Y + 1][pers.X]) != '@')
				pers.Y++;
		}

		char crd[4];
		// добавить перемещение вверх и вниз) 72 80
		crd[0] = pers.X;
		crd[1] = pers.Y;
		crd[2] = 0;
		//crd[3] = 0;

		send(local, crd, 3, 0);

		if (map[pers.Y][pers.X] == 2)
		{
			crd[2] = 1;
			crd[3]++;
			send(local, crd, 4, 0);
			map[pers.Y][pers.X] = 0;
			SetConsoleCursorPosition(h, money);
			t_money++;
			cout << t_money;
		}
		crd[2] = 0;

		SetConsoleCursorPosition(h, pers);
		cout << char(1);
	}

}


DWORD WINAPI rec(void* p)
{
	SOCKET s = *(SOCKET*)(p);

	int t_money = 0;
	COORD cmoney = { 2, 27 };

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pers2 = { 1, 3 };
	SetConsoleCursorPosition(h, pers2);
	SetConsoleTextAttribute(h, 9);

	while (true)
	{
		char message[4];

		(recv(s, message, 4, 0));
		if (message[2] == 0)
		{
			SetConsoleCursorPosition(h, pers2);
			cout << " ";
			pers2.X = message[0];
			pers2.Y = message[1];
			SetConsoleCursorPosition(h, pers2);
			SetConsoleTextAttribute(h, 12);
			cout << (char)1;
		}

		if (message[2] == 1)
		{
			SetConsoleCursorPosition(h, pers2);
			cout << " ";
			pers2.X = message[0];
			pers2.Y = message[1];

			map[pers2.Y][pers2.X] = 0;

			SetConsoleCursorPosition(h, cmoney);
			t_money++;
			cout << t_money;
		}

	}
}



int main(int argc, char** argv) // имя сервера при желании можно будет указать через параметры командной строки
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	setlocale(0, "");
	system("title CLIENT SIDE");
	cout << "процесс клиента запущен!\n";
	Sleep(PAUSE);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Initialize Winsock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed with error: " << iResult << "\n";
		return 11;
	}
	else {
		cout << "подключение Winsock.dll прошло успешно!\n";
		Sleep(PAUSE);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	const char* ip = "localhost"; // по умолчанию, оба приложения, и клиент, и сервер, запускаются на одной и той же машине

	//iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result); // раскомментировать, если нужно будет читать имя сервера из командной строки
	addrinfo* result = NULL;
	iResult = getaddrinfo(ip, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		cout << "getaddrinfo failed with error: " << iResult << "\n";
		WSACleanup();
		return 12;
	}
	else {
		cout << "получение адреса и порта клиента прошло успешно!\n";
		Sleep(PAUSE);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Attempt to connect to an address until one succeeds

	SOCKET ConnectSocket = INVALID_SOCKET;

	for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) { // серверов может быть несколько, поэтому не помешает цикл

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			cout << "socket failed with error: " << WSAGetLastError() << "\n";
			WSACleanup();
			return 13;
		}

		// Connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		cout << "создание сокета на клиенте прошло успешно!\n";
		Sleep(PAUSE);

		break;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cout << "невозможно подключиться к серверу. убедитесь, что процесс сервера запущен!\n";
		WSACleanup();
		return 14;
	}
	else {
		cout << "подключение к серверу прошло успешно!\n";
		Sleep(PAUSE);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	system("cls");

	int width = 20;
	int height = 20;
	char mas[20 * 20];

	recv(ConnectSocket, mas, 20 * 20, 0);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			map[i][j] = mas[j * width + i];
		}
	}

	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < 20; i++)
		{
			if (map[i][j] == 1)
				map[i][j] = '@';
			if (map[i][j] == 0)
				map[i][j] = ' ';
		}
	}

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

	COORD pers = { 0, 2 };
	SetConsoleCursorPosition(h, pers);
	SetConsoleTextAttribute(h, 9);
	cout << (char)1;

	CreateThread(0, 0, Sender, &ConnectSocket, 0, 0);
	CreateThread(0, 0, rec, &ConnectSocket, 0, 0);

	Sleep(INFINITE);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 16;
	}
	else {
		cout << "процесс клиента инициирует закрытие соединения с сервером.\n";
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Receive until the peer closes the connection


	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	cout << "процесс клиента прекращает свою работу!\n";

	return 0;
}