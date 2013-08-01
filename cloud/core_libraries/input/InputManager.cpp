#include "InputManager.h"

#include "../utilities/DefinesAsserts.h"
#include "../utilities/DefinesMacros.h"
#include "../utilities/IntrinsicFunctions.h"
#include <sstream>

Cloud::Input::InputManager::InputManager()
    : m_directInput(0)
    , m_directInputKeyboard(0)
    , m_directInputMouse(0)
{
    ClMemZero(&m_currentKeystate, sizeof(m_currentKeystate.SizeOf()));
    ClMemZero(&m_previousKeyState, sizeof(m_previousKeyState.SizeOf()));

    ClMemZero(&m_currectMouseState, sizeof(DIMOUSESTATE));
    ClMemZero(&m_previousMouseState, sizeof(DIMOUSESTATE));
}

CLbool Cloud::Input::InputManager::Initialise(const Settings& settings)
{
    m_settings = settings;

    auto result = DirectInput8Create(m_settings.m_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&m_directInput), 0);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create initialise direct input!");
        Shutdown();
        return false;
    }

    result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_directInputKeyboard, 0);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create initialise direct input keyboard device!");
        Shutdown();
        return false;
    }

    result = m_directInput->CreateDevice(GUID_SysMouse, &m_directInputMouse, 0);
    if (FAILED(result))
    {
        CL_ASSERT_MSG("Couldn't create initialise direct input mouse device!");
        Shutdown();
        return false;
    }

    m_directInputKeyboard->SetCooperativeLevel(m_settings.m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    m_directInputMouse->SetCooperativeLevel(m_settings.m_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    m_directInputKeyboard->SetDataFormat(&c_dfDIKeyboard);
    m_directInputMouse->SetDataFormat(&c_dfDIMouse);

    m_directInputKeyboard->Acquire();
    m_directInputMouse->Acquire();

    return true;
}

void Cloud::Input::InputManager::Shutdown()
{
    if (m_directInputMouse)
    {
        m_directInputMouse->Unacquire();
        m_directInputMouse->Release();
        m_directInputMouse = 0;
    }

    if (m_directInputKeyboard)
    {
        m_directInputKeyboard->Unacquire();
        m_directInputKeyboard->Release();
        m_directInputKeyboard = 0;
    }

    if (m_directInput)
    {
        m_directInput->Release();
        m_directInput = 0;
    }
}

void Cloud::Input::InputManager::Update()
{
    UpdateKeyboard();
    UpdateMouse();
}

CLbool Cloud::Input::InputManager::GetKeyUp(Keyboard::Key key)
{
    return (m_currentKeystate[key] & 0x80) == 0;
}

CLbool Cloud::Input::InputManager::GetKeyDown(Keyboard::Key key)
{
    return (m_currentKeystate[key] & 0x80) != 0;
}

CLbool Cloud::Input::InputManager::GetKeyPressed(Keyboard::Key key)
{
    return (m_currentKeystate[key] & 0x80) && !(m_previousKeyState[key] & 0x80);
}

CLbool Cloud::Input::InputManager::GetKeyReleased(Keyboard::Key key)
{
    return !(m_currentKeystate[key] & 0x80) && (m_previousKeyState[key] & 0x80);
}

CLbool Cloud::Input::InputManager::GetMouseUp(CLuchar key)
{
    return (m_currectMouseState.rgbButtons[key] & 0x80) == 0;
}

CLbool Cloud::Input::InputManager::GetMouseDown(CLuchar key)
{
    return (m_currectMouseState.rgbButtons[key] & 0x80) != 0;
}

CLbool Cloud::Input::InputManager::GetMousePressed(CLuchar key)
{
    return (m_currectMouseState.rgbButtons[key] & 0x80) && !(m_previousMouseState.rgbButtons[key] & 0x80);
}

CLbool Cloud::Input::InputManager::GetMouseReleased(CLuchar key)
{
    return !(m_currectMouseState.rgbButtons[key] & 0x80) && (m_previousMouseState.rgbButtons[key] & 0x80);
}

void Cloud::Input::InputManager::UpdateKeyboard()
{
    CL_ASSERT(m_directInputKeyboard, "Keyboard input device isn't properly initialised!");

    m_previousKeyState = m_currentKeystate;

    if (FAILED(m_directInputKeyboard->Poll()))
    {
        m_directInputKeyboard->Acquire();
        return;
    }

    if (FAILED(m_directInputKeyboard->GetDeviceState(m_currentKeystate.SizeOf(), &m_currentKeystate)))
    {
        return;
    }
}

void Cloud::Input::InputManager::UpdateMouse()
{
    CL_ASSERT(m_directInputMouse, "Mouse input device isn't properly initialised!");

    for(int i = 0; i < c_mouseKeyCount; ++i)
    {
        m_previousMouseState.rgbButtons[i] = m_currectMouseState.rgbButtons[i];
    }

    if (FAILED(m_directInputMouse->Poll()))
    {
        m_directInputMouse->Acquire();
        return;
    }

    if (FAILED(m_directInputMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_currectMouseState)))
    {
        return;
    }
}