#pragma once
#include<windows.h>
#include<iostream>
#include<conio.h>

using namespace std;

class Semaphore {

private:

	 HANDLE event;
	 unsigned int maxThread, freeThread; //  maximum amount of threads, the amount of possible threads at the start

public:

	 Semaphore(unsigned int size) : // constructor, analogue for CreateSemaphore from windows.h
			maxThread(size), // maxThread = size
			freeThread(size) // freeThread = size
	 {
			event = CreateEvent(NULL, TRUE, TRUE, NULL); // manual reset, signal state 

			if (event == NULL)cout << "Error creating semaphore! \n";
			if (event != NULL)cout << "Semaphore has been created! \n";

			Sleep(500);
	 }
	 ~Semaphore() { // destructor

			CloseHandle(event); // deleting event
			cout << "Semaphore has been destroyed! \n";
	 }

	 bool WaitForSemaphore(DWORD ms) { // analogue for WaitForSingleObject from windows.h

			while (TRUE) {

				 if (WaitForSingleObject(event, ms) == WAIT_OBJECT_0 && freeThread > 0) { // event is in signal state & we're able to start new thread

						if (freeThread > 0) { // checking whether we could still start new thread
							 freeThread--; // reduce the amount of free threads
							 if (freeThread == 0)ResetEvent(event); // if there are no vacant places in semaphore, we "block" and make non-signal 
							 return(FALSE); // stop cycle
						}
				 }
			}



	 }


	 void LeaveSemaphore() { // analogue for ReleaseSemaphore from windows.h

			if (freeThread == 0)SetEvent(event);
			freeThread++; // if there were no vacant places, free one and make event signal again

	 }


};