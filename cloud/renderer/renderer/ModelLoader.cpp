#include "stdafx.h"
#include "ModelLoader.h"
#include "ModelRenderer.h"
#include "Mesh.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

std::unique_ptr<Cloud::Renderer::ModelInstance> Cloud::Renderer::ModelLoader::LoadModel(const t_char* modelPath)
{
    auto&& scene = m_assetImporter.ReadFile(modelPath, 0);

    // mesh
    auto&& loadedMesh = scene->mMeshes[0];
    auto&& loadedVertices = std::span<aiVector3D>(loadedMesh->mVertices, loadedMesh->mNumVertices);
    uint index = 0;

    std::vector<Mesh::Vertex> vertices(loadedVertices.size());
    std::vector<uint32> indices(loadedVertices.size());

    for (auto&& vertex : loadedVertices)
    {
        auto&& normal = loadedMesh->mNormals[index];
        vertices[index] = Mesh::Vertex{ float4(vertex.x, vertex.y, vertex.z, 1.0f), float3(normal.x, normal.y, normal.z) };
        indices[index] = index;
        ++index;
    }

    auto&& mesh = std::make_shared<Cloud::Renderer::Mesh>(vertices, indices);

    return std::make_unique<ModelInstance>(mesh);
}

