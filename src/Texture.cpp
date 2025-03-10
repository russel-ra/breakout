#include <iostream>

#include "Texture.h"

#include "stb_image/stb_image.h"

Texture::Texture(const std::string& filepath)
	: m_ID(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Width(0),
	m_Height(0), m_BPP(0)
{
	// OpenGL begins 0,0 at bottom-left of the texture so we need this
	// NEW NOTE: SpriteRenderer will handle the OpenGL odditity
	//stbi_set_flip_vertically_on_load(1);

	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4); // desired channels is 4 because RGBA

	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	// Minification filter needed when the texture is being rendered on an area smaller in pixels than the actual texture size and it will be linearly resampled
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Magnification filter needed when the texture is being rendered on an area larger in pixels than the actual texture size so it needs to be scaled up
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Horizontal wrap, with clamping, meaning the area is not extended
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	// Tiling. We don't want to do that. S and T are like X and Y but for textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 0 because this isn't a multi-level texture. RGBA8, means 8 bits per channel. RGBA8 is how GL will be storing our texture data. 0 Pixels for the border. GL_RGBA is the format of the data we're reading
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, m_LocalBuffer);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}

}

Texture::~Texture()
{
	// TODO: create a global texture manager so textures can be properly managed

	//std::cout << "DEALLOCATED: " << m_ID << std::endl;
	//glDeleteTextures(1, &m_ID);
}

void Texture::Bind(unsigned int slot) const
{
	// Number of texture slots on each platform varies. Typically desktop has 32 and mobile 8. Since these enum values count up by 1 we can offset by slot
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
