#include <iostream>
#include <mutex>
#include <semaphore>
#include <process.h>
#include "windows.h"
#include "Semaphore.h"
using namespace std;

enum userСhoice { Process = 1, Thread, Sync, Sema,  Yes = 1, No, Work = 0, Die = 5 };

std::mutex mainMutex;

Semaphore* s;
LONG cMax = 1; // maximum amount of threads
int alive_threads = 3; // amount of threads 

DWORD WINAPI ThreadFunction1(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE); // check whether we can start new thread
	 cout << "Starting 1 thread: \n"; // starting
	 int num = 0;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n"; // making calculations
			num++;
			Sleep(100);
	 }
	 cout << "Thread 1 completed! \n";
	 s->LeaveSemaphore(); // leaving semaphore
	 alive_threads--; // 1 thread is dead, reduce the amount of alive threads
	 return(0);
}

DWORD WINAPI ThreadFunction2(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE);
	 cout << "Starting 2 thread: \n";
	 int num = 100;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n";
			num++;
			Sleep(100);
	 }
	 cout << "Thread 2 completed! \n";
	 s->LeaveSemaphore();
	 alive_threads--;
	 return(0);

}

DWORD WINAPI ThreadFunction3(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE);
	 cout << "Starting 3 thread: \n";
	 int num = 500;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n";
			num++;
			Sleep(100);
	 }
	 cout << "Thread 3 completed! \n";
	 s->LeaveSemaphore();
	 alive_threads--;
	 return(0);
}

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
			case Sema: {
				 s = new Semaphore(cMax); // creating semaphore
				 CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
				 CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
				 CreateThread(NULL, 0, ThreadFunction3, NULL, 0, NULL);
				 while (alive_threads != 0) {} // check whether all threads died
				 s -> ~Semaphore(); // destroying semaphore

				 _getch();
				 break;
			}
			default:
				 break;

			}

	 }
}



