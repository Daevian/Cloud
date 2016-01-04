#ifndef CLOUD_RENDERER_RENDERER_HEADER
#define CLOUD_RENDERER_RENDERER_HEADER

#include "CSTest.h"
#include "SpriteManager.h"
#include "ParticleManager.h"
#include "Camera.h"
#include "DebugRenderer.h"
#include "CsSort.h"


//struct lua_State;

namespace Cloud
{
namespace Renderer
{
    class GfxTexture;

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        CLbool Initialise();
        void Shutdown();

        void Update(CLdouble totalTime, CLdouble timeStep);
        void Render();

        SpriteManager& GetSpriteManager()       { return m_spriteManager; }
        ParticleManager& GetParticleManager()   { return m_particleManager; }

        Camera& GetCamera()                     { return m_camera; }

        DebugRenderer& GetDebugRendererForRenderer()       { return m_debugRenderer; }
        DebugRenderer& GetDebugRendererForGame()           { return m_debugRenderer; }
    private:
        CSTest m_csTest;
        DebugRenderer m_debugRenderer;
        SpriteManager m_spriteManager;
        ParticleManager m_particleManager;
        CsSorter m_csSorter;
        Camera m_camera;

        static const CLuint c_boxes = 800;
        std::array<ClFloat3, c_boxes> m_randomRotations;
        std::array<CLfloat, c_boxes> m_randomScales;

        Lua::StateUniquePtr m_luaState;
    };
}
}

#endif // CLOUD_RENDERER_RENDERER_HEADER