#include "stdafx.h"
#include "SpriteFactory.h"

#include "RenderCore.h"
#include "Sprite.h"

Cloud::Renderer::SpriteFactory::SpriteFactory()
{
}

CLbool Cloud::Renderer::SpriteFactory::CreateSprite(const std::string& spriteName, Sprite& spriteOutput)
{
    return ParseSprite(spriteName, spriteOutput);
}

CLbool Cloud::Renderer::SpriteFactory::ParseSprite(const std::string& spriteName, Sprite& spriteOutput)
{
    Json::Value root;
    auto spritePath = "data/assets/sprites/" + spriteName + ".sprite";
    std::ifstream jsonFile(spritePath.c_str());
    Json::Reader reader;

    CLbool parseSuccessful = reader.parse(jsonFile, root);
    if (!parseSuccessful)
    {
        std::stringstream assertMessage;
        assertMessage << "Failed to parse " << spritePath << " file" << std::endl << reader.getFormatedErrorMessages();

        CL_TRACE_CHANNEL("ERROR", assertMessage.str().c_str());
        CL_ASSERT_MSG("Failed to parse effect file!");
        return false;
    }

    // Parsing from json

    spriteOutput.Init(RenderCore::Instance().GetTextureContainer().GetTexture(root["Texture"].asString()));

    return true;
}