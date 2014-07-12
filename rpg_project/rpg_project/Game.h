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

        void SetOffset(const ClFloat4& offset)                      { m_offset = offset; }
        void SetStrafeTarget(const CLfloat strafe)                  { m_strafeTarget = strafe; }
        void SetRotationTarget(const ClFloat2& rotation)            { m_rotationTarget.Set(rotation.x * 0.1f, rotation.y * 0.2f, 0.0f, 0.0f); }
        void SetAcceleration(CLfloat acceleration)                  { m_acceleration = acceleration; }
        void SetBrakeForce(CLfloat brakeForce)                      { m_brakeForce = brakeForce; }

        CLfloat GetAcceleration() const                             { return m_acceleration; }
        CLfloat GetBrakeForce() const                               { return m_brakeForce; }

        void Update(CLfloat timeStep);
        void Reset();

        void Render();

    private:
        ClMatrix4 m_transform;
        ClFloat4 m_offset;
        ClFloat4 m_position;
        ClFloat4 m_rotation;
        ClFloat4 m_rotationTarget;
        CLfloat m_strafe;
        CLfloat m_strafeTarget;
        CLfloat m_acceleration;
        CLfloat m_brakeForce;

        VehicleEnginePivot* m_enginePivot;
    };

    class VehicleEnginePivot
    {
    public:
        VehicleEnginePivot();

        void Initialise();
        void Reset();
        void Update(CLfloat timeStep);
        void Render();

        void SetEngineRotation(EngineSide engine, const ClFloat2& rotation);
        void SetEngineStrafe(EngineSide engine, const CLfloat strafe);
        void SetEngineAcceleration(EngineSide engine, CLfloat acceleration);
        void SetEngineBrakeForce(EngineSide engine, CLfloat brakes);

        void SetSteerAmount(const CLfloat steerAmount)              { m_steerAmount = steerAmount; }
        void SetStrafeAmount(const CLfloat strafeAmount)            { m_strafeAmount = strafeAmount; }
        void SetPosition(const ClFloat4& position)                  { m_position = position; }
        void SetRotationTarget(const ClFloat3& rotation)            { m_rotationTarget.Set(rotation.x * 0.1f, rotation.y * 0.2f, rotation.z * 0.2f, 0.0f); }

        const ClMatrix4& GetTransform() const                       { return m_transform; }
        const ClMatrix4& GetTransformWithoutRotation() const        { return m_transformWithoutRotation; }

    private:
        ClMatrix4 m_transform;
        ClMatrix4 m_transformWithoutRotation;

        ClFloat4 m_position;
        ClFloat4 m_rotation;
        ClFloat4 m_rotationTarget;
        ClFloat4 m_velocity;
        ClFloat4 m_heading;
        CLfloat m_steerAmount;
        CLfloat m_strafeAmount;
        CLfloat m_strafeVelocity;
        CLfloat m_accelerationMultiplier;
        CLfloat m_maxVelocity;
        CLfloat m_brakeDragMultiplier;

        StaticArray<VehicleEngine, (CLuint)EngineSide::Max> m_engines;
    };

    class VehiclePod
    {
    public:
        VehiclePod();

        void SetPosition(const ClFloat4& position)          { m_position = position; }

        void Update(CLfloat timeStep);
        void Render();

    private:
        ClFloat4 m_position;
    };

    class Vehicle
    {
    public:
        Vehicle();

        void Initialise();
        void Reset();
        void Update(CLfloat timeStep);
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
        void Update(CLfloat timeStep);
        void Render();

    private:
        Vehicle m_vehicle;

    };
}

#endif // RPG_GAME_HEADER