#include "Network.h"
#include "game.h"
#include "staticmap.h"
#include "Renderer.h"
#include "errorlogger.h"
#include "myinputs.h"
#include "dynamicobjects.h"
#include "mydrawengine.h"
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include "AStarThread.h"

Game* Game::pInst = NULL;

Game* Game::GetInstance()
{
	if(pInst == NULL)
	{
		pInst= new Game();
	}

	return pInst;
}


void Game::Release()		// Static
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}


Game::Game()
{
	InitialiseScript();
}

Game::~Game()
{
	
}

ErrorType Game::Start()
{
 Network::GetInstance()->StartWinsock();
	m_timer.mark();
	m_timer.mark();
	m_State = RUNNING;
 if (MAKE_SERVER == true)
 {
  Network::GetInstance()->StartServer(PORT);
 }
 else
 {
  Network::GetInstance()->StartClient(IPADDRESS, PORT);
 }
	return SUCCESS;
}

ErrorType Game::RunInterface()
{

	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();

	// Handle mouse
	static int mouseX = 0;
	static int mouseY = 0;
	static float screenCentreX = 0.0f;
	static float screenCentreY = 0.0f;
	static float zoom = 0.5f;
	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleMouse();

	mouseX += int(pInputs->GetMouseDX()/zoom);
	mouseY -= int(pInputs->GetMouseDY()/zoom);

	if(mouseX<screenCentreX-200/zoom)
		screenCentreX-=((screenCentreX-200/zoom)-mouseX)/(50*zoom);
	if(mouseX>screenCentreX+200/zoom)
		screenCentreX+=(mouseX-(screenCentreX+200/zoom))/(50*zoom);
	if(mouseY>screenCentreY+200/zoom)
    screenCentreY += (mouseY - (screenCentreY+200/zoom)) / (50 * zoom);
	if(mouseY<screenCentreY-200/zoom)
    screenCentreY -= ((screenCentreY-200/zoom)-mouseY) / (50 * zoom);

	// Handle viewport
	pTheRenderer->SetViewScale(zoom);
	pTheRenderer->SetViewCentre(Vector2D(screenCentreX,screenCentreY));

	pTheRenderer->DrawMousePointer( Vector2D(float(mouseX), float(mouseY)));

	if(pInputs->GetMouseDZ()>0)
		zoom*=1.05f;
	if(pInputs->GetMouseDZ()<0)
		zoom/=1.05f;

  //// Server Output
  Network::GetInstance()->NetworkInfo();
  AStarThread::GetInstance()->ThreadInfo();

	// Display the scores
	for(int i = 0; i < NUMTEAMS; i++)
	{
		int score = (DynamicObjects::GetInstance()->GetScore(i));
		Vector2D pos(30.0f, i*25.0f+50.0f);
		pTheRenderer->DrawTextAt(pos, L"Team ", _XRGB(181, 183, 136));
		pos.set(95.0f, i*25.0f+50.0f);
		pTheRenderer->DrawNumberAt(pos, i+1);
		pos.set(135.0f, i*25.0f+50.0f);
		pTheRenderer->DrawNumberAt(pos, score);
	}

 MyDrawEngine::GetInstance()->DrawAt(Vector2D(13.0f, 62.0f), MyDrawEngine::GetInstance()->LoadPicture(L"team1.bmp"), 0.6f, 0.0f, 0.0f);
 MyDrawEngine::GetInstance()->DrawAt(Vector2D(13.0f, 87.0f), MyDrawEngine::GetInstance()->LoadPicture(L"team2.bmp"), 0.6f, 0.0f, 0.0f);

 // Give the locations some names!
 MyDrawEngine::GetInstance()->WriteText(Vector2D(-285, -875), L"Alexandria", _XRGB(181, 183, 136), 0);
 MyDrawEngine::GetInstance()->WriteText(Vector2D(-65, -25), L"Eagrose", _XRGB(181, 183, 136), 0);
 MyDrawEngine::GetInstance()->WriteText(Vector2D(125, 825), L"Nibelheim", _XRGB(181, 183, 136), 0);

 // Display bot states
	for(int i = 0; i < NUMBOTSPERTEAM; i++)
	{
		Bot state = (DynamicObjects::GetInstance()->GetBot(0, i));
		Vector2D pos(10.0f, (i * 150.0f) + 120.0f);
		pTheRenderer->DrawTextAt(pos, L"Bot    ::", _XRGB(61, 77, 92));
		pos.set(43.0f, (i * 150.0f) + 120.0f);
		pTheRenderer->DrawNumberAt(pos, i+1);

		pos.set(75.0f, (i * 150.0f) + 120.0f);

  // Bot State
  if (state.GetFSM()->CurrentState() == Capture::GetInstance())
  {
   pTheRenderer->DrawTextAt(pos, L"CAPTURING", _XRGB(60, 115, 57));
  }
  else if (state.GetFSM()->CurrentState() == Defend::GetInstance())
  {
   pTheRenderer->DrawTextAt(pos, L"DEFENDING", _XRGB(57, 89, 115));
  }
  else if (state.GetFSM()->CurrentState() == Dead::GetInstance())
  {
   pTheRenderer->DrawTextAt(pos, L"DEAD", _XRGB(77, 77, 77));
  }
  else if (state.GetFSM()->CurrentState() == Reload::GetInstance())
  {
   pTheRenderer->DrawTextAt(pos, L"RELOADING", _XRGB(112, 57, 115));
  }
  else if (state.GetFSM()->CurrentState() == Attack::GetInstance())
  {
   pTheRenderer->DrawTextAt(pos, L"ATTACKING", _XRGB(115, 57, 57));
  }

  // Kill & Death Count
  if (state.IsAlive())
  {
   MyDrawEngine::GetInstance()->DrawAt(pos + Vector2D(-30.0f, 42.0f), MyDrawEngine::GetInstance()->LoadPicture(L"bullet.bmp"), 0.6f, 0.0f, 0.0f);
   pTheRenderer->DrawTextAt(pos + Vector2D(-15.0f, 30.0f), L"Ammo: ", _XRGB(107, 107, 107));
   pTheRenderer->DrawNumberAt(pos + Vector2D(65.0f, 30.0f), state.AmmoRemaining());

   MyDrawEngine::GetInstance()->DrawAt(pos + Vector2D(-30.0f, 72.0f), MyDrawEngine::GetInstance()->LoadPicture(L"kills.bmp"), 0.6f, 0.0f, 0.0f);
   pTheRenderer->DrawTextAt(pos + Vector2D(-15.0f, 60.0f), L"Health: ", _XRGB(107, 107, 107));
   pTheRenderer->DrawNumberAt(pos + Vector2D(65.0f, 60.0f), state.GetHealth()/*state.GetKillCount()*/);

   MyDrawEngine::GetInstance()->DrawAt(pos + Vector2D(-30.0f, 102.0f), MyDrawEngine::GetInstance()->LoadPicture(L"deaths.bmp"), 0.6f, 0.0f, 0.0f);
   pTheRenderer->DrawTextAt(pos + Vector2D(-15.0f, 90.0f), L"Acc: ", _XRGB(107, 107, 107));
   pTheRenderer->DrawNumberAt(pos + Vector2D(65.0f, 90.0f), state.GetAccuracy());
  }
  else
  {
   pTheRenderer->DrawTextAt(pos + Vector2D(0.0f, 40.0f), L"Respawning in... ", _XRGB(77, 77, 77));
   pTheRenderer->DrawNumberAt(pos + Vector2D(40.0f, 60.0f), (int)state.GetTimeToRespawn());
  }
	}

	return answer;
}



