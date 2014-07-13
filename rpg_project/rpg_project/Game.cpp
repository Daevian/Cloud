#include "stdafx.h"
#include "Game.h"

#include "GameApplication.h"

RPG::Game::Game()
{
}

void RPG::Game::Initialise()
{
    m_vehicle.Initialise();
}

void RPG::Game::Update(CLfloat timeStep)
{
    if (Application::Instance().GetInput().GetPadButtonPressed(0, Cloud::Input::PadButton::Back))
    {
        m_vehicle.Reset();
    }

    if (Application::Instance().GetInput().GetPadButtonPressed(0, Cloud::Input::PadButton::B))
    {
        if (!Application::Instance().IsDebugCameraActive())
        {
            auto& renderer = Application::Instance().GetRenderer();
            m_vehicle.AttachCamera(&renderer.GetCamera());
        }
        else
        {
            m_vehicle.AttachCamera(0);
            
        }
    }

    m_vehicle.Update(timeStep);
}

void RPG::Game::Render()
{
    m_vehicle.Render();
}


////////////////// vehicle engine
RPG::VehicleEngine::VehicleEngine()
    : m_transform(ClMatrix4::Identity())
    , m_offset(0.0f, 0.0f, 0.0f, 0.0f)
    , m_position(0.0f, 0.0f, 0.0f, 1.0f)
    , m_rotation(0.0f, 0.0f, 0.0f, 0.0f)
    , m_rotationTarget(0.0f, 0.0f, 0.0f, 0.0f)
    , m_strafe(0.0f)
    , m_strafeTarget(0.0f)
    , m_acceleration(0.0f)
    , m_brakeForce(0.0f)
    , m_enginePivot(0)
{
}

void RPG::VehicleEngine::Update(CLfloat timeStep)
{
    // update position
    {
        const CLfloat movementSpeed = 3.0f;
        ClFloat4 originalPosition = ClFloat4(0.0f, 0.0f, 0.0f, 1.0f) + m_offset;
        ClFloat4 positionTarget = originalPosition + ClFloat4(0.0f, 0.0f, m_acceleration * 0.5f, 0.0f);
        const auto difference = positionTarget - m_position;
        m_position += difference * movementSpeed * timeStep;
    }
    
    // update rotation
    {
        const ClFloat4 rotationRangeMin(-0.1f, -0.1f, 0.0f, 0.0f);
        const ClFloat4 rotationRangeMax(0.1f, 0.1f, 0.0f, 0.0f);
        const CLfloat rotationSpeed = 3.0f;
        ClFloat4 rotationTarget = ClClamp(rotationRangeMin, rotationRangeMax, m_rotationTarget);
        const auto difference = rotationTarget - m_rotation;
        m_rotation += difference * rotationSpeed * timeStep;
    }

    // update strafe
    {
        const CLfloat springFactor = 2.0f;
        const CLfloat strafeSpeed = 20.0f;
        const CLfloat strafeRange = 0.6f;
        CLfloat strafeTarget = ClClamp(-strafeRange, strafeRange, m_strafeTarget);
        const auto difference = strafeTarget - m_strafe;
        m_strafe += difference * strafeSpeed * timeStep * ClAbs(m_rotation.y);
        m_strafe *= 1.0f - springFactor * timeStep;
    }

    // update transform
    {
        m_transform = ClMatrix4::Identity();
        m_transform *= ClMatrix4::Rotation(m_rotation);
        m_transform.SetCol3(m_position + ClFloat4(m_strafe, 0.0f, 0.0f, 0.0f));

        if (m_enginePivot)
        {
            m_transform *= m_enginePivot->GetTransform();
        }
    }
}

void RPG::VehicleEngine::Reset()
{
    m_transform = ClMatrix4::Identity();
    m_position = ClFloat4(0.0f, 0.0f, 0.0f, 1.0f) + m_offset;
    m_rotation.Set(0.0f, 0.0f, 0.0f, 0.0f);
    m_rotationTarget.Set(0.0f, 0.0f, 0.0f, 0.0f);
    m_strafe = 0.0f;
    m_strafeTarget = 0.0f;
    m_acceleration = 0.0f;
}

