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

        const ClMatrix4& GetView() const { return m_view; }
        const ClMatrix4& GetProjection() const { return m_projection; }

        void SetTranslation(const ClFloat4& translation);
        void SetCameraMatrix(const ClMatrix4& matrix)   { m_cameraMatrix = matrix; }
        void SetPerspective(CLfloat fovY, CLfloat aspectRatio, CLfloat nearClip, CLfloat farClip);

        void Translate(const ClFloat4& translation);
        void UpdateView();

    private:
        ClMatrix4 m_cameraMatrix;

        ClMatrix4 m_view;
        ClMatrix4 m_projection;
    };
}
}

#endif // CLOUD_RENDERER_CAMERA_HEADER