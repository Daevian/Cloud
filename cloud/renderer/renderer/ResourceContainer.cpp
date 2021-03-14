#include "stdafx.h"
#include "ResourceContainer.h"


Cloud::Renderer::ResourceId::ResourceId(const t_char* name)
    : m_hash(std::hash<std::string>()(name))
    , m_name(name)
{
}

Cloud::Renderer::ResourceContainer::ResourceContainer()
{

}