void RPG::VehicleEngine::Render()
{
    ClMatrix4 boxMatrix = ClMatrix4::Identity();
    boxMatrix *= ClMatrix4::Scale(ClFloat4(0.5f, 0.5f, 1.0f, 0.0f));
    boxMatrix *= m_transform;

    auto& renderer = Application::Instance().GetRenderer().GetDebugRendererForGame();
    renderer.AddBox(boxMatrix, FLOAT4_BLUE(1.0f));
}

////////////////// vehicle engine pivot
RPG::VehicleEnginePivot::VehicleEnginePivot()
    : m_transform(ClMatrix4::Identity())
    , m_position(0.0f, 0.0f, 0.0f, 1.0f)
    , m_rotation(0.0f, 0.0f, 0.0f, 0.0f)
    , m_rotationTarget(0.0f, 0.0f, 0.0f, 0.0f)
    , m_heading(0.0f, 0.0f, 1.0f, 0.0f)
    , m_velocity(0.0f)
    , m_steerAmount(0.0f)
    , m_strafeAmount(0.0f)
    , m_strafeVelocity(0.0f)
    , m_accelerationMultiplier(300.0f)
    , m_maxVelocity(340.0f)
    , m_brakeDragMultiplier(3.0f)
{
    for (CLuint i = 0; i < m_engines.Count(); ++i)
    {
        m_engines[i].SetEnginePivot(this);
    }

    m_engines[(CLuint)EngineSide::Left].SetOffset(ClFloat4(-1.2f, 0.0f, -0.5f, 0.0f));
    m_engines[(CLuint)EngineSide::Right].SetOffset(ClFloat4(1.2f, 0.0f, -0.5f, 0.0f));
}

void RPG::VehicleEnginePivot::Initialise()
{
    Reset();
}

void RPG::VehicleEnginePivot::Reset()
{
    m_transform = ClMatrix4::Identity();
    m_position.Set(0.0f, 0.0f, 4.5f, 1.0f);
    m_rotation.Set(0.0f, 0.0f, 0.0f, 0.0f);
    m_rotationTarget.Set(0.0f, 0.0f, 0.0f, 0.0f);
    m_heading.Set(0.0f, 0.0f, 1.0f, 0.0f);
    m_velocity = 0.0f;

    for (CLuint i = 0; i < m_engines.Count(); ++i)
    {
        m_engines[i].Reset();
    }
}