ErrorType Game::Update()
{

	m_timer.mark();
	// Cap the frame time
	if(m_timer.m_fFrameTime>0.05f)
		m_timer.m_fFrameTime=0.05f;
	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();

	// If not paused or minimised
	if(m_State == RUNNING)
	// Update Dynamic objects
	{
  DynamicObjects::GetInstance()->Update(m_timer.m_fFrameTime);
  Network::GetInstance()->UpdateNetwork(DynamicObjects::GetInstance(), sizeof(DynamicObjects));
	}

	// Render
	if(pTheRenderer->GetIsValid()==false)
	{
		m_State = MINIMISED;			// Or some other reason why invalid
	}
	else
	{
		if(m_State == MINIMISED)
			m_State = PAUSED;			// No longer minimised. Return to pause state.
		if(StaticMap::GetInstance()->Render()==FAILURE)
		{
			ErrorLogger::Writeln(L"Static map failed to render. Terminating.");
			answer = FAILURE;
		}
		if(DynamicObjects::GetInstance()->Render()==FAILURE)
		{
			ErrorLogger::Writeln(L"Dynamic objects failed to render. Terminating.");
			answer = FAILURE;
		}

		if(pTheRenderer->DrawFX(m_timer.m_fFrameTime)==FAILURE)
		{
			ErrorLogger::Writeln(L"Render failed to draw FX.");
			// Continue. Non-critical error.
		}
		if(pTheRenderer->EndScene()==FAILURE)
		{
			ErrorLogger::Writeln(L"Render failed to end scene. Terminating.");
			answer = FAILURE;
		}
	}

	if(RunInterface()==FAILURE)
	{
		ErrorLogger::Writeln(L"Interface reports failure.");
		// Non-critical error
	}

	return answer;
}

