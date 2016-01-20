////////////////////////////////////////////////////////////////////////////////
//Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
//This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
//tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
//for details. LEVER 3-D is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by the Free
//Software Foundation, either version 3 of the License, or (at your option) any
//later version.
//LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
//WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
//A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along with
//LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#include "Globals.h"
#include "MessageProcessor.h"
#include "Initialization.h"
#include <time.h>
#include "DirectXCommands.h"
#include "MexErrorMsg.h"

bool gRendererOn = false;
bool gPlay = false;
bool gRotate = false;
bool gCapture = false;
float gFramesPerSec = 5;
float numAngles = 720;
const DirectX::XMMATRIX ROT_X = DirectX::XMMatrixRotationY(-1/(numAngles)*DirectX::XM_2PI);

LRESULT CALLBACK wndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if(message==WM_DESTROY)
		PostQuitMessage(0);

	// Current mouse position
	int iMouseX = (short)LOWORD(lParam);
	int iMouseY = (short)HIWORD(lParam);
	HMENU hmenu = GetMenu(hWnd);

	static bool leftButtonDown = false;
	static bool shiftDown = false;
	static bool ctrlDown = false;
	static bool altDown = false;
	static int previousMouseX = 0;
	static int previousMouseY = 0;
	static DirectX::XMMATRIX previousWorldRotation;
	static float previousPeel;
	static bool minimized = false;
	static bool hullsOn = true;

	int label;
	Vec<float> pnt;
	Vec<float> direction;

	switch(message)
	{
	case WM_NULL:
		break;
	case WM_CREATE:
		break;
	case WM_MOVE:
		break;
	case WM_SIZE:
		if(wParam == SIZE_MINIMIZED)
		{
			gRendererOn = false;
			minimized = true;
			break;
		}
		else if(wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && minimized))
		{
			gRendererOn = true;
			minimized = false;
		}

		gWindowWidth = LOWORD(lParam);
		gWindowHeight = HIWORD(lParam);
		if(gRenderer != NULL)
		{
			gRenderer->resizeViewPort();
			gRenderer->renderAll();
		}
		break;
	case WM_MOUSEWHEEL:
		if(GET_WHEEL_DELTA_WPARAM(wParam)>0)
			gCameraDefaultMesh->zoomIncrement();
		else
			gCameraDefaultMesh->zoomDecrement();
		gRenderer->renderAll();
		break;
	case WM_MOUSEMOVE:
		if(leftButtonDown)
		{
			DirectX::XMMATRIX rotX,rotY;
			if(shiftDown)
				rotX = DirectX::XMMatrixIdentity();
			else
				rotX = DirectX::XMMatrixRotationY(((float)(iMouseX - previousMouseX) / gWindowWidth)*DirectX::XM_2PI);

			if(ctrlDown)
				rotY = DirectX::XMMatrixIdentity();
			else
				rotY = DirectX::XMMatrixRotationX((-(float)(iMouseY - previousMouseY) / gWindowHeight)*DirectX::XM_2PI);

			gRenderer->setWorldRotation(previousWorldRotation*rotX*rotY);
			gRenderer->renderAll();
		}
		break;
	case WM_LBUTTONDOWN:
		leftButtonDown = true;
		previousMouseX = iMouseX;
		previousMouseY = iMouseY;
		previousWorldRotation = gRenderer->getRootWorldRotation();
		previousPeel = gRenderer->getClipChunkPercent();
		if(previousPeel>0.2f)
			gRenderer->setClipChunkPercent(0.1f);
		break;
	case WM_RBUTTONDOWN:
		gCameraDefaultMesh->getRay(iMouseX,iMouseY,pnt,direction);
		label = gRenderer->getPolygon(pnt,direction);
		if(ctrlDown)
		{
			gMexMessageQueueOut.addMessage("cellSelected",label);
		}
		else
		{
			gMexMessageQueueOut.addMessage("rightClick",label);
		}
		ctrlDown = false;
		gRenderer->renderAll();
		break;
	case WM_LBUTTONUP:
		leftButtonDown = false;
		gRenderer->setClipChunkPercent(previousPeel);
		gRenderer->renderAll();
		break;
	case WM_KEYDOWN:
		if(VK_LEFT==wParam)
		{
			gCameraDefaultMesh->moveLeft();
			gRenderer->renderAll();
		}
		else if(VK_RIGHT==wParam)
		{
			gCameraDefaultMesh->moveRight();
			gRenderer->renderAll();
		}
		else if(VK_UP==wParam)
		{
			gCameraDefaultMesh->moveUp();
			gRenderer->renderAll();
		}
		else if(VK_DOWN==wParam)
		{
			gCameraDefaultMesh->moveDown();
			gRenderer->renderAll();
		}
		else if(VK_PRIOR==wParam) //Page Up key
		{
			gRenderer->decrementFrame();
			gRenderer->renderAll();
		}
		else if(VK_NEXT==wParam) //Page Down key
		{
			gRenderer->incrementFrame();
			gRenderer->renderAll();
		}
		else if(VK_HOME==wParam)
		{
			gRenderer->setCurrentFrame(0);
			gRenderer->renderAll();
		}
		else if(VK_END==wParam)
		{
			gRenderer->setCurrentFrame(gRenderer->getNumberOfFrames() - 1);
			gRenderer->renderAll();
		}
		else if(VK_SPACE==wParam)
		{
			gPlay = !gPlay;
			gMexMessageQueueOut.addMessage("play",gPlay);
		}
		else if(VK_SHIFT==wParam)
		{
			shiftDown = true;
			gMexMessageQueueOut.addMessage("keyDown","shift");
		}
		else if(VK_CONTROL==wParam)
		{
			ctrlDown = true;
			gMexMessageQueueOut.addMessage("keyDown","ctrl");
		}
		else if(VK_MENU == wParam)
		{
			altDown = true;
			gMexMessageQueueOut.addMessage("keyDown","alt");
		}
		else if('C' == wParam)
		{
			gCameraDefaultMesh->resetCamera();
			gRenderer->renderAll();
		}
		else if('H' == wParam)
		{
			hullsOn = !hullsOn;
			gMexMessageQueueOut.addMessage("togglePolygons",double(hullsOn));

			for(std::map<int,GraphicObjectNode*>::iterator it=gGraphicObjectNodes[GraphicObjectTypes::Polygons].begin();
				it!=gGraphicObjectNodes[GraphicObjectTypes::Polygons].end(); ++it)
			{
				it->second->setRenderable(hullsOn);
			}

			gRenderer->updateRenderList();
			gRenderer->renderAll();
		}
		else if('L'==wParam)
		{
			gRenderer->toggleLabels();
			gMexMessageQueueOut.addMessage("toggleLabels",0.0);
			gRenderer->renderAll();
		}
		else if('P' == wParam)
		{
			if(ctrlDown)
			{
				gRenderer->captureWindow(NULL);
				gRenderer->renderAll();
			}
		}
		else if('R' == wParam)
		{
			gRenderer->resetRootWorldTransform();
			gRenderer->renderAll();
		}
		else if('S' == wParam)
		{
			gRotate = !gRotate;
			gMexMessageQueueOut.addMessage("rotate",gRotate);
		}
		else if('X'==wParam)
		{
			gMexMessageQueueOut.addMessage("centerSelectedCell",1.0);
			gRenderer->renderAll();
		}
		else if('1' == wParam || VK_NUMPAD1 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a 1080p movie
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,1920 + 16,1080 + 39,SWP_NOMOVE);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a 4k movie (UHD = 3840 x 2160)
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,3840 + 16,2160 + 39,0);
			}
			{
				gMexMessageQueueOut.addMessage("keyDown","number",1.0);
			}
		}
		else if('2' == wParam || VK_NUMPAD2 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a two panel 1080p movie
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,1920 / 2 + 16,1080 + 39,SWP_NOMOVE);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a two panel 4k movie (UHD = 3840 x 2160)
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,3840 / 2 + 16,2160 + 39,0);
			}
			{
				gMexMessageQueueOut.addMessage("keyDown","number",2.0);
			}
		}
		else if('3' == wParam || VK_NUMPAD3 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a three panel 1080p movie
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,1920 / 3 + 16,1080 + 39,SWP_NOMOVE);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a three panel 4k movie (UHD = 3840 x 2160)
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,3840 / 3 + 16,2160 + 39,0);
			}
			{
				gMexMessageQueueOut.addMessage("keyDown","number",3.0);
			}
		}
		else if('4' == wParam || VK_NUMPAD4 == wParam)
		{
			if(shiftDown && ctrlDown && !altDown)
			{
				// make window size appropriate for a four panel 1080p movie
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,1920 / 2 + 16,1080 / 2 + 39,SWP_NOMOVE);
			}
			else if(ctrlDown && altDown && !shiftDown)
			{
				// make window size appropriate for a four panel 4k movie (UHD = 3840 x 2160)
				SetWindowPos(gWindowHandle,HWND_NOTOPMOST,0,0,3840 / 2 + 16,2160 / 2 + 39,0);
			}
			{
				gMexMessageQueueOut.addMessage("keyDown","number",4.0);
			}
		}
		else if('5' == wParam || VK_NUMPAD5 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",5.0);
		}
		else if('6' == wParam || VK_NUMPAD6 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",6.0);
		}
		else if('7' == wParam || VK_NUMPAD7 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",7.0);
		}
		else if('8' == wParam || VK_NUMPAD8 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",8.0);
		}
		else if('9' == wParam || VK_NUMPAD9 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",9.0);
		}
		else if('0' == wParam || VK_NUMPAD0 == wParam)
		{
			gMexMessageQueueOut.addMessage("keyDown","number",0.0);
		}
		break;
	case WM_KEYUP:
		if(VK_SHIFT==wParam)
		{
			shiftDown = false;
			gMexMessageQueueOut.addMessage("keyUp","shift");
		}
		else if(VK_CONTROL==wParam)
		{
			ctrlDown = false;
			gMexMessageQueueOut.addMessage("keyUp","ctrl");
		}
		else if(VK_MENU)
		{
			altDown = false;
			gMexMessageQueueOut.addMessage("keyUp","alt");
		}
		break;
	}

	return DefWindowProc(hWnd,message,wParam,lParam);
}

