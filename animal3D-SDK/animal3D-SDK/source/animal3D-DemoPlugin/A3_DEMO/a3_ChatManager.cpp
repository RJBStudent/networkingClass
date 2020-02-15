#include "A3_DEMO/a3_ChatManager.h"
#include "a3_DemoState.h"
/*
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

	for (std::vector<a3_Message>::iterator iter = chatManager->MessageList.begin();
		iter != chatManager->MessageList.end();
		++iter)
	{
		iter->messageLength -= demoState->renderTimer->secondsPerTick;
		if (iter->messageLength <= 0.0f)
		{
			chatManager->MessageList.erase(iter);
			continue;
		}
	}
	return 0;
}

a3i32 RenderChatManager(a3_ChatManager* chatManager, a3_DemoState* const demoState)
{

	//a3textDraw(demoState->text, -1, 0, -1, 1, 1, 1, 1, "%s : %s",  , chatManager->textInput);

	a3f32 startPos = -0.8f;
	for (a3_Message message : chatManager->MessageList)
	{
		a3textDraw(demoState->text, startPos, 0, -1, 1, 1, 1, 1, "%s : %s", message.user, message.message);
		startPos += 0.05f;
	}
	return 0;
}
*/