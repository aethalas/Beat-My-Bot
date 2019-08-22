// Title  :  AStarThread.h
// Purpose:  Speeds up pathfinding
// Author :  Jonathon H Tyson
// Date   :  09/05/2018

#pragma once
#include <thread>
#include <mutex>

// Whether to use threading or not
#define USE_THREADING true

class AStarThread
{
private:
 AStarThread();
 AStarThread(const AStarThread& other);		// Disabled
 static AStarThread* pInst;
 std::wstring m_threadStatus;

public:
 ~AStarThread();
 bool doOnce = true;                   // To enable the thread once
 std::thread *worker_thread;           // The thread
 static AStarThread* GetInstance();				// Thread-safe singleton
 static void Release();                // Releases the singleton
 void StartThread();                   // Initiates the worker thread
 void StopThread();                    // Stops the thread exection
 void Pathfind();                      // The thread to be executed
 void ThreadInfo();                    // Shows whether threading is used
}; // AStarThread