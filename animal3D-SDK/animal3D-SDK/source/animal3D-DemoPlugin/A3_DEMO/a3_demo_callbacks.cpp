/*
	Copyright 2011-2020 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein

	a3_demo_callbacks.c/.cpp
	Main implementation of window callback hooks.

	********************************************
	*** THIS IS THE LIBRARY'S CALLBACKS FILE ***
	*** App hooks your demo via this file.   ***
	********************************************
*/


#include "a3_dylib_config_export.h"
#include "a3_DemoState.h"
#include "GameObject.h"
#include "Event.h"
#include "GameObject.h"
#include "EventManager.h"

#include "a3_NetworkingManager.h"
#include "a3_ChatManager.h"
#include "BoidManager.h"
#include "Vector2.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"
#include <time.h>
#include <stdlib.h>




struct Game {
	EventManager eventManager[1];
	GameObject gameObject[1];

	// networking
	a3_NetworkingManager net[1];

	// chat
	a3_ChatManager chat[1];

	BoidManager boidManager[1];
};

Game myGame;


//-----------------------------------------------------------------------------
// networking stuff

void a3demo_startNetworking(a3_DemoState* demoState, a3boolean const isServer)
{
	a3ui16 const port_server = 60006;
	a3ui16 const port_client = 60005;
	a3ui16 const maxConnections_server = 16;
	a3ui16 const maxConnections_client = 1;

	if (isServer && !myGame.net->connected)
	{
		myGame.net->isServer = 1;
		myGame.chat->states = a3_ChatManager::ChatStates(3);
	}
	else
	{
		if (a3netStartup(myGame.net, 0, port_server, 0, maxConnections_client) > 0)
		{
			if (a3netConnect(myGame.net, myGame.net->ip) > 0)
			{
				myGame.net->isServer = 0;
				printf("\n STARTED NETWORKING AS CLIENT \n");
				myGame.net->connected = 1;
			}

		}
	}
}

void SetUpServer()
{
	a3ui16 const port_server = 60006;
	a3ui16 const port_client = 60005;
	a3ui16 const maxConnections_server = 16;
	a3ui16 const maxConnections_client = 1;
	if (a3netStartup(myGame.net, port_server, 0, maxConnections_server, 0) > 0)
	{
		printf("\nSTARTED NETWORKING AS SERVER \n");
		myGame.net->connected = 1;
		myGame.net->isServer = 1;
		myGame.chat->states = myGame.chat->IN_CHAT;
	}
}


void a3demo_stopNetworking(a3_DemoState* demoState)
{
	if (a3netDisconnect(myGame.net) > 0)
		if (a3netShutdown(myGame.net) > 0)
		{
			myGame.net->connected = false;
			myGame.net->isServer = false;
			printf("\n SHUT DOWN NETWORKING \n");
		}
}

void UpdateLoop(a3_DemoState* demoState)
{
	if (!myGame.net->connected)
		return;
	myGame.boidManager->UpdateBoids(myGame.net, demoState);


	switch (myGame.net->dataPackageType)
	{
	case 1: //SERVER ONLY
	{
		if (myGame.net->isServer)
		{
			//UPDATE BOIDS 



		}

	}
	break;
	case 2: //SERVER SENDS MESSAGES, CLIENT UPDATES
	{
		if (!myGame.net->isServer)
		{
			//Update your(the client) boids only
		}
	}
	break;
	case 3: //EVERYONE UPDATES THIER OWN, SERVER INCLUDED
	{
		//update your boids with everyone elses boids in consideration
		//dependet on other states
	}
	break;
	default:
		break;
	}
}


//-----------------------------------------------------------------------------
// miscellaneous functions

// get the size of the persistent state to allocate
//	(good idea to keep it relatively constant, so that addresses don't change 
//	when the library is reloaded... that would mess everything up!)
inline a3ui32 a3demo_getPersistentStateSize()
{
	const a3ui32 minimum = sizeof(a3_DemoState);
	a3ui32 size = 1;
	while (size < minimum)
		size += size;
	return size;
}


// consistent text initialization
inline void a3demo_initializeText(a3_TextRenderer* text)
{
	a3textInitialize(text, 18, 1, 0, 0, 0);
}


//-----------------------------------------------------------------------------
// callback sub-routines

