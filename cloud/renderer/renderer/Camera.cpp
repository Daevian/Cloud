#include "stdafx.h"
#include "Camera.h"


Cloud::Renderer::Camera::Camera()
{
}

Cloud::Renderer::Camera::~Camera()
{
}

void Cloud::Renderer::Camera::SetTranslation(const ClFloat4& translation)
{
    m_cameraMatrix.SetCol3(translation);
}

void Cloud::Renderer::Camera::SetPerspective(CLfloat fovY, CLfloat aspectRatio, CLfloat nearClip, CLfloat farClip)
{
    m_projection = ClMatrix4::Perspective(fovY, aspectRatio, nearClip, farClip);
}

void Cloud::Renderer::Camera::Translate(const ClFloat4& translation)
{
    m_cameraMatrix = ClMatrix4::Multiply(ClMatrix4::Translation(translation), m_cameraMatrix);
}

void Cloud::Renderer::Camera::UpdateView()
{
    m_view = ClMatrix4::Inverse(m_cameraMatrix);
}