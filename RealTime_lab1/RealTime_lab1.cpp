#include <iostream>
#include <mutex>
#include <semaphore>
#include <process.h>
#include "windows.h"
#include "Semaphore.h"
using namespace std;

enum userСhoice { Process = 1, Thread, Mutex , Sema, Evnt, WT, CS,   Yes = 1, No, Work = 0, Die = 8 };

std::mutex mainMutex;

Semaphore* s;
LONG cMax = 1; // maximum amount of threads
int alive_threads = 3; // amount of threads 

DWORD WINAPI ThreadFunction1(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE); // check whether we can start new thread
	 cout << "Semaphore occupied by thread 1.Starting thread:\n"; // starting
	 int num = 0;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n"; // making calculations
			num++;
			Sleep(100);
	 }
	 cout << "Thread 1 completed!\n";
	 s->LeaveSemaphore(); // leaving semaphore
	 cout << "Semaphore was realesed \n";
	 alive_threads--; // 1 thread is dead, reduce the amount of alive threads
	 return(0);
}

DWORD WINAPI ThreadFunction2(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE);
	 cout << "Semaphore occupied by thread 2.Starting thread:\n";
	 int num = 100;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n";
			num++;
			Sleep(100);
	 }
	 cout << "Thread 2 completed!\n";
	 s->LeaveSemaphore();
	 cout << "Semaphore was realesed\n";
	 alive_threads--;
	 return(0);

}

DWORD WINAPI ThreadFunction3(LPVOID lpParameter) {
	 s->WaitForSemaphore(INFINITE);
	 cout << "Semaphore occupied by thread 3.Starting thread:\n";
	 int num = 500;
	 for (int i = 0; i < 5; i++) {
			cout << num << "\n";
			num++;
			Sleep(100);
	 }
	 cout << "Thread 3 completed!\n";
	 s->LeaveSemaphore();
	 cout << "Semaphore was realesed \n";
	 alive_threads--;
	 return(0);
}

HANDLE g_hEventInitComplete = NULL;
unsigned __stdcall TreadProc_event(void* pArguments) {
	cout << (const char*)pArguments << " waiting for execution \n";
	WaitForSingleObject(g_hEventInitComplete, INFINITE);
	cout << (const char*)pArguments << " is working \n";
	Sleep(1000);
	cout << (const char*)pArguments << " is complete \n";
	_endthreadex(0);
	return 0;
}

CRITICAL_SECTION crit_sect;
unsigned __stdcall CRTITICAL_SECTION_func(void* pArguments) {
	cout << (const char*)pArguments << " waiting for execution \n";
	EnterCriticalSection(&crit_sect);
	cout << (const char*)pArguments << " has occupied critical section. Work start\n";
	Sleep(1000);
	LeaveCriticalSection(&crit_sect);
	cout << (const char*)pArguments << " is complete. Leaving critical section \n";
	_endthreadex(0);
	return 0;
}

void createProcess(int priority) {
	 STARTUPINFO cif;
	 ZeroMemory(&cif, sizeof(STARTUPINFO));
	 PROCESS_INFORMATION pi;
	 if (CreateProcess(L"c:\\windows\\notepad.exe", NULL,
			NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi) == TRUE)
	 {
			cout << "New process was created" << endl;
			cout << "handle " << pi.hProcess << endl;
			cout << "Press enter to continue " << endl;
			cin.get();				
			TerminateProcess(pi.hProcess, NO_ERROR);	// убрать процесс
	 }
}

// __stdcall используется для вызова win32 api
unsigned __stdcall ThreadProc(void* param)
{
	 /* вновь созданный поток будет выполнять эту функцию */
	cout << "Thread is working" << endl;
	 Sleep(1000);
	 delete[](int*)param;
	 return 0;
	 /* завершение функции = завершение потока */
}

