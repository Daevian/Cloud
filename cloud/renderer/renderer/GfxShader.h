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
            ClDebugName     name;
            GfxShaderBlob   shaderBlob;
        };

        class GfxComputeShader : public GfxResource
        {
            friend class GfxShaderFactory;
        public:
            typedef std::unique_ptr<GfxComputeShader, Deleter> UniquePtr;

            ID3D11ComputeShader* GetShader() const { return m_shader; }

        private:
            GfxComputeShader();
            virtual ~GfxComputeShader() override;

            static UniquePtr MakeUnique() { return GfxComputeShader::UniquePtr(new GfxComputeShader(), GfxResource::Deleter()); }

            ID3D11ComputeShader*    m_shader;
            GfxComputerShaderDesc   m_desc;
        };


        class GfxShaderFactory
        {
        public:
            static CLbool CompileShader(const ClString& shaderPath, const ClString& entryPoint, const ClString& shaderModel, GfxShaderBlob& shaderBlobOutput);
            
            GfxComputeShader::UniquePtr Create(const GfxComputerShaderDesc& desc);
            
            void Destroy(GfxComputeShader* shader);

        private:

        };
    }
}

#endif // CLOUD_RENDERER_GFX_SHADER_HEADER