ErrorType Game::End()
{
 AStarThread::GetInstance()->StopThread();
	Renderer::Release();
	StaticMap::Release();
 Dead::Release();
 Capture::Release();
 Defend::Release();
 Attack::Release();
 Reload::Release();
	DynamicObjects::Release();
 delete Graph::GetInstance();
 AStarThread::Release();
	Network::GetInstance()->CloseWinsock();
	return SUCCESS;
}


ErrorType Game::InitialiseScript()
{
	// Hard code for now
	StaticMap* pMap = StaticMap::GetInstance();
 
	// Add some rectangles
	Rectangle2D r;
	r.PlaceAt(1000, -1000, 700, -700);
	pMap->AddBlock(r);
  r.PlaceAt(-700, 700, -1000, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(1000, -700, 900, 0);
  pMap->AddBlock(r);
  r.PlaceAt(-900, 0, -1000, 700);
  pMap->AddBlock(r);

  r.PlaceAt(-700, -1000, -1400, -700);
  pMap->AddBlock(r);
  r.PlaceAt(1400, 700, 700, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(300, -1000, -300, -850);
  pMap->AddBlock(r);
  r.PlaceAt(300, 850, -300, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(500, -450, 200, -150);
  pMap->AddBlock(r);
  r.PlaceAt(-200, 150, -500, 450);
  pMap->AddBlock(r);

  r.PlaceAt(-200, -500, -750, -400);
  pMap->AddBlock(r);
  r.PlaceAt(750, 400, 200, 500);
  pMap->AddBlock(r);

  r.PlaceAt(700, -200, 600, 200);
  pMap->AddBlock(r);
  r.PlaceAt(-600, -200, -700, 200);
  pMap->AddBlock(r);
 

	// Add two spawn points
	pMap->AddSpawnPoint(Vector2D(-1250, 0));
	pMap->AddSpawnPoint(Vector2D(1250, 00));

 // Add four supply points
 pMap->AddSupplyPoint(Vector2D(900, 500));
 pMap->AddSupplyPoint(Vector2D(900, -500));
 pMap->AddSupplyPoint(Vector2D(-900, 500));
 pMap->AddSupplyPoint(Vector2D(-900, -500));

	// Place the domination points
	DynamicObjects* pDynObjects = DynamicObjects::GetInstance();

	pDynObjects->PlaceDominationPoint(Vector2D( -200,-850 ));
	pDynObjects->PlaceDominationPoint(Vector2D( 0,0 ));
	pDynObjects->PlaceDominationPoint(Vector2D( 200,850 ));


	pDynObjects->Initialise();


	return SUCCESS;
}