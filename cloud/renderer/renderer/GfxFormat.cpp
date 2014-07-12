#include "stdafx.h"
#include "GfxFormat.h"


Cloud::Renderer::GfxFormat Cloud::Renderer::GetFormat(const std::string& formatName)
{
    if (formatName == "R32G32_FLOAT")           return GfxFormat::R32G32_FLOAT;
    if (formatName == "R32G32B32_FLOAT")        return GfxFormat::R32G32B32_FLOAT;
    if (formatName == "R32G32B32A32_FLOAT")     return GfxFormat::R32G32B32A32_FLOAT;
    
    return GfxFormat::UNKNOWN;
}

CLuint Cloud::Renderer::GetFormatSize(GfxFormat format)
{
    switch (format)
    {
        case GfxFormat::R32G32_FLOAT:
            return 8;
        case GfxFormat::R32G32B32_FLOAT:
            return 12;
        case GfxFormat::R32G32B32A32_FLOAT:
            return 16;
        case GfxFormat::UNKNOWN:
        default:
            return 0;
    }
}