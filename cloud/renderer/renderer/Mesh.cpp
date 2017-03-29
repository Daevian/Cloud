#include "stdafx.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

void GenerateSphereIbVb(std::vector<Cloud::Renderer::Mesh::Vertex>& vertices, std::vector<CLuint32>& indices, CLint latBands, CLint longBands)
{
    const CLfloat c_radius = 0.5f;
    for (CLint latNumber = 0; latNumber <= latBands; latNumber++)
    {
        CLfloat theta = latNumber * CL_PI / static_cast<CLfloat>(latBands);
        CLfloat sinTheta = std::sin(theta);
        CLfloat cosTheta = std::cos(theta);

        for (CLint longNumber = 0; longNumber <= longBands; longNumber++)
        {
            CLfloat phi = longNumber * 2 * CL_PI / static_cast<CLfloat>(longBands);
            CLfloat sinPhi = std::sin(phi);
            CLfloat cosPhi = std::cos(phi);

            Cloud::Renderer::Mesh::Vertex vertex = {};
            vertex.norm = ClFloat3
            {
                cosPhi * sinTheta,
                cosTheta,
                sinPhi * sinTheta,
            };

            vertex.pos = ClFloat4(vertex.norm * c_radius, 1.0f);

            //vs.Texcoord[0] = 1 - (longNumber / static_cast<CLfloat>(longBands));
            //vs.Texcoord[1] = 1 - (latNumber / static_cast<CLfloat>(latBands));

            vertices.emplace_back(vertex);
        }
    }

    for (CLint latNumber = 0; latNumber < latBands; latNumber++)
    {
        for (CLint longNumber = 0; longNumber < longBands; longNumber++)
        {
            CLint first = (latNumber * (longBands + 1)) + longNumber;
            CLint second = first + longBands + 1;

            indices.emplace_back(first);
            indices.emplace_back(first + 1);
            indices.emplace_back(second);

            indices.emplace_back(second);
            indices.emplace_back(first + 1);
            indices.emplace_back(second + 1);
        }
    }
}

Cloud::Renderer::Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<CLuint32>& indices)
{
    m_vb = std::make_unique<VertexBuffer>();
    m_ib = std::make_unique<IndexBuffer>();

    m_vb->SetVertexData(vertices.data());
    m_vb->SetVertexCount(static_cast<CLint>(vertices.size()));
    m_vb->SetVertexSize(sizeof(Vertex));
    m_vb->Initialise();

    m_ib->SetIndexData(indices.data());
    m_ib->SetIndexCount(static_cast<CLint>(indices.size()));
    m_ib->Initialise();
}

std::shared_ptr<Cloud::Renderer::Mesh> Cloud::Renderer::Mesh::CreateSphere(CLuint latBands, CLuint longBands)
{
    std::vector<Vertex> vertices;
    std::vector<CLuint32> indices;
    GenerateSphereIbVb(vertices, indices, latBands, longBands);

    return std::make_shared<Cloud::Renderer::Mesh>(vertices, indices);
}

std::shared_ptr<Cloud::Renderer::Mesh> Cloud::Renderer::Mesh::CreatePlane()
{
    std::vector<Vertex> vertices =
    {
        Vertex{ ClFloat4(-1.0f, 0.0f,  1.0f, 1.0f), ClFloat3(0.0f, 1.0f, 0.0f) },
        Vertex{ ClFloat4( 1.0f, 0.0f,  1.0f, 1.0f), ClFloat3(0.0f, 1.0f, 0.0f) },
        Vertex{ ClFloat4(-1.0f, 0.0f, -1.0f, 1.0f), ClFloat3(0.0f, 1.0f, 0.0f) },
        Vertex{ ClFloat4( 1.0f, 0.0f, -1.0f, 1.0f), ClFloat3(0.0f, 1.0f, 0.0f) },
    };
    std::vector<CLuint32> indices =
    {
        0, 1, 2,
        2, 1, 3,
    };

    return std::make_shared<Cloud::Renderer::Mesh>(vertices, indices);
}
