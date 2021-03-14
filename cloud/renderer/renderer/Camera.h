#ifndef CLOUD_RENDERER_CAMERA_HEADER
#define CLOUD_RENDERER_CAMERA_HEADER

#include "VertexBuffer.h"

namespace Cloud
{
namespace Renderer
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

        void SetTranslation(const float4& translation);
        void SetRotation(const ClMatrix4& rotationMatrix);
        void SetPerspective(float fovY, float aspectRatio, float nearClip, float farClip);

        const ClMatrix4& GetCameraTransform()                               const { return m_cameraTransform; }
        const ClMatrix4& GetView()                                          const { return m_view; }
        const ClMatrix4& GetProjection()                                    const { return m_projection; }

        void SetCameraTransform(const ClMatrix4& matrix)                       { m_cameraTransform = matrix; }

        void Translate(const float4& translation);
        void UpdateView();

    private:
        ClMatrix4 m_cameraTransform;
        ClMatrix4 m_view;
        ClMatrix4 m_projection;
    };
}
}

#endif // CLOUD_RENDERER_CAMERA_HEADER