void RPG::VehicleEnginePivot::Update(CLfloat timeStep)
{
    // update heading
    {
        m_heading = ClMatrix4::Rotation(ClFloat4(0.0f, m_steerAmount * timeStep, 0.0f, 0.0f)) * m_heading;
    }

    ClMatrix4 headingMatrix = ClMatrix4::LookTo(0.0f, m_heading, ClFloat4(0.0f, 1.0f, 0.0f, 0.0f));

    // update position
    {
        CLfloat normalisedAcceleration = 0.0f;
        CLfloat brakeForce = 0.0f;
        for (CLuint i = 0; i < m_engines.Count(); ++i)
        {
            normalisedAcceleration += m_engines[i].GetAcceleration() * 0.5f;
            brakeForce += m_engines[i].GetBrakeForce() * 0.5f;
        }

        CLfloat acceleration = normalisedAcceleration * m_accelerationMultiplier;

        const CLfloat strafeAcceleration = 500.0f;
        const CLfloat maxStrafeSpeed = 50.0f;
        const CLfloat strafeDrag = 5.0f;
        m_strafeVelocity += m_strafeAmount * strafeAcceleration * acceleration * timeStep;
        m_strafeVelocity /= 1.0f + strafeDrag * timeStep;
        m_strafeVelocity = ClClamp(-maxStrafeSpeed, maxStrafeSpeed, m_strafeVelocity);

        ClFloat4 localSpaceVelocity(0.0f, 0.0f, 0.0f, 0.0f);

        const CLfloat drift = 0.8f;
        const CLfloat drag = 1.0f + brakeForce * m_brakeDragMultiplier;

        const ClFloat4 maxVelocity(m_maxVelocity, m_maxVelocity, m_maxVelocity, 0.0f);
        const ClFloat4 minVelocity(-m_maxVelocity, -m_maxVelocity, -m_maxVelocity, 0.0f);
        localSpaceVelocity = headingMatrix * m_velocity;
        localSpaceVelocity += ClFloat4(0.0f, 0.0f, acceleration * timeStep, 0.0f);
        localSpaceVelocity *= ClFloat4(1.0f - (1.0f / drift) * timeStep, 1.0f, 1.0f, 0.0f);
        localSpaceVelocity /= 1.0f + drag * timeStep;
        localSpaceVelocity = ClClamp(minVelocity, maxVelocity, localSpaceVelocity);

        const ClMatrix4 inverseHeadingMatrix = ClMatrix4::Transpose(headingMatrix);
        m_velocity = inverseHeadingMatrix  * localSpaceVelocity;
        const ClFloat4 strafeVelocity = inverseHeadingMatrix * ClFloat4(m_strafeVelocity, 0.0f, 0.0f, 0.0f);

        m_position += (strafeVelocity + m_velocity) * timeStep;
    }
    
    // update rotation
    {
        const CLfloat transitionSpeed = 3.0f;
        const auto difference = m_rotationTarget - m_rotation;
        m_rotation += difference * transitionSpeed * timeStep;
    }

    // update transform
    {
        m_transform = ClMatrix4::Identity();
        m_transform *= ClMatrix4::Transpose(headingMatrix) * ClMatrix4::Rotation(m_rotation);
        m_transform.SetCol3(m_position);

        m_transformWithoutRotation = ClMatrix4::Identity();
        m_transformWithoutRotation *= ClMatrix4::Transpose(headingMatrix);
        m_transformWithoutRotation.SetCol3(m_position);
    }

    // update engines
    {
        for (CLuint i = 0; i < m_engines.Count(); ++i)
        {
            m_engines[i].Update(timeStep);
        }
    }
}

void RPG::VehicleEnginePivot::Render()
{
    ClMatrix4 boxMatrix = ClMatrix4::Identity();
    boxMatrix *= ClMatrix4::Scale(ClFloat4(0.2f, 0.2f, 0.2f, 0.0f));
    boxMatrix *= m_transform;

    auto& renderer = Application::Instance().GetRenderer().GetDebugRendererForGame();
    renderer.AddBox(boxMatrix, FLOAT4_YELLOW(1.0f));

    for (CLuint i = 0; i < m_engines.Count(); ++i)
    {
        m_engines[i].Render();
    }
}

void RPG::VehicleEnginePivot::SetEngineRotation(EngineSide engine, const ClFloat2& rotation)
{
    m_engines[(CLuint)engine].SetRotationTarget(rotation);
}

void RPG::VehicleEnginePivot::SetEngineStrafe(EngineSide engine, const CLfloat strafe)
{
    m_engines[(CLuint)engine].SetStrafeTarget(strafe);
}

void RPG::VehicleEnginePivot::SetEngineAcceleration(EngineSide engine, CLfloat acceleration)
{
    m_engines[(CLuint)engine].SetAcceleration(acceleration);
}

void RPG::VehicleEnginePivot::SetEngineBrakeForce(EngineSide engine, CLfloat brakeForce)
{
    m_engines[(CLuint)engine].SetBrakeForce(brakeForce);
}

////////////////// vehicle pod
RPG::VehiclePod::VehiclePod()
{
}

void RPG::VehiclePod::Update(CLfloat timeStep)
{
    timeStep;
}

void RPG::VehiclePod::Render()
{
    auto& renderer = Application::Instance().GetRenderer().GetDebugRendererForGame();
    renderer.AddBox(m_position.GetXYZ(), ClFloat3(0.0f, 0.0f, 0.0f), ClFloat3(0.5f, 0.2f, 1.0f), FLOAT4_GREEN(1.0f));
}

////////////////// vehicle
RPG::Vehicle::Vehicle()
    : m_camera(0)
    , m_cameraTransform(ClMatrix4::Identity())
{
}

void RPG::Vehicle::Initialise()
{
    Reset();
}

void RPG::Vehicle::Reset()
{
    m_enginePivot.Reset();
    m_pod.SetPosition(ClFloat4(0.0, 0.5f, -0.5f, 1.0f));
}

