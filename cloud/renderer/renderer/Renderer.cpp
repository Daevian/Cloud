#include "stdafx.h"
#include "Renderer.h"

#include "RenderCore.h"
#include "Settings.h"

#include "lua-5.3.2/LuaWrapper.h"

Cloud::Renderer::Renderer::Renderer()
    : m_luaState(nullptr)
{
}

static int LuaPrint(lua_State* state)
{
    int nargs = lua_gettop(state);
    std::stringstream stream;

    for (int i = 1; i <= nargs; ++i)
    {
        stream << lua_tostring(state, i);
    }

    CL_TRACE_CHANNEL("LUA", stream.str().c_str());

    return 0;
}

CLbool Cloud::Renderer::Renderer::Initialise()
{
    m_debugRenderer.Initialise();

    //m_csSorter.Init();
    //m_csTest.Initialise();
    //m_particleManager.Initialise();

    {
        //GfxCore::Instance().m_renderTargetView;
        GfxTextureDesc desc;
        ClMemZero(&desc, sizeof(desc));
        desc.name = "3dSceneSurface";
        //desc.
       // m_3dSceneSurface
    }

    const CLfloat width  = (CLfloat)Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Width"].asDouble();
    const CLfloat height = (CLfloat)Cloud::Renderer::Settings::Instance().GetRoot()["Resolution"]["Height"].asDouble();

    m_camera.SetCameraTransform(ClMatrix4::Identity());
    m_camera.SetPerspective(Math::ToRadians(45.0f), width / height, 0.1f, 1000.f);

    
    m_camera.SetTranslation(ClFloat4(0.0f, 2.0f, -10.0f, 1.0f));
    ClMatrix4 cameraRotation = ClMatrix4::Rotation(0.2f, 0.0f, 0.0f);
    m_camera.SetRotation(cameraRotation);

    for (int i = 0; i < c_boxes; ++i)
    {
        m_randomRotations[i] = ClFloat3(ClRandFloat(), ClRandFloat(), ClRandFloat());
        m_randomScales[i] = ClRandFloat() * 0.5f + 0.5f;
    }

    int top;
    m_luaState = luaL_newstate();
    top = lua_gettop(m_luaState);
    luaL_openlibs(m_luaState);
    top = lua_gettop(m_luaState);
    

    const luaL_Reg printlib[] = {
        { "print", LuaPrint },
        { nullptr, nullptr }, /* end of array */
    };

    lua_getglobal(m_luaState, "_G");
    luaL_setfuncs(m_luaState, printlib, 0);
    lua_pop(m_luaState, 1);

    int result = luaL_loadfile(m_luaState, "data/scripts/renderer/render.lua");
    top = lua_gettop(m_luaState);
   // int result = luaL_dofile(m_luaState, "data/scripts/renderer/render.lua");
    //lua_load
    if (result != LUA_OK && result == LUA_ERRFILE)
    {
        CL_ASSERT_MSG("couldn't load lua file");
    }

    return true;
}

void Cloud::Renderer::Renderer::Shutdown()
{
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
    int result = 0;
    //int test;
    
    //result = luaL_dofile(m_luaState, "data/scripts/renderer/render.lua");

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
    if (result != LUA_OK && result == LUA_ERRRUN)
    {
        if (lua_isstring(m_luaState, -1))
        {
            const char* err = lua_tostring(m_luaState, -1);
            CL_TRACE(err);
        }
    }

    auto& renderCore = RenderCore::Instance();

    auto backbuffer = renderCore.GetBackbuffer();
    auto depthStencil = renderCore.GetDepthStencil();

    renderCore.GetRenderingDevice().ClearColour(*backbuffer);
    renderCore.GetRenderingDevice().ClearDepth(*depthStencil);

    renderCore.GetRenderingDevice().SetRenderTarget(backbuffer, depthStencil);
    
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