inline void a3demoCB_keyCharPress_main(a3_DemoState* demoState, a3i32 asciiKey,
	const a3ui32 demoSubMode, const a3ui32 demoOutput,
	const a3ui32 demoSubModeCount, const a3ui32 demoOutputCount)
{
	switch (asciiKey)
	{
		// sub-modes
	case '>':
		break;
	case '<':
		break;

		// toggle active camera
//	case 'v':
//		demoState->activeCamera = (demoState->activeCamera + 1) % demoStateMaxCount_cameraObject;
//		break;
//	case 'c':
//		demoState->activeCamera = (demoState->activeCamera - 1 + demoStateMaxCount_cameraObject) % demoStateMaxCount_cameraObject;
//		break;

		// toggle skybox
	case 'b':
		demoState->displaySkybox = 1 - demoState->displaySkybox;
		break;

		// toggle hidden volumes
	case 'h':
		demoState->displayHiddenVolumes = 1 - demoState->displayHiddenVolumes;
		break;

		// toggle pipeline overlay
	case 'o':
		demoState->displayPipeline = 1 - demoState->displayPipeline;
		break;
	}
}

inline void a3demoCB_keyCharHold_main(a3_DemoState* demoState, a3i32 asciiKey)
{
	// handle special cases immediately
	switch (asciiKey)
	{
	case 'l':
		break;
	case 'L':
		break;
	}
}


//-----------------------------------------------------------------------------
// callback prototypes
// NOTE: do not move to header; they should be private to this file
// NOTE: you may name these functions whatever you like, just be sure to 
//	update your debug config file: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo-debug.txt"
// copy this config line and the DLL to your main config with a new name when 
//	you're happy with it: 
//	"<root>/resource/animal3D-data/animal3D-demoinfo.txt"


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus

	A3DYLIBSYMBOL a3_DemoState* a3demoCB_load(a3_DemoState* demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3_DemoState* a3demoCB_unload(a3_DemoState* demoState, a3boolean hotbuild);
	A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_DemoState* demoState);
	A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_DemoState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_DemoState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_DemoState* demoState);
	A3DYLIBSYMBOL void a3demoCB_windowMove(a3_DemoState* demoState, a3i32 newWindowPosX, a3i32 newWindowPosY);
	A3DYLIBSYMBOL void a3demoCB_windowResize(a3_DemoState* demoState, a3i32 newWindowWidth, a3i32 newWindowHeight);
	A3DYLIBSYMBOL void a3demoCB_keyPress(a3_DemoState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyHold(a3_DemoState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_DemoState* demoState, a3i32 virtualKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_DemoState* demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_DemoState* demoState, a3i32 asciiKey);
	A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_DemoState* demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_DemoState* demoState, a3i32 cursorX, a3i32 cursorY);
	A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_DemoState* demoState);
	A3DYLIBSYMBOL void RenderAllApplications(a3_DemoState* demoState);
	A3DYLIBSYMBOL void UpdateInput(a3_DemoState* demoState);
	A3DYLIBSYMBOL void HandleOutput(a3_DemoState* demoState);

#ifdef __cplusplus
}
#endif	// __cplusplus


//-----------------------------------------------------------------------------
// callback implementations

// demo is loaded
A3DYLIBSYMBOL a3_DemoState* a3demoCB_load(a3_DemoState* demoState, a3boolean hotbuild)
{
	srand((unsigned int)time(NULL));   // Initialization, should only be called once.
	
	const a3ui32 stateSize = a3demo_getPersistentStateSize();
	const a3ui32 trigSamplesPerDegree = 4;



	// do any re-allocation tasks
	if (demoState && hotbuild)
	{
		//	const a3ui32 stateSize = a3demo_getPersistentStateSize();
		//	a3_DemoState copy = *demoState;

			// example 1: copy memory directly
		//	free(demoState);
		//	demoState = (a3_DemoState *)malloc(stateSize);
		//	memset(demoState, 0, stateSize);
		//	*demoState = copy;

			// call refresh to re-link pointers in case demo state address changed
		a3demo_refresh(demoState);
		a3demo_initSceneRefresh(demoState);
		a3trigInitSetTables(trigSamplesPerDegree, demoState->trigTable);
	}

	// do any initial allocation tasks
	else if (demoState = (a3_DemoState*)malloc(stateSize))
	{
		// HEAP allocate persistent state
		// stack object will be deleted at the end of the function
		// good idea to set the whole block of memory to zero
		memset(demoState, 0, stateSize);

		// set up trig table (A3DM)
		a3trigInit(trigSamplesPerDegree, demoState->trigTable);

		// initialize state variables
		// e.g. timer, thread, etc.
		a3timerSet(demoState->renderTimer, 30.0);
		a3timerStart(demoState->renderTimer);

		// text
		a3demo_initializeText(demoState->text);
		demoState->textInit = 1;
		demoState->textMode = 1;
		demoState->textModeCount = 3;	// 0=off, 1=controls, 2=data


		// enable asset streaming between loads
	//	demoState->streaming = 1;


		// create directory for data
		a3fileStreamMakeDirectory("./data");


		// set default GL state
		a3demo_setDefaultGraphicsState();

		// geometry
		a3demo_loadGeometry(demoState);

		// shaders
		a3demo_loadShaders(demoState);

		// scene objects
		a3demo_initScene(demoState);

		//Chat manager
		InitChatManager(myGame.chat);



		*myGame.gameObject = GameObject();
		*myGame.eventManager = EventManager();
		myGame.net->connected = 0;
	}

	// return persistent state pointer
	return demoState;
}

