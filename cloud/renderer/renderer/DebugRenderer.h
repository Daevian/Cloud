#ifndef CLOUD_RENDERER_DEBUG_RENDERER_HEADER
#define CLOUD_RENDERER_DEBUG_RENDERER_HEADER

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "GfxInstanceBuffer.h"

namespace Cloud
{
namespace Renderer
{
    class ShaderEffect;

    class DebugRenderer
    {
    public:
        enum class BarDirection
        {
            Left,
            Right,
            Up,
            Down,
        };

        DebugRenderer();

        bool Initialise();
        void Uninitialise();

        void AddLine2D(const float2& start, const float2& end, const float4& colour);
        void AddBar(const float2& position, const float2& size, const float4& colour, float value, BarDirection direction = BarDirection::Up);
        void AddQuad(const float2& topLeft, const float2& bottomRight, const float4& colour);
        void AddBox(const float3& position, const float3& rotation, const float3& scale, const float4& colour);
        void AddBox(const ClMatrix4& matrix, const float4& colour);
        void RecordCommandList(ID3D12GraphicsCommandList* commandList);

    private:
        bool InitialiseLine2D();
        bool InitialiseQuad();
        bool InitialiseBox();
        void RenderLine2D();
        void RenderQuads(ID3D12GraphicsCommandList* commandList);
        void RenderBoxes(ID3D12GraphicsCommandList* commandList);

        struct Line2D
        {
            static const uint c_maxCount = 1024;
            static const uint c_vertexCount = c_maxCount * 2;

            struct Vertex
            {
                float4 position;
                float4 colour;
            };
        };

        struct Quad
        {
            static const uint c_maxCount = 1024;
            static const uint c_vertexCount = c_maxCount * 4;
            static const uint c_indexCount = c_maxCount * 6;

            struct Vertex
            {
                float4 position;
                float4 colour;
            };
        };

        struct Box
        {
            static const uint c_maxCount = 1024;

            struct Vertex
            {
                float4 pos;
                float3 norm;
            };

            struct Instance
            {
                ClMatrix4 model;
                float4  colour;
            };

            struct InstanceBuffer
            {
                Utils::StaticArray<Math::Matrix4, c_maxCount> m_instanceMatrices;
            };
        };

        // line2D
        Utils::StaticArray<Line2D::Vertex, Line2D::c_vertexCount> m_line2DVertices;
        VertexBuffer m_line2DVB;
        ShaderEffect* m_line2DEffect;

        // quad
        Utils::StaticArray<Quad::Vertex, Quad::c_vertexCount> m_quadVertices;
        VertexBuffer m_quadVB;
        IndexBuffer  m_quadIB;
        ShaderEffect* m_quadEffect;

        // box
        Utils::StaticArray<Box::Instance, Box::c_maxCount> m_boxInstances;
        Box::InstanceBuffer m_boxInstanceData;
        VertexBuffer m_boxVB;
        IndexBuffer  m_boxIB;
        VertexBuffer m_boxInstanceBuffer;
        //GfxInstanceBuffer m_boxInstanceBuffer;
        ShaderEffect* m_boxEffect;

        uint m_line2DsToRender;
        uint m_quadsToRender;
        uint m_boxesToRender;

        
    };
}
}

#endif // CLOUD_RENDERER_DEBUG_RENDERER_HEADER