void RPG::Vehicle::Update(CLfloat timeStep)
{
    using namespace Cloud::Input;

    if (!Application::Instance().IsDebugCameraActive())
    {
        auto& input = Application::Instance().GetInput();
        auto leftAccel = input.GetPadLeftTrigger(0);
        auto rightAccel = input.GetPadRightTrigger(0);
        auto leftBrake = input.GetPadButtonDown(0, PadButton::LeftShoulder);
        auto rightBrake = input.GetPadButtonDown(0, PadButton::RightShoulder);
        auto leftRotation = input.GetPadLeftStick(0);
        auto rightRotation = input.GetPadRightStick(0);

        CLfloat strafe = 0.0f;
        CLfloat leftStrafe = leftRotation.x;
        CLfloat rightStrafe = rightRotation.x;// - leftRotation.x;
        CLfloat steerAmount = 0.0f;
        if (leftRotation.x < 0.0f)
        {
            strafe += leftRotation.x;
            if (rightRotation.x < 0.0f)
            {
                steerAmount += (leftRotation.x + rightRotation.x) * 0.5f;
            }
        }
        
        if (rightRotation.x > 0.0f)
        {
            strafe += rightRotation.x;
            if (leftRotation.x > 0.0f)
            {
                steerAmount += (leftRotation.x + rightRotation.x) * 0.5f;
            }
        }

        strafe -= steerAmount;
        leftStrafe -= steerAmount;
        rightStrafe -= steerAmount;
        
        CLfloat roll = steerAmount;

        ClFloat2 leftEngineRotation(leftRotation.y, leftRotation.x);
        ClFloat2 rightEngineRotation(rightRotation.y, rightRotation.x);
        
        // pitch engines up and remove yaw when pivot is rolling
        leftEngineRotation -= ClFloat2(ClAbs(roll), roll);
        rightEngineRotation -= ClFloat2(ClAbs(roll), roll);

        m_enginePivot.SetSteerAmount(steerAmount);
        m_enginePivot.SetStrafeAmount(strafe);
        m_enginePivot.SetRotationTarget(ClFloat3(0.0f, roll, -roll));

        m_enginePivot.SetEngineAcceleration(EngineSide::Left, !leftBrake ? leftAccel : 0.0f);
        m_enginePivot.SetEngineAcceleration(EngineSide::Right, !rightBrake ? rightAccel : 0.0f);
        m_enginePivot.SetEngineBrakeForce(EngineSide::Left, leftBrake ? 1.0f : 0.0f);
        m_enginePivot.SetEngineBrakeForce(EngineSide::Right, rightBrake ? 1.0f : 0.0f);
        m_enginePivot.SetEngineRotation(EngineSide::Left, leftEngineRotation);
        m_enginePivot.SetEngineRotation(EngineSide::Right, rightEngineRotation);
        m_enginePivot.SetEngineStrafe(EngineSide::Left, leftStrafe);
        m_enginePivot.SetEngineStrafe(EngineSide::Right, rightStrafe);



        
    }
    
    m_enginePivot.Update(timeStep);
    m_pod.Update(timeStep);

    UpdateCamera();
}

void RPG::Vehicle::Render()
{
    m_enginePivot.Render();
    m_pod.Render();
}

void RPG::Vehicle::AttachCamera(Cloud::Renderer::Camera* camera)
{
    if (camera)
    {
        m_cameraTransform = camera->GetCameraTransform();
        const auto& enginePivotTransform = m_enginePivot.GetTransformWithoutRotation();

        m_cameraTransform *= ClMatrix4::Inverse(enginePivotTransform);
    }
    else
    {
        const auto& enginePivotTransform = m_enginePivot.GetTransformWithoutRotation();    
        m_camera->SetCameraTransform(enginePivotTransform * m_cameraTransform);
    }

    m_camera = camera;
    
}

void RPG::Vehicle::UpdateCamera()
{
    if (m_camera)
    {
        const auto& enginePivotTransform = m_enginePivot.GetTransformWithoutRotation();    
        m_camera->SetCameraTransform(enginePivotTransform * m_cameraTransform);
    }
}