// demo is unloaded; option to unload to prep for hotbuild
A3DYLIBSYMBOL a3_DemoState* a3demoCB_unload(a3_DemoState* demoState, a3boolean hotbuild)
{
	// release things that need releasing always, whether hotbuilding or not
	// e.g. kill thread
	// nothing in this example, but then...

	// release persistent state if not hotbuilding
	// good idea to release in reverse order that things were loaded...
	//	...normally; however, in this case there are reference counters 
	//	to make sure things get deleted when they need to, so there is 
	//	no need to reverse!
	if (!hotbuild)
	{
		// free fixed objects
		a3textRelease(demoState->text);

		// free graphics objects
		a3demo_unloadGeometry(demoState);
		a3demo_unloadShaders(demoState);

		// validate unload
		a3demo_validateUnload(demoState);

		// erase other stuff
		a3trigFree();

		// networking
		a3demo_stopNetworking(demoState);

		// erase persistent state
		free(demoState);
		demoState = 0;
	}

	// return state pointer
	return demoState;
}

// window updates display
// **NOTE: DO NOT USE FOR RENDERING**
A3DYLIBSYMBOL a3i32 a3demoCB_display(a3_DemoState* demoState)
{
	// do nothing, should just return 1 to indicate that the 
	//	window's display area is controlled by the demo
	return 1;
}

// window idles
A3DYLIBSYMBOL a3i32 a3demoCB_idle(a3_DemoState* demoState)
{
	// perform any idle tasks, such as rendering
	if (!demoState->exitFlag)
	{
		if (a3timerUpdate(demoState->renderTimer) > 0)
		{
			// render timer ticked, update demo state and draw
			//a3demo_input(demoState, demoState->renderTimer->secondsPerTick);
			UpdateInput(demoState);
			InputChatManager(myGame.chat, demoState);
			a3netProcessInbound(myGame.net, myGame.eventManager, myGame.boidManager, myGame.chat);
			UpdateChatManager(myGame.chat, demoState, myGame.net);
			UpdateLoop(demoState);
			//a3demo_update(demoState, demoState->renderTimer->secondsPerTick);
			//a3netProcessOutbound(myGame.net);
			HandleOutput(demoState);
			RenderAllApplications(demoState);

			// update input
			a3mouseUpdate(demoState->mouse);
			a3keyboardUpdate(demoState->keyboard);
			a3XboxControlUpdate(demoState->xcontrol);

			// render occurred this idle: return +1
			return +1;
		}

		// nothing happened this idle: return 0
		return 0;
	}

	// demo should exit now: return -1
	return -1;
}

// window gains focus
A3DYLIBSYMBOL void a3demoCB_windowActivate(a3_DemoState* demoState)
{
	// nothing really needs to be done here...
	//	but it's here just in case
}

// window loses focus
A3DYLIBSYMBOL void a3demoCB_windowDeactivate(a3_DemoState* demoState)
{
	// reset input; it won't track events if the window is inactive, 
	//	active controls will freeze and you'll get strange behaviors
	a3keyboardReset(demoState->keyboard);
	a3mouseReset(demoState->mouse);
	a3XboxControlReset(demoState->xcontrol);
	a3XboxControlSetRumble(demoState->xcontrol, 0, 0);
}

// window moves
A3DYLIBSYMBOL void a3demoCB_windowMove(a3_DemoState* demoState, a3i32 newWindowPosX, a3i32 newWindowPosY)
{
	// nothing needed here
}

