#include "A3_DEMO/a3_ChatManager.h"
#include "a3_DemoState.h"
#include <stdio.h>
#include <string.h>
#include "a3_NetworkingManager.h"



a3i32 InitChatManager(a3_ChatManager* chatManager)
{
	chatManager->inputIndex = 0;
	for (a3_Message message : chatManager->messageList)
	{
		message.render = 0; //Should not render
		message.messageLength = 1; //1 frame length???
		
	}
	chatManager->states = chatManager->STARTUP;
	return 0;
}


a3i32 InputChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState)
{
	for (int i = 32; i < 127; i++)
	{
		if (demoState->keyboard->keyASCII.key[i] && !demoState->keyboard->keyASCII0.key[i])
		{
			if (chatManager->inputIndex <= TEXT_ARRAY_SIZE)
			{

				chatManager->textInput[chatManager->inputIndex] = i;
				chatManager->inputIndex++;
			}
		}
	}

	if (demoState->keyboard->keyASCII.key[8] && !demoState->keyboard->keyASCII0.key[8])
	{
		if (chatManager->inputIndex > 0)
		{
			chatManager->textInput[chatManager->inputIndex] = 0;
			chatManager->inputIndex--;
		}
		else if (chatManager->inputIndex == 0)
		{
			chatManager->textInput[chatManager->inputIndex] = 0;
		}
	}
	return 0;
}


a3i32 UpdateChatManager(a3_ChatManager* chatManager, a3_DemoState* demoState, a3_NetworkingManager* net)
{
	if ((demoState->keyboard->key.key[10] && !demoState->keyboard->key0.key[10]) ||
		(demoState->keyboard->key.key[13] && !demoState->keyboard->key0.key[13]))
	{

		switch (chatManager->states)
		{
		case 0:
			if (chatManager->textInput[0] == 0)
			{
				strcpy(net->ip, "127.0.0.1");
			}
			else
			{
				strcpy(net->ip, chatManager->textInput);
			}
			chatManager->states = chatManager->USERNAME_INPUT;
			break;

		case 1:
			strcpy(chatManager->user, chatManager->textInput);
			chatManager->states = chatManager->IN_CHAT;
			break;
		case 2:
			//SEND MESSAGE TO SERVER

			break;
		default:
			break;
		}


		memset(chatManager->textInput, 0, TEXT_ARRAY_SIZE);
		chatManager->inputIndex = 0;
	}

	for( int i =0;
		i < MAX_MESSAGES_RECEIVED;
		i++)
	{
		chatManager->messageList[i].messageLength -= demoState->renderTimer->secondsPerTick;
		if (chatManager->messageList[i].messageLength <= 0.0f)
		{
			chatManager->messageList[i].render = 0;
			continue;
		}
	}
	return 0;
}

a3i32 RenderChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState, a3_NetworkingManager* net)
{
	switch (chatManager->states)
	{
	case 0:
		a3textDraw(demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter IP : %s", chatManager->textInput);
		break;
	case 1:
		a3textDraw(demoState->text, -1, 0, -1, 1, 1, 1, 1, "Enter USERNAME: %s", chatManager->textInput);
		break;
	case 2:

		if (net->isServer == 0)
		{

		a3textDraw(demoState->text, -1, -0.9f, -1, 1, 1, 1, 1, "%s : %s", chatManager->user, chatManager->textInput);

		a3f32 startPos = -0.8f;
		for (a3_Message message : chatManager->messageList)
		{
			if (message.render == 1)
			{
				a3textDraw(demoState->text, startPos, 0, -1, 1, 1, 1, 1, "%s : %s", message.user, message.message);
				startPos += 0.05f;
			}
		}

		}
		break;
	}
	

	
	return 0;
}