void beginThread() {
	 HANDLE hThread;
	 unsigned dwThread;
	 /* создаем новый поток */
	 cout << "Creating new thread" << endl;
	 hThread = (HANDLE)_beginthreadex(
			NULL, 0, ThreadProc, new int[128], 0, &dwThread
	 );
	 WaitForSingleObject(hThread, INFINITE); // ожидание завершения потока
	 cout << "Thread is done" << endl;
	 CloseHandle(hThread);
	 cout << "Press enter to continue " << endl;
	 cin.get();
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
	 cout << "Using Mutex mechanism." << endl;
	 cout << "Working mode without mutex" << endl;
	 auto a = std::thread{ printChar, 30, '+' };
	 auto b = std::thread{ printChar, 30, '-' };
	 a.join();
	 b.join();
	 cout << "Working mode with mutex" << endl;
	 a = std::thread{ printCharMut, 30, '+' };
	 b = std::thread{ printCharMut, 30, '-' };
	 a.join();
	 b.join();
	 cout << "Press enter to continue " << endl;
	 cin.get();
	// выведенные символы никогда не перемешаются из-за использования мьютекса
}

void syncMechsSemaphore() {
	cout << "Using Semaphore mechanism." << endl;
	s = new Semaphore(cMax); // creating semaphore
	CreateThread(NULL, 0, ThreadFunction1, NULL, 0, NULL);
	CreateThread(NULL, 0, ThreadFunction2, NULL, 0, NULL);
	CreateThread(NULL, 0, ThreadFunction3, NULL, 0, NULL);
	while (alive_threads != 0) {} // check whether all threads died
	s -> ~Semaphore(); // destroying semaphore
	cout << "Press enter to continue " << endl;
	cin.get();
}

void syncMechsEvent() {
	cout << "Using Event mechanism." << endl;
	g_hEventInitComplete = CreateEvent(NULL, FALSE, FALSE, L"First");
	HANDLE workThread_event = (HANDLE)_beginthreadex(NULL, 0, TreadProc_event, (void*)"Thread", 0, NULL);
	Sleep(1000);
	if (g_hEventInitComplete) {
		SetEvent(g_hEventInitComplete);
		cout << "Got singnal from event \n";
		ResetEvent(g_hEventInitComplete);
		CloseHandle(g_hEventInitComplete);
	}
	if (workThread_event) {
		WaitForSingleObject(workThread_event, INFINITE);
		CloseHandle(workThread_event);
	}
	cout << "Press enter to continue " << endl;
	cin.get();
}

void syncMechsWT() {
	cout << "Using WaitableTimer mechanism." << endl;
	HANDLE timer = NULL;
	LARGE_INTEGER time;
	time.QuadPart = -10000000LL;
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (timer) {
		cout << "Create WaitableTimer with 1 second of delay. Waiting for thread..." << endl;
		SetWaitableTimer(timer, &time, 1000, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		cout << "Thread is working now" << endl;
		Sleep(1000);
		cout << "Thread is complete" << endl;
		CloseHandle(timer);
	}
	cout << "Press enter to continue " << endl;
	cin.get();
}

void syncMechsCS() {
	cout << "Using CriticalSection mechanism." << endl;
	InitializeCriticalSection(&crit_sect);
	HANDLE workThread_cs = (HANDLE)_beginthreadex(NULL, 0, CRTITICAL_SECTION_func, (void*)"Thread 1", 0, NULL);
	HANDLE workThread_1_cs = (HANDLE)_beginthreadex(NULL, 0, CRTITICAL_SECTION_func, (void*)"Thread 2", 0, NULL);
	if (workThread_cs) {
		WaitForSingleObject(workThread_cs, INFINITE);
		CloseHandle(workThread_cs);
	}
	if (workThread_1_cs) {
		WaitForSingleObject(workThread_1_cs, INFINITE);
		CloseHandle(workThread_1_cs);
	}
	DeleteCriticalSection(&crit_sect);
	cout << "Press enter to continue " << endl;
	cin.get();
}

void main()
{
	 cout << "The functions will be executed in the following sequence: " << endl;
	 cout << "1. Create process " << endl;
	 cout << "2. Create thread and using *wait function" << endl;
	 cout << "3. Using sync mechanisms" << endl;
	 for (int i = 0; i < Die; i++) {
			switch (i) {
			case Process: {
				 int priority = 0;
				 createProcess(priority);
				 break;
			}
			case Thread: {
				 beginThread();
				 break;
			}
			case Mutex: {
				 syncMechsMutex();
				 break;
			}
			case Sema: {
				syncMechsSemaphore();
				break;
			}
			case Evnt: {
				syncMechsEvent();
				break;
			}
			case WT: {
				syncMechsWT();
				break;
			}
			case CS: {
				syncMechsCS();
				break;
			}
			default:
				 break;
			}
	 }
	 cout << "All mechanisms were illustrated. Work with program is complete." << endl;
}



