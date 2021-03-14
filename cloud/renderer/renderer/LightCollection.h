#pragma once
#include "GfxConstantBuffer.h"

namespace Cloud
{
    namespace Renderer
    {
        /*class DirectionalLight
        {
        public:
            std::array<float, 3>  direction  = {0.0f, 1.0f, 0.0f};
            float intensity = 0.0f;
        };*/

        struct LightingCBufferData
        {
            struct DirectionalLight
            {
                std::array<float, 3>  direction = { 0.0f, 1.0f, 0.0f };
                float                 intensity = 0.0f;
            };

            struct PointLight
            {
                std::array<float, 3>  position = { 0.0f, -5.0f, 0.0f };
                float                 intensity = 0.0f;
            };

            static const uint c_pointLightCount = 1;

            DirectionalLight dirLight;
            std::array<PointLight, c_pointLightCount>   pointLights;
        };

        class LightCollection
        {
        public:
            LightCollection();

            const GfxConstantBuffer& GetCBuffer() const { return m_lightingCBuffer; }

            void Initialise();
            void Update();

            void PopulateDebugMenu();

        private:
            //DirectionalLight m_directionalLight;
            GfxConstantBuffer m_lightingCBuffer;
            LightingCBufferData m_lightCBufferData;
        };
    }
}