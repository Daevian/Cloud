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
    m_cameraTransform.SetCol3(translation);
}

void Cloud::Renderer::Camera::SetRotation(const ClMatrix4& rotationMatrix)
{
    m_cameraTransform.SetUpper3x3(rotationMatrix);
}

void Cloud::Renderer::Camera::SetPerspective(CLfloat fovY, CLfloat aspectRatio, CLfloat nearClip, CLfloat farClip)
{
    m_projection = ClMatrix4::Perspective(fovY, aspectRatio, nearClip, farClip);
}

void Cloud::Renderer::Camera::Translate(const ClFloat4& translation)
{
    m_cameraTransform = ClMatrix4::Multiply(ClMatrix4::Translation(translation), m_cameraTransform);
}

void Cloud::Renderer::Camera::UpdateView()
{
    m_view = ClMatrix4::Inverse(m_cameraTransform);
}