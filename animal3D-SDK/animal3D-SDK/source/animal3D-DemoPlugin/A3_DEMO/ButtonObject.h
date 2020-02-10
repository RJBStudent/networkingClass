#pragma once
#include "animal3D-A3DG/animal3D-A3DG.h"
#include "animal3D-A3DM/animal3D-A3DM.h"
#include "A3_DEMO/_utilities/a3_DemoShaderProgram.h"
#include "A3_DEMO/a3_DemoState.h"



class ButtonObject
{
public:
	ButtonObject();
	~ButtonObject();

	void Init(const a3_Texture* texture, a3real xPos, a3real yPos, a3real width, a3real height, a3real sheetXPos = 0, a3real sheetYPos = 0);

	void Render(const a3_DemoState* demoState, const a3_DemoStateShaderProgram* currentDemoProgram, a3mat4 projMat) const;

	bool ButtonClickCheck(a3i32 mouseX, a3i32 mouseY) const;

	void SetTexture(a3_Texture* texture);

private:	   
	const a3_Texture *buttonTexture;
	a3vec4 spritePos;
	
	a3mat4 spriteMat;
	a3mat4 atlasMat;
	a3real spriteWidth;
	a3real spriteHeight;

};

ButtonObject::ButtonObject()
{
}

ButtonObject::~ButtonObject()
{
}

void ButtonObject::SetTexture(a3_Texture* texture)
{
	buttonTexture = texture;
}

void ButtonObject::Init(const a3_Texture* texture, a3real xPos, a3real yPos, a3real width, a3real height, a3real sheetXPos , a3real sheetYPos )
{
	buttonTexture = texture;
	spritePos.x = xPos;
	spritePos.y = yPos;
	spritePos.z = 0;
	spritePos.w = 1;
	spriteWidth = width;
	spriteHeight = height;
	a3real4x4SetNonUnif(spriteMat.m, width, height, 1);

	spriteMat.v3 = spritePos;

}


bool ButtonObject::ButtonClickCheck(a3i32 mouseX, a3i32 mouseY) const
{
	//center point is where it is drawing
	if ((mouseX > spritePos.x - (spriteWidth) && mouseX < spritePos.x + (spriteWidth)) && 
		(mouseY > spritePos.y - (spriteHeight) && mouseY < spritePos.y + (spriteHeight)))
	{
		return true;
	}
	return false;
}

/// <summary>
/// Render button
/// </summary>
void ButtonObject::Render(const a3_DemoState* demoState, const a3_DemoStateShaderProgram* currentDemoProgram, a3mat4 projMat) const
{
	a3mat4 modelViewMat;
	a3real4x4Product(modelViewMat.m, projMat.m, spriteMat.m);
	a3shaderUniformSendFloatMat(a3unif_mat4, 0, currentDemoProgram->uMVP, 1, modelViewMat.mm);
	a3textureActivate(buttonTexture, a3tex_unit00);
	a3vertexDrawableActivateAndRender(demoState->draw_unitquad);

}