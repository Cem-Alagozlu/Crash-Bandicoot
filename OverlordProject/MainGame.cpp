#include "stdafx.h"
#include "MainGame.h"
#include "GeneralStructs.h"
#include "SceneManager.h"
#include "PhysxProxy.h"
#include "DebugRenderer.h"
#include "Game/Menu/MenuMain.h"
#include "Game/Menu/MenuQuit.h"
#include "Game/Menu/MenuExtra.h"
#include "Game/Level/Level.h"
#include "Prefabs/Player/PlayerInventory.h"

MainGame::MainGame(void)
{}

MainGame::~MainGame(void)
{
	
}

void MainGame::OnGamePreparing(GameSettings& gameSettings)
{
	UNREFERENCED_PARAMETER(gameSettings);
}

void MainGame::Initialize()
{
	//PlayerInventory::GetInstance()->Initialize();

	SceneManager::GetInstance()->AddGameScene(new MenuMain());
	SceneManager::GetInstance()->AddGameScene(new MenuExtra());
	SceneManager::GetInstance()->AddGameScene(new MenuQuit());
	SceneManager::GetInstance()->AddGameScene(new Level());
	//
	SceneManager::GetInstance()->SetActiveGameScene(L"MenuMain");
}

LRESULT MainGame::WindowProcedureHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(message);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);



	switch (message)
	{
		case WM_KEYUP:
		{
			if ((lParam & 0x80000000) != 0x80000000)
				return -1;

			//NextScene
			if (wParam == VK_F3)
			{
				//SceneManager::GetInstance()->NextScene();
				return 0;
			}
			//PreviousScene
			else if (wParam == VK_F2)
			{
				//SceneManager::GetInstance()->PreviousScene();
				return 0;
			}
			else if (wParam == VK_F4)
			{
				DebugRenderer::ToggleDebugRenderer();
				return 0;
			}
			else if (wParam == VK_F6)
			{
				auto activeScene = SceneManager::GetInstance()->GetActiveScene();
				activeScene->GetPhysxProxy()->NextPhysXFrame();
			}
		}
	}

	return -1;
}
