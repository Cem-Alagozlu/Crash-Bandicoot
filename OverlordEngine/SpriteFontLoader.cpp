#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	std::string idBytes{};
	for (size_t i = 0; i < 3; i++)
	{
		idBytes += pBinReader->Read<char>();
	}
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	if (idBytes != "BMF")
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	int version = static_cast<int>(pBinReader->Read<char>());
	if (version < 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	int8_t blockId = pBinReader->Read<int8_t>();
	int32_t blockSize = pBinReader->Read<int32_t>();

	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	int16_t fontSize = pBinReader->Read<int16_t>();
	pSpriteFont->m_FontSize = fontSize;

	//Move the binreader to the start of the FontName[BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//12 -> because thats the position of font name (included the fontsize)
	pBinReader->MoveBufferPosition(12);

	//Retrieve the FontName [SpriteFont::m_FontName]
	std::wstring name{};
	name = pBinReader->ReadNullString();
	pSpriteFont->m_FontName = name;

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<int8_t>();
	blockSize = pBinReader->Read<int32_t>();

	//move 4 pos, scale w & scale h
	pBinReader->MoveBufferPosition(4);

	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	int16_t textureWidth = pBinReader->Read<int16_t>();
	int16_t textureHeight = pBinReader->Read<int16_t>();
	pSpriteFont->m_TextureWidth = textureWidth;
	pSpriteFont->m_TextureHeight = textureHeight;

	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
	int16_t pageCount = pBinReader->Read<int16_t>();
	
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
	}
	//**********
	// BLOCK 2 *
	//**********
	//move 5 pos to the next block
	pBinReader->MoveBufferPosition(5);

	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<int8_t>();
	blockSize = pBinReader->Read<int32_t>();

	//Retrieve the PageName (store Local)
	std::wstring pageName{};
	pageName = pBinReader->ReadNullString();

	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	if (pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}

	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	
	pageName = assetFile.substr(0, assetFile.rfind('/') + 1) + pageName;
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(pageName);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<int8_t>();
	blockSize = pBinReader->Read<int32_t>();

	//Retrieve Character Count (see documentation)
	int characterCount = blockSize / 20;

	//Retrieve Every Character, For every Character:
	for (int i = 0; i < characterCount; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t charId = static_cast<wchar_t>(pBinReader->Read<int32_t>());

		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!SpriteFont::IsCharValid(charId))
		{
			Logger::LogWarning(L"Character not valid");
			continue;
		}

		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric& fontMetric = pSpriteFont->GetMetric(charId);

		//> Set IsValid to true [FontMetric::IsValid]
		fontMetric.IsValid = true;

		//> Set Character (CharacterId) [FontMetric::Character]
		fontMetric.Character = charId;

		//> Retrieve Xposition (store Local)
		int16_t charPosX = pBinReader->Read<int16_t>();
	
		//> Retrieve Yposition (store Local)
		int16_t charPosY = pBinReader->Read<int16_t>();

		//> Retrieve & Set Width [FontMetric::Width]
		fontMetric.Width = pBinReader->Read<int16_t>();

		//> Retrieve & Set Height [FontMetric::Height]
		fontMetric.Height = pBinReader->Read<int16_t>();

		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		fontMetric.OffsetX = pBinReader->Read<int16_t>();

		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		fontMetric.OffsetY = pBinReader->Read<int16_t>();

		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		fontMetric.AdvanceX = pBinReader->Read<int16_t>();

		//> Retrieve & Set Page [FontMetric::Page]
		fontMetric.Page = pBinReader->Read<int8_t>();

		//> Retrieve Channel (BITFIELD!!!) 
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		int8_t channel = pBinReader->Read<int8_t>();
		//if ((channel & 0b00000001) == 1)
		//{
		//	fontMetric.Channel = 2;
		//}
		//else if ((channel & 0b00000010) == 2)
		//{
		//	fontMetric.Channel = 1;
		//}
		//else if ((channel & 0b00000100) == 4 )
		//{
		//	fontMetric.Channel = 0;
		//}
		if (channel == 1) fontMetric.Channel = 2;
		else if (channel == 2) fontMetric.Channel = 1;
		else if (channel == 4) fontMetric.Channel = 0;

		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		fontMetric.TexCoord = DirectX::XMFLOAT2(charPosX / float(textureWidth), charPosY / float(textureHeight));
	}
	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
