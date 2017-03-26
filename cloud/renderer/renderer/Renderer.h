#ifndef CLOUD_RENDERER_RENDERER_HEADER
#define CLOUD_RENDERER_RENDERER_HEADER

#include "CSTest.h"
#include "SpriteManager.h"
#include "ParticleManager.h"
#include "Camera.h"
#include "DebugRenderer.h"
#include "CsSort.h"
#include "ModelRenderer.h"
#include "ImguiRenderer.h"
#include "ResourceContainer.h"


//struct lua_State;

namespace Cloud
{
namespace Renderer
{
    class GfxTexture;
    class Scene;
    class LightCollection;

    class Renderer
    {
    public:
        static CLbool s_showDebugMenu;
        Renderer();
        ~Renderer();

        CLbool Initialise();
        void Shutdown();

        void Update(CLdouble totalTime, CLdouble timeStep);
        void Render();

        SpriteManager& GetSpriteManager()       { return m_spriteManager; }
        ParticleManager& GetParticleManager()   { return m_particleManager; }

        Camera& GetCamera()                     { return m_camera; }

        DebugRenderer& GetDebugRendererForRenderer()        { return m_debugRenderer; }
        DebugRenderer& GetDebugRendererForGame()            { return m_debugRenderer; }

        static ResourceContainer& GetResourceContainer()    { return *s_resourceContainer; }
        
    private:
        void PopulateDebugMenu();

        ResourceContainer m_resourceContainer;
        CSTest m_csTest;
        DebugRenderer m_debugRenderer;
        SpriteManager m_spriteManager;
        ParticleManager m_particleManager;
        ModelRenderer m_modelRenderer;
        ImguiRenderer m_imguiRenderer;
        CsSorter m_csSorter;
        Camera m_camera;

        static const CLuint c_boxes = 800;
        std::array<ClFloat3, c_boxes> m_randomRotations;
        std::array<CLfloat, c_boxes> m_randomScales;

        std::unique_ptr<LuaStateEx> m_luaState;

        std::array<std::unique_ptr<ModelInstance>, 10> m_instances;
        std::unique_ptr<Scene> m_forwardScene;
        std::unique_ptr<Scene> m_imguiScene;
        std::unique_ptr<LightCollection> m_lights;

        static ResourceContainer* s_resourceContainer;
    };
}
}

#endif // CLOUD_RENDERER_RENDERER_HEADER