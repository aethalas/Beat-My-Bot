// Title  :  state.cpp
// Purpose:  Provides the Bots with a state
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

#include "state.h"
#include "bot.h"
#include "dynamicobjects.h"
#include "staticmap.h"
#include "AStarThread.h"

Dead* Dead::m_instance = nullptr;
Capture* Capture::m_instance = nullptr;
Defend* Defend::m_instance = nullptr;
Attack* Attack::m_instance = nullptr;
Reload* Reload::m_instance = nullptr;


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Dead::Dead()
{
 
} // Dead()


Dead::~Dead()
{
 
} // ~Dead()


Dead* Dead::GetInstance()
{
 if (!m_instance)
 {
  m_instance = new Dead;
 }
 return m_instance;
} // GetInstance()



void Dead::Release()
{
 if (m_instance)
 {
  delete m_instance;
  m_instance = NULL;
 }
}


void Dead::Enter(Bot* pBot)
{
 pBot->m_timerOnce = true;
 pBot->GetBVR()->SetBehaviour(0, 0, 0, 0, 0);
 pBot->GetBVR()->m_vPath.clear();
} // Enter()


void Dead::Execute(Bot* pBot)
{
 if (pBot->IsAlive())
 {
  pBot->GetFSM()->ChangeState(Capture::GetInstance());
 } // if
} // Execute()


void Dead::Exit(Bot* pBot)
{
 pBot->StopAiming();

} // Exit()


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Capture::Capture()
{
 captureLog.open("captureLog.txt");
} // Capture()


Capture::~Capture()
{
 captureLog.close();
 AStarThread::GetInstance()->StopThread();
} // ~Capture()


Capture* Capture::GetInstance()
{
 if (!m_instance)
 {
  m_instance = new Capture;
 }
 return m_instance;
} // GetInstance()



void Capture::Release()
{
 if (m_instance)
 {
  delete m_instance;
  m_instance = NULL;
 }
}



void Capture::Enter(Bot* pBot)
{
 pBot->t1 = std::chrono::high_resolution_clock::now();

 int dom = pBot->GetBotNumber()%3;
 Vector2D target2 = DynamicObjects::GetInstance()->GetDominationPoint(dom).m_Location;
 pBot->target = target2;
 pBot->dom = dom;

 if (AStarThread::GetInstance()->doOnce && pBot->GetBotNumber() == 0)
 {
  AStarThread::GetInstance()->StartThread();
  AStarThread::GetInstance()->doOnce = false;
 }

 if (USE_THREADING == true)
 {
	 pBot->m_bPathFound = false;
	 pBot->m_bSeekPath = true;
	 pBot->GetBVR()->SetBehaviour(1, 0, 0, 1, 0);
 }
 else
 {
	 pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);
	 pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), pBot->target)); 
 }
} // Enter()


void Capture::Execute(Bot* pBot)
{
 if (pBot->IsAlive())
 {
  if (pBot->m_bPathFound)
  {
   pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);
  }

  if ((pBot->GetLocation() - pBot->target).magnitude() <= 400 && pBot->m_timerOnce)
  {
	  pBot->m_timerOnce = false;
	  pBot->t2 = std::chrono::high_resolution_clock::now();
	  std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(pBot->t2 - pBot->t1);
	  captureLog << time_span.count() << "\n";
	  pBot->m_timeTaken = time_span.count();
  }

  if (DynamicObjects::GetInstance()->GetDominationPoint(pBot->dom).m_OwnerTeamNumber == pBot->GetTeamNumber() &&
   (pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(pBot->dom).m_Location).magnitude() <= 400.0f)
  {
   pBot->GetFSM()->ChangeState(Defend::GetInstance());
  } // if

  // Checking for enemies nearby
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
   if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() <= 400.0f &&
       DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).IsAlive() &&
       StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()))
   {
    pBot->GetFSM()->ChangeState(Attack::GetInstance());
   }
  }

 	pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(pBot->target, Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));

  }
  else
  {
   pBot->GetFSM()->ChangeState(Dead::GetInstance()); 
  } // else
}

void Capture::Exit(Bot* pBot)
{
 //pBot->m_bPathFound = false;
} // Exit()


