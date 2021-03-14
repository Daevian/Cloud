#ifndef RPG_GAME_HEADER
#define RPG_GAME_HEADER

using namespace Cloud::Utils;

namespace Cloud
{
namespace Renderer
{
    class Camera;
}
}

namespace RPG
{
    enum class EngineSide
    {
        Left    = 0,
        Right   = 1,
        Max = 2,
    };

    class VehicleEnginePivot;

    class VehicleEngine
    {
    public:
        VehicleEngine();

        void SetEnginePivot(VehicleEnginePivot* enginePivot)        { m_enginePivot = enginePivot; }

        void SetOffset(const float4& offset)                      { m_offset = offset; }
        void SetStrafeTarget(const float strafe)                  { m_strafeTarget = strafe; }
        void SetRotationTarget(const float2& rotation)            { m_rotationTarget.Set(rotation.x * 0.1f, rotation.y * 0.2f, 0.0f, 0.0f); }
        void SetAcceleration(float acceleration)                  { m_acceleration = acceleration; }
        void SetBrakeForce(float brakeForce)                      { m_brakeForce = brakeForce; }

        float GetAcceleration() const                             { return m_acceleration; }
        float GetBrakeForce() const                               { return m_brakeForce; }

        void Update(float timeStep);
        void Reset();

        void Render();

    private:
        ClMatrix4 m_transform;
        float4 m_offset;
        float4 m_position;
        float4 m_rotation;
        float4 m_rotationTarget;
        float m_strafe;
        float m_strafeTarget;
        float m_acceleration;
        float m_brakeForce;

        VehicleEnginePivot* m_enginePivot;
    };

    class VehicleEnginePivot
    {
    public:
        VehicleEnginePivot();

        void Initialise();
        void Reset();
        void Update(float timeStep);
        void Render();

        void SetEngineRotation(EngineSide engine, const float2& rotation);
        void SetEngineStrafe(EngineSide engine, const float strafe);
        void SetEngineAcceleration(EngineSide engine, float acceleration);
        void SetEngineBrakeForce(EngineSide engine, float brakes);

        void SetSteerAmount(const float steerAmount)              { m_steerAmount = steerAmount; }
        void SetStrafeAmount(const float strafeAmount)            { m_strafeAmount = strafeAmount; }
        void SetPosition(const float4& position)                  { m_position = position; }
        void SetRotationTarget(const float3& rotation)            { m_rotationTarget.Set(rotation.x * 0.1f, rotation.y * 0.2f, rotation.z * 0.2f, 0.0f); }

        const ClMatrix4& GetTransform() const                       { return m_transform; }
        const ClMatrix4& GetTransformWithoutRotation() const        { return m_transformWithoutRotation; }

    private:
        ClMatrix4 m_transform;
        ClMatrix4 m_transformWithoutRotation;

        float4 m_position;
        float4 m_rotation;
        float4 m_rotationTarget;
        float4 m_velocity;
        float4 m_heading;
        float m_steerAmount;
        float m_strafeAmount;
        float m_strafeVelocity;
        float m_accelerationMultiplier;
        float m_maxVelocity;
        float m_brakeDragMultiplier;

        StaticArray<VehicleEngine, (uint)EngineSide::Max> m_engines;
    };

    class VehiclePod
    {
    public:
        VehiclePod();

        void SetPosition(const float4& position)          { m_position = position; }

        void Update(float timeStep);
        void Render();

    private:
        float4 m_position;
    };

    class Vehicle
    {
    public:
        Vehicle();

        void Initialise();
        void Reset();
        void Update(float timeStep);
        void Render();

        void AttachCamera(Cloud::Renderer::Camera* camera);

    private:
        void UpdateCamera();

        VehicleEnginePivot m_enginePivot;
        VehiclePod m_pod;

        Cloud::Renderer::Camera* m_camera;
        ClMatrix4 m_cameraTransform;
    };

    class Game
    {
    public:
        Game();

        void Initialise();
        void Update(float timeStep);
        void Render();

    private:
        Vehicle m_vehicle;

    };
}

#endif // RPG_GAME_HEADER