HRESULT messageProcess(MSG& msg)
{
	HRESULT hr = S_OK;
	static clock_t lastTimeUpdate = clock();
	static clock_t lastRotateUpdate = clock();
	static int curAngle = 0;

	if(msg.message==WM_QUIT)
		return E_FAIL;

	int pleaseRender = 0;

	if(gRendererOn)
	{
		//gRenderer->renderAll();

		if(gPlay)
		{
			float timeFromLast = (float)(clock() - lastTimeUpdate) / CLOCKS_PER_SEC;
			if(timeFromLast > 1.0f/gFramesPerSec)
			{
				lastTimeUpdate = clock();
				gRenderer->incrementFrame();
				pleaseRender = 1;
				if(gCapture)
				{
					//gRenderer->renderAll();
					gRenderer->captureWindow(NULL);
				}
			}
		}
		if(gRotate)
		{
			float timeFromLast = (float)(clock() - lastRotateUpdate) / CLOCKS_PER_SEC;
			if(timeFromLast > 2.5f/numAngles)
			{
				lastRotateUpdate = clock();
				DirectX::XMMATRIX previousWorldRotation = gRenderer->getRootWorldRotation();
				gRenderer->setWorldRotation(previousWorldRotation*ROT_X);
				pleaseRender = 1;
				if(gCapture)
				{
					++curAngle;
					// This guy renders
					//gRenderer->renderAll();
					gRenderer->captureWindow(NULL);
					if(curAngle == numAngles)
					{
						gCapture = false;
						gRotate = false;
						curAngle = 0;
					}
				}
			}
		}
	}
	if(pleaseRender == 1)
	{
		gRenderer->renderAll();
	}

	return hr;
}

