#include "stdafx.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

void GenerateSphereIbVb(std::vector<Cloud::Renderer::Mesh::Vertex>& vertices, std::vector<uint32>& indices, int latBands, int longBands)
{
    const float c_radius = 0.5f;
    for (int latNumber = 0; latNumber <= latBands; latNumber++)
    {
        float theta = latNumber * CL_PI / static_cast<float>(latBands);
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int longNumber = 0; longNumber <= longBands; longNumber++)
        {
            float phi = longNumber * 2 * CL_PI / static_cast<float>(longBands);
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            Cloud::Renderer::Mesh::Vertex vertex = {};
            vertex.norm = float3
            {
                cosPhi * sinTheta,
                cosTheta,
                sinPhi * sinTheta,
            };

            vertex.pos = float4(vertex.norm * c_radius, 1.0f);

            //vs.Texcoord[0] = 1 - (longNumber / static_cast<float>(longBands));
            //vs.Texcoord[1] = 1 - (latNumber / static_cast<float>(latBands));

            vertices.emplace_back(vertex);
        }
    }

    for (int latNumber = 0; latNumber < latBands; latNumber++)
    {
        for (int longNumber = 0; longNumber < longBands; longNumber++)
        {
            int first = (latNumber * (longBands + 1)) + longNumber;
            int second = first + longBands + 1;

            indices.emplace_back(first);
            indices.emplace_back(first + 1);
            indices.emplace_back(second);

            indices.emplace_back(second);
            indices.emplace_back(first + 1);
            indices.emplace_back(second + 1);
        }
    }
}

Cloud::Renderer::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
{
    m_vb = std::make_unique<VertexBuffer>();
    m_ib = std::make_unique<IndexBuffer>();

    m_vb->SetVertexData(vertices.data());
    m_vb->SetVertexCount(static_cast<int>(vertices.size()));
    m_vb->SetVertexSize(sizeof(Vertex));
    m_vb->Initialise();

    m_ib->SetIndexData(indices.data());
    m_ib->SetIndexCount(static_cast<int>(indices.size()));
    m_ib->Initialise();
}

std::shared_ptr<Cloud::Renderer::Mesh> Cloud::Renderer::Mesh::CreateSphere(uint latBands, uint longBands)
{
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;
    GenerateSphereIbVb(vertices, indices, latBands, longBands);

    return std::make_shared<Cloud::Renderer::Mesh>(vertices, indices);
}

std::shared_ptr<Cloud::Renderer::Mesh> Cloud::Renderer::Mesh::CreatePlane()
{
    std::vector<Vertex> vertices =
    {
        Vertex{ float4(-1.0f, 0.0f,  1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },
        Vertex{ float4( 1.0f, 0.0f,  1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },
        Vertex{ float4(-1.0f, 0.0f, -1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },
        Vertex{ float4( 1.0f, 0.0f, -1.0f, 1.0f), float3(0.0f, 1.0f, 0.0f) },
    };
    std::vector<uint32> indices =
    {
        0, 1, 2,
        2, 1, 3,
    };

    return std::make_shared<Cloud::Renderer::Mesh>(vertices, indices);
}
