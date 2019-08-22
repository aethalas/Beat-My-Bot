// Title  :  behaviour.h
// Purpose:  Provides the bots with behaviour
// Author :  Jonathon H Tyson
// Date   :  13/10/2017

#pragma once
#include <vector>

enum Deceleration { slow = 3, normal = 2, fast = 1 };

class Vector2D;
class Behaviour
{
private:
 bool m_bSeek;
 bool m_bArrive;  
 bool m_bPursuit;  
 bool m_bAvoidWall;
 bool m_bFollowPath;

public:
 Behaviour();
 std::vector<Vector2D> m_vPath;
 void SetBehaviour(bool seek, bool arrive, bool pursuit, bool wallAvoid, bool followPath);
 void SetPath(std::vector<Vector2D>* pThePath);
 std::vector<Vector2D>* GetPath();
 Vector2D Seek(Vector2D targetPos, Vector2D botPos, Vector2D botVel);
 Vector2D Pursuit(Vector2D targetPos, Vector2D targetVel, Vector2D botPos, Vector2D botVel);
 Vector2D Arrive(Vector2D targetPos, Vector2D botPos, Vector2D botVel, Deceleration botDeceleration);
 Vector2D WallAvoidance(Vector2D botPos, Vector2D botVel);
 Vector2D FollowPath(std::vector<Vector2D>* pThePath, Vector2D botPos, Vector2D botVel);
 Vector2D AccumulateBehaviour(Vector2D targetPos, Vector2D targetVel, Vector2D botPos, Vector2D botVel, std::vector<Vector2D>* pThePath);
};