#ifndef CLOUD_RENDERER_SHADEREFFECTCONTAINER_HEADER
#define CLOUD_RENDERER_SHADEREFFECTCONTAINER_HEADER

#include "../../core_libraries/utilities/DynamicArray.h"

namespace Cloud
{
    namespace Renderer
    {
        class ShaderEffect;

        class ShaderEffectContainer
        {
        public:
            ShaderEffectContainer();
            ~ShaderEffectContainer();

            ShaderEffect* GetEffect(const std::string& effectPath);

        private:
            ShaderEffect* LoadEffect(const std::string& effectPath);

            Utils::DynamicArray<ShaderEffect*> m_effects;

        };
    }
}

#endif // CLOUD_RENDERER_SHADEREFFECTCONTAINER_HEADER