void Capture::SetDomTarget(Bot* pBot)
{
	if (pBot->GetBotNumber() == 0 || pBot->GetBotNumber() == 1 || pBot->GetBotNumber() == 2)
	{
		//pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(1).m_Location));
  pBot->SetTargetDomPoint(1);
	} // if
	else
	{
		//pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(2).m_Location));
  pBot->SetTargetDomPoint(2);
	} // else
 //pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetBotNumber()%3).m_Location));
 //pBot->SetTargetDomPoint(pBot->GetBotNumber() % 3);
} // SetTarget()


void Capture::CheckDomPoint(Bot* pBot)
{
 if (DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetTargetDomPoint()).m_OwnerTeamNumber == pBot->GetTeamNumber())
 {
  pBot->GetFSM()->ChangeState(Defend::GetInstance());
 } // if
} // CheckDomPoint()


void Capture::ClosestEnemy(Bot* pBot)
{
 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() <= 400 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive())
  {
   pBot->GetFSM()->ChangeState(Attack::GetInstance());
  } // if
 } // for
} // ClosestEnemy()


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Defend::Defend()
{
 
} // Defend()


Defend::~Defend()
{
 
} // ~Defend()


Defend* Defend::GetInstance()
{
 if (!m_instance)
 {
  m_instance = new Defend;
 }
 return m_instance;
} // GetInstance()


void Defend::Release()
{
 if (m_instance)
 {
  delete m_instance;
  m_instance = NULL;
 }
}


void Defend::Enter(Bot* pBot)
{
 //pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);
 //pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), pBot->target));
// CheckDistance(pBot);
} // Defend()


void Defend::Execute(Bot* pBot)
{
 if (!pBot->IsAlive())
 {
  pBot->GetFSM()->ChangeState(Dead::GetInstance());
 // pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(pBot->target, Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));
 } // if
 else
 {
  //if (CheckForThreats(pBot))
  //{
  // pBot->GetFSM()->ChangeState(Attack::GetInstance());
  //}

  // Checking for enemies nearby
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
   if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) &&
    (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() <= 400 &&
    DynamicObjects::GetInstance()->GetBot(1, i).IsAlive())
   {
    pBot->GetFSM()->ChangeState(Attack::GetInstance());
   } // if
   else if (pBot->AmmoRemaining() <= 5)
   {
    pBot->GetFSM()->ChangeState(Reload::GetInstance());
   }
  } // for

  pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(pBot->target, Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));
 } // else
} // Execute()


void Defend::Exit(Bot* pBot)
{
 
} // Exit()


bool Defend::CheckForThreats(Bot *pBot)
{
 bool bThreatDetected = false;
 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).IsAlive() &&
   (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() < 400.0f)
  {
   bThreatDetected = true;
  }
 }
 return bThreatDetected;
} // CheckForThreats()


void Defend::SetDomTarget(Bot* pBot)
{
	pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetTargetDomPoint()).m_Location));
} // SetTarget()


void Defend::CheckDomPoint(Bot* pBot)
{
 if (DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetTargetDomPoint()).m_OwnerTeamNumber != pBot->GetTeamNumber())
 {
  pBot->GetFSM()->ChangeState(Capture::GetInstance());
 } // if
} // CheckDomPoint()


void Defend::ClosestEnemy(Bot* pBot)
{
 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() <= 400 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive())
  {
   pBot->GetFSM()->ChangeState(Attack::GetInstance());
  } // if
  else if (pBot->AmmoRemaining() <= 5)
  {
   pBot->GetFSM()->ChangeState(Reload::GetInstance());
  } //elseif
 } // for
} // ClosestEnemy()


void Defend::CheckDistance(Bot* pBot)
{
 if ((StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetTargetDomPoint()).m_Location)) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(pBot->GetTargetDomPoint()).m_Location).magnitude() <= 400.0f)
 {
  pBot->GetBVR()->SetBehaviour(1, 0, 0, 1, 0);
 } // if
 else
 {
  pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);

  int dom = pBot->GetBotNumber() % 3;
  Vector2D target2 = DynamicObjects::GetInstance()->GetDominationPoint(dom).m_Location;
  pBot->target = target2;

 } // else
} // CheckDistance()


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Attack::Attack()
{
 
} // Attack()


Attack::~Attack()
{
 
} // ~Attack()


Attack* Attack::GetInstance()
{
 if (!m_instance)
 {
  m_instance = new Attack;
 }
 return m_instance;
} // GetInstance()


