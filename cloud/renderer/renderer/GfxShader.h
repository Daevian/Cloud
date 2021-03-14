#ifndef CLOUD_RENDERER_GFX_SHADER_HEADER
#define CLOUD_RENDERER_GFX_SHADER_HEADER

#include "GfxResource.h"

namespace Cloud
{
    namespace Renderer
    {
        struct GfxShaderBlob
        {
            ID3DBlob* blob;
        };

        struct GfxComputerShaderDesc
        {
            Cloud::DebugName     name;
            GfxShaderBlob   shaderBlob;
        };

        class GfxComputeShader : public GfxResource
        {
            friend class GfxShaderFactory;
        public:
            typedef std::unique_ptr<GfxComputeShader, Deleter> UniquePtr;

#ifdef USE_DIRECTX12
#else
            ID3D11ComputeShader* GetShader() const { return m_shader.get(); }
#endif

        private:
            GfxComputeShader();
            virtual ~GfxComputeShader() override;

            static UniquePtr MakeUnique() { return GfxComputeShader::UniquePtr(new GfxComputeShader(), GfxResource::Deleter()); }

#ifdef USE_DIRECTX12
#else
            Dx::UniquePtr<ID3D11ComputeShader>    m_shader;
#endif
            GfxComputerShaderDesc   m_desc;
        };


        class GfxShaderFactory
        {
        public:
            static bool CompileShader(const Cloud::String& shaderPath, const Cloud::String& entryPoint, const Cloud::String& shaderModel, GfxShaderBlob& shaderBlobOutput);
            
            GfxComputeShader::UniquePtr Create(const GfxComputerShaderDesc& desc);

        private:

        };
    }
}

#endif // CLOUD_RENDERER_GFX_SHADER_HEADER