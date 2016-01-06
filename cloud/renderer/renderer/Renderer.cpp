#include "stdafx.h"
#include "Renderer.h"

#include "RenderCore.h"
#include "Settings.h"

Cloud::Renderer::Renderer::Renderer()
{
}

Cloud::Renderer::Renderer::~Renderer()
{
}

CLbool Cloud::Renderer::Renderer::Initialise()
{
    m_debugRenderer.Initialise();

    //m_csSorter.Init();
    //m_csTest.Initialise();
    //m_particleManager.Initialise();


    const CLfloat width  = (CLfloat)Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asDouble();
    const CLfloat height = (CLfloat)Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asDouble();

    m_camera.SetCameraTransform(ClMatrix4::Identity());
    m_camera.SetPerspective(Math::ToRadians(45.0f), width / height, 0.1f, 1000.f);

    
    m_camera.SetTranslation(ClFloat4(0.0f, 2.0f, -10.0f, 1.0f));
    ClMatrix4 cameraRotation = ClMatrix4::Rotation(0.2f, 0.0f, 0.0f);
    m_camera.SetRotation(cameraRotation);

    for (auto& rot : m_randomRotations)
    {
        rot = ClFloat3(ClRandFloat(), ClRandFloat(), ClRandFloat());
    }

    for (auto& scale : m_randomScales)
    {
        scale = ClRandFloat() * 0.5f + 0.5f;
    }

    m_luaState = Lua::NewStateAndSetup();

    Lua::Register(m_luaState.get(), "GfxClearColour", [](lua_State* state)->int
    {
        GfxTexture* texture = Lua::ToUserData<GfxTexture>(state, 1);
        if (texture && texture->GetRtv())
        {
            RenderCore::Instance().GetRenderingDevice().ClearColour(*texture);
        }

        return 0;
    });

    Lua::Register(m_luaState.get(), "GfxClearDepth", [](lua_State* state)->int
    {
        GfxTexture* texture = Lua::ToUserData<GfxTexture>(state, 1);
        if (texture && texture->GetDsv())
        {
            RenderCore::Instance().GetRenderingDevice().ClearDepth(*texture);
        }
        
        return 0;
    });

    Lua::Register(m_luaState.get(), "GfxSetRenderTarget", [](lua_State* state)->int
    {
        GfxTexture* renderTarget = Lua::ToUserData<GfxTexture>(state, 1);
        GfxTexture* depth        = Lua::ToUserData<GfxTexture>(state, 2);
        if ((renderTarget && !renderTarget->GetRtv()) ||
            (depth && !depth->GetDsv()))
        {
            return 0;
        }
        
        RenderCore::Instance().GetRenderingDevice().SetRenderTarget(renderTarget, depth);

        return 0;
    });

    Lua::Register(m_luaState.get(), "GetResource", [](lua_State* state)->int
    {
        const char* resourceId = lua_tostring(state, 1);

        auto& renderCore = RenderCore::Instance();
        if (strcmp(resourceId, "backbuffer") == 0)
        {
            auto backbuffer = renderCore.GetBackbuffer();
            Lua::PushLightUserData(state, backbuffer);
        }
        else if (strcmp(resourceId, "depth") == 0)
        {
            auto depth = renderCore.GetDepthStencil();
            Lua::PushLightUserData(state, depth);
        }

        return 1;
    });
    

    return true;
}

void Cloud::Renderer::Renderer::Shutdown()
{
    m_luaState = nullptr;
    //m_csSorter.Uninit();
   // m_csTest.Uninitialise();
   // m_particleManager.Uninitialise();
    m_spriteManager.Unload();
    m_debugRenderer.Uninitialise();
}

void Cloud::Renderer::Renderer::Update(CLdouble totalTime, CLdouble timeStep)
{
    CL_UNUSED(totalTime);
    CL_UNUSED(timeStep);
    
    static CLfloat rotationX = 0.0f;
    rotationX += 1.0f * (CLfloat)timeStep;

    const CLfloat c_distance = 100.0f;
    const CLint c_boxesPerRow = 25;
    const CLfloat c_width = c_distance * c_boxesPerRow;

    CLfloat x = -c_width * 0.5f;
    CLfloat y = -5.0f;
    CLfloat z = -3.0f;
    for (int i = 0; i < c_boxes; ++i)
    {
        ClFloat3 position = ClFloat3(x, y, z);
        ClFloat3 rotation = m_randomRotations[i] + ClFloat3(rotationX, 0.0f, 0.0f);;
        ClFloat3 scale    = ClFloat3(m_randomScales[i], m_randomScales[i], m_randomScales[i]);

        x += c_distance;
        if (x > c_width * 0.5f)
        {
            x = -c_width * 0.5f;
            z += c_distance;
        }
        
        m_debugRenderer.AddBox(position, rotation, scale, FLOAT4_WHITE(1.0f));
    }

    m_camera.UpdateView();
    //m_csSorter.Update();

   // m_csTest.Update();
    //m_particleManager.Update(static_cast<CLfloat>(timeStep));
    //m_particleManager.Fill();
}

void Cloud::Renderer::Renderer::Render()
{
    Lua::DoFile(m_luaState.get(), "data/scripts/renderer/render.lua");

    /*
    //result = lua_getglobal(m_luaState, "Main");
    lua_pushvalue(m_luaState, -1);
    test = lua_gettop(m_luaState);
    result = lua_pcall(m_luaState, 0, 0, 0);
    //result = luaL_loadfile(m_luaState, "data/scripts/renderer/render.lua");
    test = lua_gettop(m_luaState);
    lua_pushvalue(m_luaState, -1);
    test = lua_gettop(m_luaState);
    result = lua_pcall(m_luaState, 0, 0, 0);
   //int stackSize = lua_gettop(m_luaState);
   //lua_pop(m_luaState, stackSize);
   */


    auto& renderCore = RenderCore::Instance();
    
    // Sort items
    //m_csSorter.Dispatch();

    // Render all items

    auto& perSceneConstBuffer = renderCore.GetPerSceneConstData();
    perSceneConstBuffer.view = m_camera.GetView();
    perSceneConstBuffer.projection = m_camera.GetProjection();
    renderCore.GpuUpdatePerSceneConstBuffer();

    //m_spriteManager.Render();
    //m_particleManager.Render();

    // Render post


    // Render debug items
    m_debugRenderer.Render();

    RenderCore::Instance().Present();
}
