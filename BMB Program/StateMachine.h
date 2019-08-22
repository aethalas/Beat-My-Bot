// Title  :  StateMachine.h
// Purpose:  Provides the BMB Program with a State Machine
// Author :  Jonathon H Tyson
// Date   :  09/11/2017

#pragma once
#include "state.h"

template <class EntityType>
class StateMachine  
{
private:
 EntityType* m_pOwner;
 State<EntityType>* m_pCurrentState;
 State<EntityType>* m_pGlobalState;
 State<EntityType>* m_pPreviousState;

public:
 StateMachine(EntityType *pOwner):m_pOwner(pOwner),
                                 m_pCurrentState(nullptr),
                                 m_pGlobalState(nullptr),
                                 m_pPreviousState(nullptr)
 {}

 void SetCurrentState(State<EntityType>* pState)
 {
  m_pCurrentState = pState;
 }

 void SetGlobalState(State<EntityType>* pState)
 {
  m_pGlobalState = pState;
 }

 void SetPreviousState(State<EntityType>* pState) 
 {
  m_pPreviousState = pState;
 }

 void Update() const
 {
  if (m_pCurrentState)
  {
   m_pCurrentState->Execute(m_pOwner);
  }
 }

 void ChangeState(State<EntityType>* pNewState)
 {
  m_pPreviousState = m_pCurrentState;
  m_pCurrentState->Exit(m_pOwner);
  m_pCurrentState = pNewState;
  m_pCurrentState->Enter(m_pOwner);
 }

 void RevertToPreviousState()
 {
  ChangeState(m_pPreviousState); 
 }

 State<EntityType>* CurrentState() const
 {
  return m_pCurrentState; 
 }

 State<EntityType>* GlobalState() const
 {
  return m_pGlobalState;
 }

 State<EntityType>* PreviousState() const
 {
  return m_pPreviousState;
 }

 bool isInState(const State<EntityType>& st) const
 {
  return 0;
 }
};