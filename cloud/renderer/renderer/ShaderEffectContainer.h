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

            std::shared_ptr<ShaderEffect> FindEffect(const std::string& effectPath);
            ShaderEffect* GetEffect(const std::string& effectPath);

        private:
            ShaderEffect* LoadEffect(const std::string& effectPath);

            std::unordered_map<ClString, std::weak_ptr<ShaderEffect>> m_effects;
            std::mutex m_cacheLock;

        };
    }
}

#endif // CLOUD_RENDERER_SHADEREFFECTCONTAINER_HEADER