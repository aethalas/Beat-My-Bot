#include "AStarThread.h"
#include "dynamicObjects.h"
#include "mydrawengine.h"
#define NULL 0

std::mutex mtx;


AStarThread::AStarThread()
{
 if (USE_THREADING == true)
 {
  m_threadStatus = L":: Threading Enabled!";
 }
 else
 {
  m_threadStatus = L":: No Threading!";
 }
}


AStarThread::~AStarThread()
{

}


AStarThread* AStarThread::pInst = NULL;


AStarThread* AStarThread::GetInstance()				// Static
{
 if (pInst == NULL)
 {
  pInst = new AStarThread;
 }

 return pInst;
}



void AStarThread::Release()
{
 if (pInst)
 {
  delete pInst;
  pInst = NULL;
 }
}



void AStarThread::StartThread()
{
 worker_thread = new std::thread(&AStarThread::Pathfind, this);
}



void AStarThread::StopThread()
{
 worker_thread->join();
 delete worker_thread;
 worker_thread = nullptr;
}


void AStarThread::Pathfind()
{
 std::lock_guard<std::mutex> lock(mtx);
 while (true)
 {
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
   if (DynamicObjects::GetInstance()->GetBot(0, i).m_bSeekPath)
   {
    DynamicObjects::GetInstance()->GetBot(0, i).GetBVR()->m_vPath.clear();
    DynamicObjects::GetInstance()->GetBot(0, i).GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(DynamicObjects::GetInstance()->GetBot(0, i).GetLocation(), DynamicObjects::GetInstance()->GetBot(0, i).target));
    DynamicObjects::GetInstance()->GetBot(0, i).m_bPathFound = true;
    DynamicObjects::GetInstance()->GetBot(0, i).m_bSeekPath = false;
   }
  }
 }
} // Pathfind()



void AStarThread::ThreadInfo()
{
 MyDrawEngine::GetInstance()->WriteText(305.0f, 10.0f, m_threadStatus.c_str(), _XRGB(128, 77, 77));
} // ThreadInfo()