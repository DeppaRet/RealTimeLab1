#include <iostream>
#include <mutex>
#include <semaphore>
#include <process.h>
#include "windows.h"
using namespace std;

enum userСhoice { Process = 1, Thread, Sync,  Yes = 1, No, Work = 0, Die = 4 };

std::mutex mainMutex;
void createProcess(int priority) {
	 STARTUPINFO cif;
	 ZeroMemory(&cif, sizeof(STARTUPINFO));
	 PROCESS_INFORMATION pi;
	 if (CreateProcess(L"c:\\windows\\notepad.exe", NULL,
			NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi) == TRUE)
	 {
			cout << "process" << endl;
			cout << "handle " << pi.hProcess << endl;
			cout << "Press any button to continue " << endl;
			//cin.get();				
			//TerminateProcess(pi.hProcess, NO_ERROR);	// убрать процесс
	 }
}

// __stdcall используется для вызова win32 api
unsigned __stdcall ThreadProc(void* param)
{
	 /* вновь созданный поток будет выполнять эту функцию */
	 Sleep(1000);
	 delete[](int*)param;
	 return 0;
	 /* завершение функции = завершение потока */
}

void beginThread() {
	 HANDLE hThread;
	 unsigned dwThread;
	 /* создаем новый поток */
	 hThread = (HANDLE)_beginthreadex(
			NULL, 0, ThreadProc, new int[128], 0, &dwThread
	 );
	 cout << "Work in progress" << endl;
	 
	 WaitForSingleObject(hThread, INFINITE); // ожидание завершения потока
	 cout << "Work is done" << endl;
	 CloseHandle(hThread);
	 
}


auto printChar(int n, char c) {
	 for (int i = 0; i < n; ++i) { std::cout << c; }
			cout << endl;
}

auto printCharMut(int n, char c) {
	 mainMutex.lock();
	 for (int i = 0; i < n; ++i) { std::cout << c; }
			cout << endl;
	 mainMutex.unlock();
}

void syncMechsMutex() {
	 int userChoice = 0;
	 cout << "1. Without mutex" << endl;
	 cout << "2. With mutex" << endl;
	 cin >> userChoice;
	 if (userChoice == 1) {
			auto a = std::thread{ printChar, 30, '+' };
			auto b = std::thread{ printChar, 30, '-' };
			a.join();
			b.join(); 
	 }
	 else {
			auto a = std::thread{ printCharMut, 30, '+' };
			auto b = std::thread{ printCharMut, 30, '-' };
			a.join();
			b.join(); // выведенные символы никогда не перемешаются из-за использования мьютекса
	 }


}

void main()
{
	 int userChoice = 0;
	 cout << "The functions will be executed in the following sequence: " << endl;
	 cout << "1. Create process " << endl;
	 cout << "2. Create thread and using *wait function" << endl;
	 cout << "3. Using sync mechanisms" << endl;
	 for (int i = userChoice; i < Die; i++) {
			switch (i) {
			case Process: {
				 int priority = 0;
				 // cout << "Choose process priority: " << endl;
				 // cout << "1. Idle priority" << endl;
				 // cout << "2. Normal priority" << endl;
				 // cout << "3. High priority" << endl;
				 // cout << "4. Real time priority" << endl;
				 // cin >> priority;
				 createProcess(priority);
				 break;
			}
			case Thread: {
				 beginThread();
				 cin.get();
				 break;
			}
			case Sync: {
				 syncMechsMutex();
				 break;
			}
			default:
				 break;

			}

	 }
}



