#include "stdafx.h"
#include "TextureContainer.h"

#include "Texture.h"

Cloud::Renderer::TextureContainer::TextureContainer()
:m_textures(128, 128)
{
}

Cloud::Renderer::TextureContainer::~TextureContainer()
{
    for (int i = 0; i < m_textures.Count(); ++i)
    {
        m_textures[i]->Unload();
        delete m_textures[i];
    }

    m_textures.RemoveAll();
}

Cloud::Renderer::Texture* Cloud::Renderer::TextureContainer::GetTexture(const std::string& texturePath)
{
    for (int i = 0; i < m_textures.Count(); ++i)
    {
        if (m_textures[i]->GetPath() == texturePath)
        {
            return m_textures[i];
        }
    }

    return LoadTexture(texturePath);
}

Cloud::Renderer::Texture* Cloud::Renderer::TextureContainer::LoadTexture(const std::string& texturePath)
{
    Texture* texture = new Texture(texturePath);
    texture->Load();
    m_textures.Add(texture);

    return texture;
}