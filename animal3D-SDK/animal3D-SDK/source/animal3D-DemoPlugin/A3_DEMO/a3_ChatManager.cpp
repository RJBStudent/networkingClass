#include "A3_DEMO/a3_ChatManager.h"

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

a3i32 UpdateChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState)
{
	if ((demoState->keyboard->key.key[10] && !demoState->keyboard->key0.key[10]) ||
		(demoState->keyboard->key.key[13] && !demoState->keyboard->key0.key[13]))
	{

	}
	return 0;
}

a3i32 RenderChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState)
{
	return 0;
}