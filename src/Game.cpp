#include "Component_Physical.h"
#include "ICommon.h"
#include "IFont.h"
#include "EntityFactory.h"
#include "EntityManager.h"
#include "Game.h"
#include "IGraphics.h"
#include "GUIManager.h"
#include "Level.h"
#include "IResourceManager.h"
#include "ITexture.h"

#include "MutexLock.h"
#include "VersionNumber.h"

#include "StringUtils.h"

CGame game_local;

CGame::CGame()
{
	gameState = EGameState::NONE;
	playerEntity = NULL;
}

CGame::~CGame()
{
	delete playerEntity;
}

void CGame::Initialize()
{
	gameState = EGameState::MENU;

	common->Printf("PonyTerra %d.%d.%d.%d\n", PONYTERRA_MAJOR, PONYTERRA_MINOR, PONYTERRA_REV, PONYTERRA_BUILD);

	PreloadMenuData();

	level.Initialize();

	guiManager.Initialize();
	guiManager.Push(GetGUI("MenuFront"));
}

void CGame::InitializeGame()
{
	gameState = EGameState::INGAME;
	playerEntity = entityFactory.CreatePlayer();
	entityMgr.AddEntity(playerEntity);
}

void CGame::Quit()
{
}

const char *CGame::GetGameName()
{
	return "PonyTerra";
}

void CGame::PreloadMenuData()
{
	resMgr->GetTexture("data/res/tex/menu_bg.png");
	resMgr->GetFont("data/res/font/menu.fnt");
}

void CGame::ReleaseMenuData()
{
	resMgr->ClearCache("data/res/tex/menu_bg.png");
	resMgr->ClearCache("data/res/font/menu.fnt");
}

void CGame::Update(float dtTime)
{
	bool generating = false;

	{
		CMutexLock lock(&levelGenerate.stateLock);
		generating = levelGenerate.isGenerating;
	}

	if (!generating && level.IsLoaded()) {
		try {
			if (levelGenThread->joinable()) {
                levelGenThread->join();
                //delete levelGenThread;
				levelGenerate.isGenerating = false;
			}
		}
        catch(std::exception &e) {
            common->Error("%s\n", e.what());
		}

		InitializeGame();
	}

	switch(gameState) {
		case EGameState::MENU:
			guiManager.Update(dtTime);
			break;

		case EGameState::INGAME:
			entityMgr.UpdateAll(dtTime);
			break;
	}
}

void CGame::Draw()
{
	switch(gameState) {
		case EGameState::MENU:
			guiManager.Draw();
			break;

		case EGameState::INGAME:
			level.Draw();

			CComponent_Physical *playerPhysical = playerEntity->GetComponents()->Get<CComponent_Physical>(COMPONENT_PHYSICAL);

			graphics->DrawText(
				resMgr->GetFont("data/res/font/sys.fnt"), 
				CVector2f(0.0f, (float)graphics->GetHeight() - 20.0f), 
				CColor::white, 
				StrUtl::FormatString("Player pos: %f:%f",
				playerPhysical->rect.pos.x,
				playerPhysical->rect.pos.y).c_str());

			break;
	}
}

CRect CGame::GetViewRect() const
{
	if(gameState == EGameState::INGAME) {

		return CRect(
			playerEntity->GetComponents()->Get<CComponent_Physical>(COMPONENT_PHYSICAL)->rect.pos,
			graphics->GetWidth(), 
			graphics->GetHeight());

	} else {

		return CRect();

	}
}

CEntity *CGame::GetPlayerEntity() const
{
	return playerEntity;
}