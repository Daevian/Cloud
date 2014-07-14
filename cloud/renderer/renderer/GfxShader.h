#ifndef CLOUD_RENDERER_GFX_SHADER_HEADER
#define CLOUD_RENDERER_GFX_SHADER_HEADER

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

        class GfxComputeShader
        {
            friend class GfxShaderFactory;
        public:
            ID3D11ComputeShader* GetShader() const { return m_shader; }

        private:
            GfxComputeShader();
            ~GfxComputeShader() {};

            ID3D11ComputeShader*    m_shader;
            GfxComputerShaderDesc   m_desc;
        };


        class GfxShaderFactory
        {
        public:
            static CLbool CompileShader(const ClString& shaderPath, const ClString& entryPoint, const ClString& shaderModel, GfxShaderBlob& shaderBlobOutput);
            
            GfxComputeShader* Create(const GfxComputerShaderDesc& desc);
            
            void Destroy(GfxComputeShader* shader);

        private:

        };
    }
}

#endif // CLOUD_RENDERER_GFX_SHADER_HEADER