// window resizes
A3DYLIBSYMBOL void a3demoCB_windowResize(a3_DemoState* demoState, a3i32 newWindowWidth, a3i32 newWindowHeight)
{
	a3ui32 i;
	a3_DemoCamera* camera;

	// account for borders here
	const a3i32 frameBorder = 0;
	const a3ui32 frameWidth = newWindowWidth + frameBorder + frameBorder;
	const a3ui32 frameHeight = newWindowHeight + frameBorder + frameBorder;
	const a3real windowAspect = (a3real)newWindowWidth / (a3real)newWindowHeight;
	const a3real frameAspect = (a3real)frameWidth / (a3real)frameHeight;

	// copy new values to demo state
	demoState->windowWidth = newWindowWidth;
	demoState->windowHeight = newWindowHeight;
	demoState->windowWidthInv = a3recip((a3real)newWindowWidth);
	demoState->windowHeightInv = a3recip((a3real)newWindowHeight);
	demoState->windowAspect = windowAspect;
	demoState->frameWidth = frameWidth;
	demoState->frameHeight = frameHeight;
	demoState->frameWidthInv = a3recip((a3real)frameWidth);
	demoState->frameHeightInv = a3recip((a3real)frameHeight);
	demoState->frameAspect = frameAspect;
	demoState->frameBorder = frameBorder;

	// framebuffers should be initialized or re-initialized here 
	//	since they are likely dependent on the window size


	// use framebuffer deactivate utility to set viewport
	a3framebufferDeactivateSetViewport(a3fbo_depthDisable, -frameBorder, -frameBorder, demoState->frameWidth, demoState->frameHeight);

	// viewing info for projection matrix
	// initialize cameras dependent on viewport
	for (i = 0, camera = demoState->camera + i; i < demoStateMaxCount_cameraObject; ++i, ++camera)
	{
		camera->aspect = frameAspect;
		a3demo_updateCameraProjection(camera);
	}
}

// any key is pressed
A3DYLIBSYMBOL void a3demoCB_keyPress(a3_DemoState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is held
A3DYLIBSYMBOL void a3demoCB_keyHold(a3_DemoState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_down);
}

// any key is released
A3DYLIBSYMBOL void a3demoCB_keyRelease(a3_DemoState* demoState, a3i32 virtualKey)
{
	// persistent state update
	a3keyboardSetState(demoState->keyboard, (a3_KeyboardKey)virtualKey, a3input_up);
}

// ASCII key is pressed (immediately preceded by "any key" pressed call above)
// NOTE: there is no release counterpart
A3DYLIBSYMBOL void a3demoCB_keyCharPress(a3_DemoState* demoState, a3i32 asciiKey)
{
	a3ui32 demoSubMode = demoState->demoSubMode[demoState->demoMode];
	const a3ui32 demoSubModeCount = demoState->demoSubModeCount[demoState->demoMode];
	const a3ui32 demoOutput = demoState->demoOutputMode[demoState->demoMode][demoSubMode];
	const a3ui32 demoOutputCount = demoState->demoOutputCount[demoState->demoMode][demoSubMode];

	// persistent state update
	a3keyboardSetStateASCII(demoState->keyboard, (a3byte)asciiKey);


	if (myGame.chat->states == 1 || myGame.chat->states == 0)
		return;
	else if (myGame.chat->states == 3)
	{
		switch (asciiKey)
		{
			// DATa PUSH
		case '1':
		{

			myGame.net->dataPackageType = (a3_NetworkingManager::DataPackagingType)1;
			printf("\nData PUSH \n");
			SetUpServer();
			for (int i = 0; i < BoidManager::BOIDS_PER_USER; i++)
			{
				float x = (float)(rand() % 100) + -50;
				float y = (float)(rand() % 100) + -50;

				myGame.boidManager->SpawnNewBoid(Vector2(x*0,y * 0), Vector2(x, y), true, 20);
			}
		}
		break;

		// DATA SHARE
		case '2':
		{

			myGame.net->dataPackageType = (a3_NetworkingManager::DataPackagingType)2;
			printf("\nData SHARING \n");
			SetUpServer();
		}
		break;

		// DATA COUPLED
		case '3':
		{

			myGame.net->dataPackageType = (a3_NetworkingManager::DataPackagingType)3;
			printf("\nData COUPLED \n");
			SetUpServer();
		}
		break;
		default:
			break;
		}

		

		return;
	}

	// handle special cases immediately
	switch (asciiKey)
	{


		// stop networking
	case '0':
		a3demo_stopNetworking(demoState);
		break;

		// start networking as server
	case '1':
		a3demo_startNetworking(demoState, 1);
		break;

		// start networking as client
	case '2':
		a3demo_startNetworking(demoState, 0);
		break;
	}

}

// ASCII key is held
A3DYLIBSYMBOL void a3demoCB_keyCharHold(a3_DemoState* demoState, a3i32 asciiKey)
{
	// persistent state update
	a3keyboardSetStateASCII(demoState->keyboard, (a3byte)asciiKey);


	// callback for current mode
	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		a3demoCB_keyCharHold_main(demoState, asciiKey);
		break;
	}
}

