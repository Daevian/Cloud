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

        CLbool Initialise();
        void Uninitialise();

        void AddLine2D(const ClFloat2& start, const ClFloat2& end, const ClFloat4& colour);
        void AddBar(const ClFloat2& position, const ClFloat2& size, const ClFloat4& colour, CLfloat value, BarDirection direction = BarDirection::Up);
        void AddQuad(const ClFloat2& topLeft, const ClFloat2& bottomRight, const ClFloat4& colour);
        void AddBox(const ClFloat3& position, const ClFloat3& rotation, const ClFloat3& scale, const ClFloat4& colour);
        void AddBox(const ClMatrix4& matrix, const ClFloat4& colour);
        void RecordCommandList(ID3D12GraphicsCommandList* commandList);

    private:
        CLbool InitialiseLine2D();
        CLbool InitialiseQuad();
        CLbool InitialiseBox();
        void RenderLine2D();
        void RenderQuads(ID3D12GraphicsCommandList* commandList);
        void RenderBoxes(ID3D12GraphicsCommandList* commandList);

        struct Line2D
        {
            static const CLuint c_maxCount = 1024;
            static const CLuint c_vertexCount = c_maxCount * 2;

            struct Vertex
            {
                ClFloat4 position;
                ClFloat4 colour;
            };
        };

        struct Quad
        {
            static const CLuint c_maxCount = 1024;
            static const CLuint c_vertexCount = c_maxCount * 4;
            static const CLuint c_indexCount = c_maxCount * 6;

            struct Vertex
            {
                ClFloat4 position;
                ClFloat4 colour;
            };
        };

        struct Box
        {
            static const CLuint c_maxCount = 1024;

            struct Vertex
            {
                ClFloat4 pos;
                ClFloat3 norm;
            };

            struct Instance
            {
                ClMatrix4 model;
                ClFloat4  colour;
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

        CLuint m_line2DsToRender;
        CLuint m_quadsToRender;
        CLuint m_boxesToRender;

        
    };
}
}

#endif // CLOUD_RENDERER_DEBUG_RENDERER_HEADER