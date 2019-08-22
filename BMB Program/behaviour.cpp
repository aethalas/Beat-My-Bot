// Title  :  Behaviour.cpp
// Purpose:  Provides the Bots with different Behaviour Patterns
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

#include "behaviour.h"
#include "rules.h"
#include "Shapes.h"
#include "staticmap.h"
#include "Renderer.h"
#include "mydrawengine.h"

Behaviour::Behaviour()
{
 m_bSeek = false;
 m_bArrive = false; 
 m_bPursuit = false; 
 m_bAvoidWall = false;
 m_bFollowPath = false;
} // SteeringBehaviour()


void Behaviour::SetBehaviour(bool seek, bool arrive, bool pursuit, bool wallAvoid, bool followPath)
{
 m_bSeek = seek;
 m_bArrive = arrive; 
 m_bPursuit = pursuit;  
 m_bAvoidWall = wallAvoid;
 m_bFollowPath = followPath;
} // SetBehaviours()


std::vector<Vector2D>* Behaviour::GetPath()
{
 return &m_vPath;
} // GetPath()


void Behaviour::SetPath(std::vector<Vector2D>* pThePath)
{
 m_vPath = *pThePath;
} // SetPath()


Vector2D Behaviour::Seek(Vector2D targetPos, Vector2D botPos, Vector2D botVel)
{
 Vector2D desiredVelocity = (targetPos - botPos) * MAXBOTSPEED;
 return (desiredVelocity - botVel) * MAXIMUMACCELERATION;
} // Seek()


Vector2D Behaviour::Pursuit(Vector2D targetPos, Vector2D targetVel, Vector2D botPos, Vector2D botVel)
{
 float distance = (targetPos - botPos).magnitude();
 float time = distance / MAXBOTSPEED;

 targetPos += targetVel * time;

 return Seek(targetPos, botPos, botVel);
} // Pursuit()


Vector2D Behaviour::Arrive(Vector2D targetPos, Vector2D botPos, Vector2D botVel, Deceleration botDeceleration)
{
 float toTarget = (targetPos - botPos).magnitude();
 
 if (toTarget > 0.0f)
 {
  //const float DecelerationTweaker = 0.3f;	
  const float DecelerationTweaker = 1.0f;
  float botSpeed = toTarget / ((float)botDeceleration * DecelerationTweaker);

  botSpeed = min(botSpeed, MAXBOTSPEED);

  Vector2D desiredVelocity = (targetPos - botPos) * botSpeed / toTarget;
  return (desiredVelocity - botVel);
 } // if
 return Vector2D(0,0);
} // Arrive()


Vector2D Behaviour::WallAvoidance(Vector2D botPos, Vector2D botVel)
{
 // Creates a circle around each of the bots that, when inside of a block,
 // allows the behaviour to push the bot away from any obstacles.
 Circle2D b1(botPos, 45.0f);
 Vector2D desiredVelocity;
 //desiredVelocity.set(0, 0);
  
 if (StaticMap::GetInstance()->IsInsideBlock(b1))
 {
  //this->m_bSeek = false;
  //this->m_bFollowPath = false;
  MyDrawEngine::GetInstance()->FillCircle(botPos, 45.0f, _XRGB(18, 18, 18));
  desiredVelocity += 150.0f * StaticMap::GetInstance()->GetNormalToSurface(b1);
 } // if

 return desiredVelocity;
} // WallAvoidance()


Vector2D Behaviour::FollowPath(std::vector<Vector2D>* pThePath, Vector2D botPos, Vector2D botVel)
{
 Renderer::GetInstance()->DrawLine(botPos, pThePath->back(), _XRGB(29, 59, 61));

 if (pThePath->size() == 1)
 { // If the stack is at the last element, then slow down to ensure the bot
   // hits the target area.
  return Arrive(pThePath->back(), botPos, botVel, fast);
 } //if
 else if (!pThePath->empty())
 {
  if ((StaticMap::GetInstance()->IsLineOfSight(botPos, (*pThePath)[pThePath->size() - 2])) && (botPos - pThePath->back()).magnitude() < 180)
  {
   pThePath->pop_back();
  }
  return Arrive(pThePath->back(), botPos, botVel, normal);
 } //elseif
 else
 {
  return Seek(pThePath->back(), botPos, botVel);
 } //else
} // FollowPath()


Vector2D Behaviour::AccumulateBehaviour(Vector2D targetPos, Vector2D targetVel, Vector2D botPos, Vector2D botVel, std::vector<Vector2D>* pThePath)
{
 Vector2D newAccel;

 if (m_bSeek)
 {
  newAccel += Seek(targetPos, botPos, botVel);
 } //if

 if (m_bArrive)
 {
  newAccel += Arrive(targetPos, botPos, botVel, fast);
 } // if

 if (m_bPursuit)
 {
  newAccel += Pursuit(targetPos, targetVel, botPos, botVel);
 } // if

 if (m_bAvoidWall)
 {
  newAccel += WallAvoidance(botPos, botVel);
 } // if

 if (m_bFollowPath)
 {
  newAccel += FollowPath(pThePath, botPos, botVel);
 } // if

 return newAccel;
} // AccumulateBehaviour()