// Title  :  state.h
// Purpose:  Provides the Bots with a state
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

#include <fstream>
#include <chrono>

#pragma once

template <class EntityType>
class State
{
private:

public:
 virtual void Enter(EntityType*) = 0;
 virtual void Execute(EntityType*) = 0;
 virtual void Exit(EntityType*) = 0;
};



class Bot;
class Dead : public State<Bot>
{
private:
 Dead();

 static Dead* m_instance;

public:
 ~Dead();
 static Dead* GetInstance();
 static void Release();
 void Enter(Bot* pBot);
 void Execute(Bot* pBot);
 void Exit(Bot* pBot);
};



class Capture : public State<Bot>
{
private:
 Capture();

 static Capture* m_instance;
 std::ofstream captureLog;

public:
 ~Capture();
 static Capture* GetInstance();
 static void Release();
 void Enter(Bot* pBot);
 void Execute(Bot* pBot);
 void Exit(Bot* pBot);
 void SetDomTarget(Bot* pBot);
 void CheckDomPoint(Bot* pBot);
 void ClosestEnemy(Bot* pBot);
};



class Defend : public State<Bot>
{
private:
 Defend();
 static Defend* m_instance;

public:
 ~Defend();
 static Defend* GetInstance();
 static void Release();
 void Enter(Bot* pBot);
 void Execute(Bot* pBot);
 void Exit(Bot* pBot);
 void SetDomTarget(Bot* pBot);
 void CheckDomPoint(Bot* pBot);
 void ClosestEnemy(Bot* pBot);
 void CheckDistance(Bot* pBot);
 bool CheckForThreats(Bot *pBot);
};



class Attack : public State<Bot>
{
private:
 Attack();
 static Attack* m_instance;

public:
 ~Attack();
 static Attack* GetInstance();
 static void Release();
 void Enter(Bot* pBot);
 void Execute(Bot* pBot);
 void Exit(Bot* pBot);
 void SetTargetBot(Bot* pBot);
 void CheckAmmo(Bot* pBot);
 void CheckTarget(Bot* pBot);
 void CheckShoot(Bot* pBot);
 bool CheckForThreats(Bot *pBot);
};



class Reload : public State<Bot>
{
private:
 Reload();
 static Reload* m_instance;

public:
 ~Reload();
 static Reload* GetInstance();
 static void Release();
 void Enter(Bot* pBot);
 void Execute(Bot* pBot);
 void Exit(Bot* pBot);
 void SetReloadTarget(Bot* pBot);
 void CheckAmmo(Bot* pBot);
 void ClosestEnemy(Bot* pBot);
};
