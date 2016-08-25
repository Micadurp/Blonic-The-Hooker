#include "fontclass.h"

FontClass::FontClass()
{
	m_Font = 0;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* _device, char* _fontFilename, WCHAR* _textureFilename)
{
	bool result;


	// Load in the text file containing the font data.
	result = LoadFontData(_fontFilename);
	if (!result)
	{
		return false;
	}

	// Load the texture that has the font characters on it.
	result = LoadTexture(_device, _textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void FontClass::Shutdown()
{
	// Release the font texture.
	ReleaseTexture();

	// Release the font data.
	ReleaseFontData();

	return;
}

bool FontClass::LoadFontData(char* _filename)
{
	ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	m_Font = new FontType[95];

	if (!m_Font)
	{
		return false;
	}
		
	// Read in the font size and spacing between chars.
	fin.open(_filename);
	if (fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for (i = 0; i<95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	// Release the font data array.
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = 0;
	}

	return;
}

void FontClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

bool FontClass::LoadTexture(ID3D11Device* _device, WCHAR* _texName)
{
	HRESULT result = CreateDDSTextureFromFile(_device, _texName, NULL, &m_texture);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_texture;
}

void FontClass::BuildVertexArray(void* _vertices, const char* _sentence, float _drawX, float _drawY)
{
	VertexType* vertexPtr;
	int numLetters, index;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)_vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(_sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for (int i = 0; i<numLetters; i++)
	{
		int letter = ((int)_sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
		{
			_drawX = _drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = { _drawX, _drawY, 0.0f, 1.0f };									// Top left.
			vertexPtr[index].texture = { m_Font[letter].left, 0.0f };
			index++;

			vertexPtr[index].position = { (_drawX + m_Font[letter].size), (_drawY - 16), 0.0f, 1.0f };	// Bottom right.
			vertexPtr[index].texture = { m_Font[letter].right, 1.0f };
			index++;

			vertexPtr[index].position = { _drawX, (_drawY - 16), 0.0f, 1.0f };							// Bottom left.
			vertexPtr[index].texture = { m_Font[letter].left, 1.0f };
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = { _drawX, _drawY, 0.0f, 1.0f };									// Top left.
			vertexPtr[index].texture = { m_Font[letter].left, 0.0f };
			index++;

			vertexPtr[index].position = { _drawX + m_Font[letter].size, _drawY, 0.0f, 1.0f };				// Top right.
			vertexPtr[index].texture = { m_Font[letter].right, 0.0f };
			index++;

			vertexPtr[index].position = { (_drawX + m_Font[letter].size), (_drawY - 16), 0.0f, 1.0f };	// Bottom right.
			vertexPtr[index].texture = { m_Font[letter].right, 1.0f };
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			_drawX = _drawX + m_Font[letter].size + 1.0f;
		}
	}

	return;
}