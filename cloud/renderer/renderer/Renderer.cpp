#include "stdafx.h"
#include "Renderer.h"

#include "RenderCore.h"
#include "Settings.h"
#include "Scene.h"
#include "LightCollection.h"
#include "Material.h"

CLbool Cloud::Renderer::Renderer::s_showDebugMenu = false;
Cloud::Renderer::ResourceContainer* Cloud::Renderer::Renderer::s_resourceContainer = nullptr;

Cloud::Renderer::Renderer::Renderer()
{
    s_resourceContainer = &m_resourceContainer;
}

Cloud::Renderer::Renderer::~Renderer()
{
}

static CLint TestFunction(CLfloat val1, CLint val2)
{
    CLint val = static_cast<CLint>(val1 + val2);
    return val;
}

static CLint TestFunction2(CLfloat val1)
{
    CLint val = static_cast<CLint>(val1 + val1);
    return val;
}

static CLfloat TestFunction3()
{
    return 3.14f;
}

CLbool Cloud::Renderer::Renderer::Initialise()
{
    m_modelRenderer.Initialise();
    m_debugRenderer.Initialise();
    m_imguiRenderer.Initialise();

    m_resourceContainer.AddResource(std::make_shared<Material>(ResourceId("metal0")));

    //m_csSorter.Init();
    //m_csTest.Initialise();
    //m_particleManager.Initialise();

    m_lights = std::make_unique<LightCollection>();
    m_lights->Initialise();

    m_forwardScene = std::make_unique<Scene>(m_modelRenderer, m_lights.get());

    CLfloat positionOffset = 5.0f;
    for (auto&& instance : m_instances)
    {
        instance = std::make_unique<ModelInstance>();
        instance->GetTransformMutable() *= ClMatrix4::Translation(positionOffset, 0.0f, 0.0f);
        positionOffset += 1.5f;
        m_forwardScene->AddModelInstance(*instance);
    }

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

    m_luaState = std::make_unique<LuaStateEx>();

    m_luaState->RegisterFunction<CLint, CLfloat, CLint>("TestFunction", TestFunction);
    m_luaState->RegisterFunction<CLint, CLfloat>("TestFunction2", TestFunction2);
    m_luaState->RegisterFunction<CLfloat>("TestFunction3", TestFunction3);

    m_luaState->RegisterFunction<void, GfxTexture*>("GfxClearColour", [](GfxTexture* /*texture*/)
    {
#ifdef USE_DIRECTX12
#else
        if (texture && texture->GetRtv())
        {
            RenderCore::Instance().GetRenderingDevice().ClearColour(*texture);
        }
#endif
    });

    m_luaState->RegisterFunction<void, GfxTexture*>("GfxClearDepth", [](GfxTexture* /*texture*/)
    {
#ifdef USE_DIRECTX12
#else
        if (texture && texture->GetDsv())
        {
            RenderCore::Instance().GetRenderingDevice().ClearDepth(*texture);
        }
#endif
    });

    m_luaState->RegisterFunction<void, GfxTexture*, GfxTexture*>("GfxSetRenderTarget", [](GfxTexture* /*renderTarget*/, GfxTexture* /*depth*/)
    {
#ifdef USE_DIRECTX12
#else
        if ((renderTarget && !renderTarget->GetRtv()) ||
            (depth && !depth->GetDsv()))
        {
            return;
        }
        
        RenderCore::Instance().GetRenderingDevice().SetRenderTarget(renderTarget, depth);
#endif
    });

    m_luaState->RegisterFunction<GfxTexture*, const CLchar*>("GetResource", [](const CLchar* resourceId)
    {
        auto& renderCore = RenderCore::Instance();
        if (strcmp(resourceId, "backbuffer") == 0)
        {
            auto backbuffer = renderCore.GetBackbuffer();
            return backbuffer;
        }
        else if (strcmp(resourceId, "depth") == 0)
        {
            auto depth = renderCore.GetDepthStencil();
            return depth;
        }

        return static_cast<GfxTexture*>(nullptr);
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

    m_imguiRenderer.NewFrame();

    if (s_showDebugMenu)
    {
        PopulateDebugMenu();
    }
    
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

    m_lights->Update();

    //m_csSorter.Update();

   // m_csTest.Update();
    //m_particleManager.Update(static_cast<CLfloat>(timeStep));
    //m_particleManager.Fill();
}

void Cloud::Renderer::Renderer::Render()
{
    m_luaState->DoFile("data/scripts/renderer/render.lua");
    
   // auto test = m_luaState->Call<CLfloat, CLint>("testest", "lol", "nope", 123, 142.43f);
   // CL_UNUSED(test);
    m_luaState->Call("render");
    

    auto& renderCore = RenderCore::Instance();

    // Sort items
    //m_csSorter.Dispatch();

    auto& perSceneConstBuffer = renderCore.GetPerSceneConstData();
    perSceneConstBuffer.view = m_camera.GetView();
    perSceneConstBuffer.projection = m_camera.GetProjection();
    perSceneConstBuffer.viewProj = m_camera.GetProjection() * m_camera.GetView();
    renderCore.GpuUpdatePerSceneConstBuffer();

    // render gbuffer
    // render deferred lighting
    
    auto&& forwardScene = *m_forwardScene;
    RenderCore::Instance().QueueRecordCommandListJob(
        [&forwardScene](ID3D12GraphicsCommandList* cl)
    {
        forwardScene.RecordCommandList(cl);
    });

    //m_spriteManager.Render();
    //m_particleManager.Render();

    // Render post


    // Render debug items
    auto& debugRenderer = m_debugRenderer;
    RenderCore::Instance().QueueRecordCommandListJob(
        [&debugRenderer](ID3D12GraphicsCommandList* cl)
    {
        debugRenderer.RecordCommandList(cl);
    });

    auto&& imguiRenderer = m_imguiRenderer;
    RenderCore::Instance().QueueRecordCommandListJob(
        [&imguiRenderer](ID3D12GraphicsCommandList* cl)
    {
        imguiRenderer.Render(cl);
    });

    

    RenderCore::Instance().RecordCommandLists();
    RenderCore::Instance().Present();
}

void Cloud::Renderer::Renderer::PopulateDebugMenu()
{
    ImGui::ShowTestWindow();

    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiSetCond_FirstUseEver);
    static CLbool s_modelWindowOpen = true;

    ImGuiWindowFlags window_flags = 0;
    if (ImGui::Begin("Renderer", &s_modelWindowOpen, window_flags))
    {
        if (ImGui::CollapsingHeader("Scene"))
        {
            m_forwardScene->PopulateDebugMenu();
        }

        if (ImGui::CollapsingHeader("Lighting"))
        {
            m_lights->PopulateDebugMenu();
        }
    }    

    ImGui::End();
}