DWORD WINAPI messageLoop(LPVOID lpParam)
{
	std::string* rootDir = (std::string*)lpParam;
	HRESULT hr;
	try
	{
		hr = windowInit(gDllInstance,true,*rootDir);
	}
	catch(const std::exception& e)
	{
		sendErrMessage(e.what());
	}
	catch(const std::string& e)
	{
		sendErrMessage(e);
	}
	catch(...)
	{
		sendErrMessage("Caught an unknown error!");
	}

	delete rootDir;

	MSG msg ={0};

	if(hr==S_OK)
	{
		gRendererInit = true;

		DWORD termWait = WaitForSingleObject(gTermEvent,0);

		try
		{
			// DirectX thread
			while(termWait != WAIT_OBJECT_0 && hr==S_OK)
			{
				if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				// Inside here check for a message from queue
				// checkQueue();
				if(dataQueue->getNumMessages() > 0)
				{
					hr = checkMessage();
					if(hr==S_FALSE)
						break;
				}


				/**/
				hr = messageProcess(msg);

				termWait = WaitForSingleObject(gTermEvent,0);
			}
		}
		catch(const std::exception& e)
		{
			sendErrMessage(e.what());
		}
		catch(const std::string& e)
		{
			sendErrMessage(e);
		}
		catch(...)
		{
			sendErrMessage("Caught an unknown error!");
		}

		if(messageLoopHandle != NULL)
		{
			gRendererOn = false;

			Sleep(1000);

			//if (gRenderer != NULL)
			//gRenderer->getMutex();

			for(int i = 0; i < GraphicObjectTypes::VTend; ++i)
			{
				if(gRenderer != NULL)
				{
					std::map<int,GraphicObjectNode*>::iterator objectIter = gGraphicObjectNodes[i].begin();
					for(; objectIter != gGraphicObjectNodes[i].end(); ++objectIter)
					{
						GraphicObjectNode* node = objectIter->second;
						node->releaseRenderResources();
					}
				}

				gGraphicObjectNodes[i].clear();
			}

			gBorderObj = NULL;
			firstVolumeTextures.clear();
		}

		if(mexMessageMutex != NULL)
		{
			CloseHandle(mexMessageMutex);
			mexMessageMutex = NULL;
		}

	}
	else
	{
		sendHrErrMessage(hr);
	}

	try
	{
		SAFE_DELETE(gRenderer);
		SAFE_DELETE(gCameraDefaultMesh);
		SAFE_DELETE(gCameraWidget);

		DestroyWindow(gWindowHandle);
		UnregisterClass(szWndClassName,gDllInstance);

		gRendererInit = false;

		gMexMessageQueueOut.addMessage("close",1.0);
	}
	catch(const std::exception& e)
	{
		sendErrMessage(e.what());
	}
	catch(const std::string& e)
	{
		sendErrMessage(e);
	}
	catch(...)
	{
		sendErrMessage("Caught an unknown error!");
	}

	return ((int)msg.wParam);
}

