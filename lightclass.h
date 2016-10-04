#pragma once

//simple class to maintain direction and color of lights

////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_



//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
using namespace DirectX;
//////////////////////////////////////////////////////////////////////////
// Class name: LightClass
//////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass& other);
	~LightClass();

	void SetDiffuseColor(float red, float green, float blue, float alpha);
	void SetDirection(float x, float y, float z);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;


};

#endif