#include "STexture.h"

STexture::STexture()
	:UVIndex(0)
{

}

STexture::~STexture()
{

}

STexture2D::STexture2D()
{
	TextureType = ETextureType::ETT_2D;
}