HRESULT checkMessage()
{
	HRESULT hr = S_OK;
	Message m = dataQueue->getNextMessage();

	if(m.command == "loadTexture")
	{
		XloadTextureCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "close")
	{
		hr = S_FALSE;
	}
	else if(m.command == "viewTexture")
	{
		GraphicObjectTypes* typ = (GraphicObjectTypes*)m.data;
		
		setCurrentTexture(*typ);
		gRenderer->renderAll();
		delete typ;
	}
	else if(m.command == "peelUpdate")
	{
		XpeelUpdateCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "textureLightingUpdate")
	{
		XtextureLightingUpdateCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "textureAttenUpdate")
	{
		XtextureAttenUpdateCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "segmentationLighting")
	{
		XsegmentationLighting(m);
		gRenderer->renderAll();
	}
	else if(m.command == "play")
	{
		XplayCommand(m);
	}
	else if(m.command == "rotate")
	{
		XrotateCommand(m);
	}
	else if(m.command == "showLabels")
	{
		XshowLabelsCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "resetView")
	{
		XresetViewCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "captureSpinMovie")
	{
		XcaptureSpinMovieCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "transferUpdate")
	{
		XtransferUpdateCommand(m);
		gRenderer->renderAll();
	}
// 	else if(m.command == "viewTexture")
// 	{
// 		XviewTextureCommand(m);
// 		gRenderer->renderAll();
// 	}
	else if(m.command == "viewSegmentation")
	{
		XviewSegmentationCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "wireframeSegmentation")
	{
		XwireframeSegmentationCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "loadPolygon")
	{
		XaddPolygonCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "loadPolygons")
	{
		XaddPolygonsCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "removePolygon")
	{
		XremovePolygonCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "displayPolygons")
	{
		XdisplayPolygonsCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "setFrame")
	{
		XsetFrameCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "setViewOrigin")
	{
		XsetViewOriginCommand(m);
		gRenderer->renderAll();
	}
	else if(m.command == "updatePolygons")
	{

	}
	else if(m.command == "addPolygons")
	{

	}
	else if(m.command == "setCapturePath")
	{

	}
	else if(m.command == "captureWindow")
	{
		XcaptureWindow();
	}
	else if(m.command == "centerSelectedCell")
	{

	}
	else if(m.command == "deleteAllPolygons")
	{
		XdeleteAllPolygonsCommand(m);
		gRenderer->renderAll();
	}
	else if (m.command == "setRotation")
	{
		double* rotations = (double*)m.data;
		rotations[0] = rotations[0]/180.0 * DirectX::XM_PI;
		rotations[1] = rotations[1]/180.0 * DirectX::XM_PI;

		DirectX::XMMATRIX rotX,rotY;
		rotX = DirectX::XMMatrixRotationY(float(rotations[0]));
		rotY = DirectX::XMMatrixRotationX(float(rotations[1]));

		DirectX::XMMATRIX previousWorldRotation = gRenderer->getRootWorldRotation();
		gRenderer->setWorldRotation(previousWorldRotation*rotX*rotY);
		gRenderer->renderAll();

		delete[] rotations;
	}
	else if(m.command=="takeControl")
	{
		gRendererOn = false;
	}
	else if(m.command=="releaseControl")
	{
		gRendererOn = true;
	}
	else
	{
		// Print an error message
		/*char buff[255];
		sprintf_s(buff, "%s is not a valid command!\n", command);
		mexErrMsgTxt(buff);*/
	}
	return hr;
}