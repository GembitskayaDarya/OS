#include <iostream>
#include <Windows.h>
#include <conio.h>

using namespace std;

struct message {
	int sender;
	int receiver;
	char text[100];
};

int main(int argc, char *argv[]) {

	cout << "Client " << argv[1] << " was created!" << endl;

	HANDLE hWritePipe, hReadPipe, enableRead;

	hWritePipe = (HANDLE)atoi(argv[3]);
	hReadPipe = (HANDLE)atoi(argv[2]);
	enableRead = (HANDLE)atoi(argv[4]);

	int command;
	bool isStop = false;
	while (true) {
		cout << "\n1 - read message, 2 - send message, 3 - exit" << endl;
		cin >> command;
		switch (command)
		{
		case 1:
		{
			message m;
			DWORD dwBytesRead;
			ReadFile(hReadPipe, &m, sizeof(m), &dwBytesRead, NULL);
			//_cprintf("%d client says: %s \n", m.sender, m.text);
			cout << m.sender << " client said: " << m.text << endl;
			break;
		}

		case 2:
		{
			DWORD dwBytesWritten;
			message m;
			m.sender = atoi(argv[1]);
			cout << "Enter receiver number:\n";
			cin >> m.receiver;
			cout << "Input message:\n";
			cin >> m.text;
			if (WriteFile(hWritePipe, &m, sizeof(m), &dwBytesWritten, NULL)) {
				SetEvent(enableRead);
				//_cprintf("Message sent.\n", m.text);
				cout << "Message sent\n";
			}
			break;
		}

		case 3:
		{
			isStop = true;
			break;
		}
		}
		if (isStop)
			break;
	}


	return 0;
}