void Attack::Release()
{
 if (m_instance)
 {
  delete m_instance;
  m_instance = NULL;
 }
}


void Attack::Enter(Bot* pBot)
{
 pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 0);
 float bestDistance = FLT_MAX;

 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
      GetLocation()).magnitude() <= 600 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() < bestDistance &&
      pBot->GetTargetBot() < 0)
  {
   bestDistance = DynamicObjects::GetInstance()->GetBot(1, i).GetLocation().magnitude();
   pBot->SetTarget(1, i);
  } // if
 } // for
 // SetTargetBot(pBot);
} // Enter()


void Attack::Execute(Bot* pBot)
{
 // Check if the bot is alive
 if (!pBot->IsAlive())
 {
  pBot->GetFSM()->ChangeState(Dead::GetInstance());
 } // if

 // Check ammo
 else if (pBot->AmmoRemaining() <= 0)
 {
  pBot->GetFSM()->ChangeState(Reload::GetInstance());
 } // else if
 else
 {
  // ***************************************************
  //float bestDistance = FLT_MAX;

  //for (int i = 0; i < NUMBOTSPERTEAM; i++)
  //{
  // if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
  //  GetLocation()).magnitude() <= 500 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() < bestDistance)
  // {
  //  bestDistance = DynamicObjects::GetInstance()->GetBot(1, i).GetLocation().magnitude();
  //  pBot->SetTarget(1, i);
  // } // if
  //} // for



  float bestDistance = FLT_MAX;

  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
   if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()) &&
    (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() <= 500.0f &&
    DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).IsAlive() &&
    (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() < bestDistance &&
     pBot->GetTargetBot() < 0)
   {

    float distance = (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude();

    if (distance < bestDistance)
    {
     bestDistance = distance;
    }

    pBot->SetTarget(1 - pBot->GetTeamNumber(), i);
   } // if
  } // for


   if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), pBot->GetTargetBot()).GetLocation()) &&
    DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), pBot->GetTargetBot()).IsAlive() &&
    (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), pBot->GetTargetBot()).GetLocation()).magnitude() <= 400.0f)
   {
    if (pBot->GetAccuracy() >= 0.7f)
    {
     pBot->Shoot();
    } // if
   } // elseif

   if (!CheckForThreats(pBot))
   {
    pBot->GetFSM()->ChangeState(Capture::GetInstance());
    //if (pBot->GetFSM()->PreviousState() == Defend::GetInstance())
    //{
    // pBot->GetFSM()->ChangeState(Defend::GetInstance());
    //}
    //else if (pBot->GetFSM()->PreviousState() == Capture::GetInstance())
    //{
    // pBot->GetFSM()->ChangeState(Capture::GetInstance());
    //}
   }

   //else if (!DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).IsAlive())
   //{
   // pBot->SetKillCount(1);
   // pBot->GetFSM()->RevertToPreviousState();
   //} // if
  //else if (!StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()))
  //{
  // pBot->GetFSM()->RevertToPreviousState();
  //} // elseif
  //else if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()).magnitude() > 400)
  //{
  // pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation(), DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetVelocity(), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));
  //} // elseif
  

  //if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()).magnitude() < 500)
  //{
  // if (pBot->GetAccuracy() >= 0.7)
  // {
  //  pBot->Shoot();
  // } // if
  //} // if
 }

 
} // Attack()


bool Attack::CheckForThreats(Bot *pBot)
{
 bool bThreatDetected = false;
 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).IsAlive() &&
      (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() < 400.0f &&
      StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()))
  {
   bThreatDetected = true;
  }
 }
 return bThreatDetected;
} // CheckForThreats()


void Attack::Exit(Bot* pBot)
{
 pBot->StopAiming();
} // Exit()


void Attack::SetTargetBot(Bot* pBot)
{
 float bestDistance = FLT_MAX;

 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).
      GetLocation()).magnitude() <= 600 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() < bestDistance)
  {
    bestDistance = DynamicObjects::GetInstance()->GetBot(1, i).GetLocation().magnitude();
    pBot->SetTarget(1, i);
  } // if
 } // for
} // SetTargetBot()


void Attack::CheckAmmo(Bot* pBot)
{
 if (pBot->AmmoRemaining() <= 0)
 {
  pBot->GetFSM()->ChangeState(Reload::GetInstance());
 } // if
} // CheckAmmo()


