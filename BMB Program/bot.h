#pragma once
#include "shapes.h"
#include "rules.h"
#include <vector>
#include "graph.h"
#include "behaviour.h"
#include "StateMachine.h"
#include <chrono>

typedef Behaviour BVR;
typedef StateMachine<Bot> FSM;

class Behaviour;
class Bot
{
protected:
 Vector2D targetPoint;			// Used only by placeholder AI. Delete this.
 Vector2D m_Position;			// Current world coordinates
 Vector2D m_Velocity;			// Current velocity
 Vector2D m_Acceleration;
 int      m_iAmmo;					// Not currently used
 int      m_iAimingAtTeam;			// Team number of the bot being aimed at
 int      m_iAimingAtBot;				// Number of the bot being aimed at
 int      m_iOwnTeamNumber;
 int      m_iOwnBotNumber;
 int      m_iHealth;					// Health (up to 100)
 int      m_deathCount;
 int      m_domTarget;
 int      m_killCount;
 float    m_dAccuracy;				// Accuracy of the current firing solution (1==100%)
 float    m_dTimeToCoolDown;		// Countdown until the time the bot can shoot again
 float    m_dTimeToRespawn;		// Countdown until the bot can respawn. If zero or below, bot is alive
 float    m_dDirection;			// Direction the bot is pointing in. radians anticlockwise from south
 bool     m_bFiring;
 bool     m_bAiming;					// If true, bot is aiming and cannot move
 BVR     *m_pBehaviour;
 FSM     *m_pStateMachine;

public:
	Bot();

 bool     m_bSeekPath;
 bool     m_bPathFound;
 Vector2D target;
 int dom;
 std::chrono::high_resolution_clock::time_point t1;
 std::chrono::high_resolution_clock::time_point t2;
 bool m_timerOnce = true;
 float m_timeTaken = 0;
 bool m_useThreading = true;

	// Runs once each frame. Handles physics, shooting, and calls
	// ProcessAI
	void Update(float frametime);

 void Reload();

	// Returns the location of the bot
	Vector2D GetLocation();
	
	// Returns the direction the bot is pointing. In radians anticlockwise from south
	float GetDirection();

	// Restarts the bot in a new location, with full heath, etc
	void PlaceAt(Vector2D position);	
	
	// Returns true if the bot is currently not respawning
	bool IsAlive();					
	
	// This is your function. Use it to set up any states at the beginning of the game
	// and analyse the map.
	// Remember that bots have not spawned yet, so will not be in their
	// starting positions.
	void StartAI();

	// This is your function. Use it to set the orders for the bot.
	// Will be called once each frame from Update
	void ProcessAI();	

	// This is a quick n' dirty AI for team 2.
	// Try to make team 1 better than this.
	// Will be called once each frame from Update
	void ProcessAIBadly();
			
	// Returns the number of the team of the bot being aimed at.
	// Returns a negative number if no bot is being aimed at.
	int GetTargetTeam();	

	// Returns the number of the bot being aimed at.
	// Returns a negative number if no bot is being aimed at.
	int GetTargetBot();

	// Sets the bots own team number and bot number.
	// No need to call this
	void SetOwnNumbers(int teamNo, int botNo);

	// Returns the current health of the bot
	int GetHealth();

	// Returns the current accuracy of the bot.
	// Accuracy is the probability of hitting the current target.
	// If the bot is not aiming, this will be zero.
	double GetAccuracy();

	// Sets the target of the current bot.
	// This will reset the accuracy to zero if the target bot 
	// has changed.
	// It also sets the bot to aiming mode, which means it will stop
	// moving.
	// If you want the bot to stop aiming, just set mbAiming = false
	// or use the StopAiming method
	void SetTarget(int targetTeamNo, int targetBotNo);

	// Call this to set the bot to shoot, if it can.
	// Once a shot it taken, the bot will not shoot again unless told to do so.
	void Shoot();

	// Delivers the specified damage to the bot.
	// If this drops health below zero, the bot will die
	// and will respawn at the spawn point after a specified interval
	void TakeDamage(int amount);

	// Stops the bot from aiming, so it can move again
	void StopAiming();

 // Returns the cooldown to firing
 float TimeToCooldDown() const;

 // Gets the behaviour of the bot as a component
 Behaviour* GetBVR() const;

 // Get the bots Finite State Machine
 StateMachine<Bot>* GetFSM() const;

 // Gets the team number of the bot
 int GetTeamNumber() const;

 // Gets the bots identifying number
 int GetBotNumber() const;

 // This function returns the amount of ammunition the bot currently has.
 int AmmoRemaining() const;

 // Gets the velocity of the bot
 Vector2D GetVelocity() const;

 // This function is used to draw the bots time to respawn to the screen for
 // illustration purposes only. Attaches to the UI. 
 float GetTimeToRespawn() const;

 // Handles the acceleration of the bot
 void UpdateAcceleration(Vector2D newAcceleration);

 // Retrieves the amount of deaths the bot has accumulated.
 int GetDeathCount() const;

 // Increments the death account by a requested amount. God knows why you'd want
 // to add more than one, but ah well.
 void SetDeathCount(int amount);

 // Retrieves the amount of kills the bot has accumulated.
 int GetKillCount() const;

 // Increments the kill account by a requested amount.
 void SetKillCount(int amount);

 // Retrieves bots target domination point.
 int GetTargetDomPoint() const;

 // Sets the bots target domination point.
 void SetTargetDomPoint(int amount);
};