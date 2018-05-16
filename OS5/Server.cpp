#include <iostream>
#include <Windows.h>
#include <conio.h>

using namespace std;

bool isStop = false;

struct message {
	int sender;
	int receiver;
	char text[100];
};


int threadFunc() {
	cout << "Press any key to stop the server" << endl;
	_getch();
	isStop = true;
	ExitProcess(0);
}

int main(int argc, char *argv[]) {

	int countClients = 0;
	cout << "Enter amount of clients: " << endl;
	cin >> countClients;

	STARTUPINFO* si = new STARTUPINFO[countClients];
	PROCESS_INFORMATION* piApp = new PROCESS_INFORMATION[countClients];
	HANDLE* hWritePipe = new HANDLE[countClients];
	HANDLE* hReadPipe = new HANDLE[countClients];
	HANDLE* events = new HANDLE[countClients];

	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	for (int i = 0; i < countClients; i++)
	{
		events[i] = CreateEvent(&sa, false, false, NULL);
		if (!CreatePipe(&hReadPipe[i], &hWritePipe[i], &sa, 0))
		{
			_cputs("Create pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return GetLastError();
		}
		char params[80];
		wsprintf(params,
			"Client.exe %d %d %d %d", (int)(i + 1),
			(int)hReadPipe[i],
			(int)hWritePipe[i],
			(int)events[i]);

		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);

		if (!CreateProcess(NULL, params, NULL, NULL, TRUE,
			CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &piApp[i]))
		{
			_cputs("The new process is not created.\n");
			_cputs("Check a name of the process.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return 0;
		}
	}

	HANDLE  hThread;
	DWORD IDThread;

	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadFunc, NULL, 0, &IDThread);
	if (hThread == NULL)
		return GetLastError();


	while (true) {
		if (isStop)
			break;
		message m;
		DWORD dwBytesRead;
		int index = WaitForMultipleObjects(countClients, events, false, INFINITE);
		index -= WAIT_OBJECT_0;
		if (!ReadFile(hReadPipe[index], &m, sizeof(m), &dwBytesRead, NULL))
		{
			_cputs("Read from the pipe failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return GetLastError();
		}

		if (m.receiver - 1 >= 0 && m.receiver - 1 < countClients) {
			DWORD dwBytesWritten;
			if (!WriteFile(hWritePipe[m.receiver - 1], &m, sizeof(m), &dwBytesWritten, NULL))
			{
				_cputs("Write to file failed.\n");
				_cputs("Press any key to finish.\n");
				_getch();
				return GetLastError();
			}
		}
		cout << "Message from " << m.sender << " to " << m.receiver << ": " << m.text << endl;
	}


	for (int i = 0; i < countClients; i++)
	{
		CloseHandle(piApp[i].hThread);
		CloseHandle(piApp[i].hProcess);
		CloseHandle(events[i]);
		CloseHandle(hWritePipe[i]);
		CloseHandle(hReadPipe[i]);
	}

	delete[] si;
	delete[] piApp;
	delete[] events;
	delete[] hWritePipe;
	delete[] hReadPipe;

	return 0;
}