// mouse button is clicked
A3DYLIBSYMBOL void a3demoCB_mouseClick(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse button is double-clicked
A3DYLIBSYMBOL void a3demoCB_mouseDoubleClick(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_down);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse button is released
A3DYLIBSYMBOL void a3demoCB_mouseRelease(a3_DemoState* demoState, a3i32 button, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetState(demoState->mouse, (a3_MouseButton)button, a3input_up);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse wheel is turned
A3DYLIBSYMBOL void a3demoCB_mouseWheel(a3_DemoState* demoState, a3i32 delta, a3i32 cursorX, a3i32 cursorY)
{
	// controlled camera when zooming
	a3_DemoCamera* camera;

	// persistent state update
	a3mouseSetStateWheel(demoState->mouse, (a3_MouseWheelState)delta);
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);

	switch (demoState->demoMode)
	{
		// main render pipeline
	case demoStateMode_main:
		// can use this to change zoom
		// zoom should be faster farther away
		camera = demoState->camera + demoState->activeCamera;
		camera->fovy -= camera->ctrlZoomSpeed * (camera->fovy / a3real_oneeighty) * (a3f32)delta;
		camera->fovy = a3clamp(camera->ctrlZoomSpeed, a3real_oneeighty - camera->ctrlZoomSpeed, camera->fovy);
		a3demo_updateCameraProjection(camera);
		break;
	}
}

// mouse moves
A3DYLIBSYMBOL void a3demoCB_mouseMove(a3_DemoState* demoState, a3i32 cursorX, a3i32 cursorY)
{
	// persistent state update
	a3mouseSetPosition(demoState->mouse, cursorX, cursorY);
}

// mouse leaves window
A3DYLIBSYMBOL void a3demoCB_mouseLeave(a3_DemoState* demoState)
{
	// reset mouse state or any buttons pressed will freeze
	a3mouseReset(demoState->mouse);
}


//-----------------------------------------------------------------------------

A3DYLIBSYMBOL void RenderAllApplications(a3_DemoState* demoState)
{
	glClear(GL_COLOR_BUFFER_BIT);
	RenderChatManager(myGame.chat, demoState, myGame.net);

	if (myGame.chat->states == 2 && myGame.net->connected == 0)
	{
		a3textDraw(demoState->text, -1, 0.9f, -1, 1, 1, 1, 1, "AS SOON AS IP IS ENTERED PRESS 1 TO ENTER AS SERVER AND 2 TO ENTER AS CLIENT");
	}
	else if (myGame.chat->states == 3)
	{
		a3textDraw(demoState->text, -1, 0.9f, -1, 1, 1, 1, 1, "ENTER 1 FOR DATA PUSH, ENTER 2 FOR DATA SHARING, ENTER 3 FOR DATA COUPLED");
	}
	else
	{
		if (myGame.net->isServer == 1)
		{

			a3textDraw(demoState->text, 0, 0.9f, -1, 1, 1, 1, 1, "THIS IS THE SERVER");
		}
		myGame.boidManager->RenderBoids(myGame.net, demoState);
	}

	myGame.gameObject->Render(demoState);


}




A3DYLIBSYMBOL void UpdateInput(a3_DemoState* demoState)
{
	   	 
	if (myGame.net->isServer)
		return;
	if (myGame.net->connected == 0)
		return;

	/*if (demoState->keyboard->key.key[a3key_downArrow] && !demoState->keyboard->key0.key[a3key_downArrow])
	{
		MoveEvent* newEvent = new MoveEvent(myGame.gameObject->getX(), myGame.gameObject->getY() - 10, myGame.gameObject, true);
		myGame.eventManager->AddEvent(newEvent);


	}*/
}



A3DYLIBSYMBOL void HandleOutput(a3_DemoState* demoState)
{
	if (!myGame.net->connected)
		return;
	switch (myGame.net->dataPackageType)
	{
	case 1: //SERVER ONLY
	{
		if (myGame.net->isServer)
		{
			// send to clients
		}
	}
	break;
	case 2: //SERVER SENDS MESSAGES, CLIENT UPDATES
	{
		if (!myGame.net->isServer)
		{
			//Send to server
		}
	}
	break;
	case 3: //EVERYONE UPDATES THIER OWN, SERVER INCLUDED
	{
		if (!myGame.net->isServer)
		{
			//Send to server
		}
		else
		{
			//send to each client
		}
	}
	break;
	default:
		break;
	}
	myGame.eventManager->HandleEvents(myGame.net);
}