void Attack::CheckTarget(Bot* pBot)
{
 if (!DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).IsAlive())
 {
  pBot->SetKillCount(1);
  pBot->GetFSM()->RevertToPreviousState();
 } // if
 else if (!StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()))
 {
  pBot->GetFSM()->RevertToPreviousState();
 } // elseif
 else if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()).magnitude() > 400)
 {
 	pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation(), DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetVelocity(), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));
 } // elseif
 else if (pBot->GetTargetTeam() != 1)
 {
  pBot->SetTarget(1, pBot->GetTargetBot());
 } // elseif
} // CheckTarget()


void Attack::CheckShoot(Bot* pBot)
{
 if ((pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, pBot->GetTargetBot()).GetLocation()).magnitude() < 500)
 {
  if (pBot->GetAccuracy() >= 0.6)
  {
   pBot->Shoot();
  } // if
 } // if
} // CheckShoot()
 

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
Reload::Reload()
{
 
} // Reload()


Reload::~Reload()
{
 
} // ~Reload()


Reload* Reload::GetInstance()
{
 if (!m_instance)
 {
  m_instance = new Reload;
 }
 return m_instance;
} // GetInstance()


void Reload::Release()
{
 if (m_instance)
 {
  delete m_instance;
  m_instance = NULL;
 }
}


void Reload::Enter(Bot* pBot)
{
 pBot->GetBVR()->m_vPath.clear();
 if (USE_THREADING == true)
 {
  Vector2D target2 = StaticMap::GetInstance()->GetClosestResupplyLocation(pBot->GetLocation());
  pBot->target = target2;
	 pBot->m_bPathFound = false;
	 pBot->m_bSeekPath = true;
	 pBot->GetBVR()->SetBehaviour(1, 0, 0, 1, 0);
 }
 else
 {
  pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);
  SetReloadTarget(pBot);
 }
} // Enter()


void Reload::Execute(Bot* pBot)
{
 if (pBot->IsAlive())
 {
  if (pBot->m_bPathFound)
  {
   pBot->GetBVR()->SetBehaviour(0, 0, 0, 1, 1);
  }

  if (pBot->AmmoRemaining() >= MAXAMMO)
  {
   pBot->GetFSM()->ChangeState(Capture::GetInstance());
  }
  //ClosestEnemy(pBot);
  //CheckAmmo(pBot);

  //// Checking for enemies nearby
  for (int i = 0; i < NUMBOTSPERTEAM; i++)
  {
   if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()) &&
    (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1, i).GetLocation()).magnitude() <= 400 &&
    DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() &&
    pBot->AmmoRemaining() > 0)
   {
    pBot->GetFSM()->ChangeState(Attack::GetInstance());
   } // if
  } // for

 	pBot->UpdateAcceleration(pBot->GetBVR()->AccumulateBehaviour(StaticMap::GetInstance()->GetClosestResupplyLocation(pBot->GetLocation()), Vector2D(0, 0), pBot->GetLocation(), pBot->GetVelocity(), pBot->GetBVR()->GetPath()));
 } // if
 else
 {
  pBot->GetFSM()->ChangeState(Dead::GetInstance()); 
 } // else
} // Execute()


void Reload::Exit(Bot* pBot)
{

} // Exit()


void Reload::SetReloadTarget(Bot* pBot)
{
	pBot->GetBVR()->SetPath(&Graph::GetInstance()->Pathfind(pBot->GetLocation(), StaticMap::GetInstance()->GetClosestResupplyLocation(pBot->GetLocation())));
} // SetTarget()


void Reload::CheckAmmo(Bot* pBot)
{
 if (pBot->AmmoRemaining() == MAXAMMO)
 {
  pBot->GetFSM()->ChangeState(Capture::GetInstance());
 }
} // CheckAmmo


void Reload::ClosestEnemy(Bot* pBot)
{
 for (int i = 0; i < NUMBOTSPERTEAM; i++)
 {
  if (StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()) && (pBot->GetLocation() - DynamicObjects::GetInstance()->GetBot(1 - pBot->GetTeamNumber(), i).GetLocation()).magnitude() <= 400 && DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() && (pBot->AmmoRemaining() > 0))
  {
   pBot->GetFSM()->ChangeState(Attack::GetInstance());
  } // if
 } // for
} // ClosestEnemy()
 