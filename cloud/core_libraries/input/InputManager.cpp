#include "InputManager.h"

#include "../utilities/DefinesAsserts.h"
#include "../utilities/DefinesMacros.h"
#include "../utilities/IntrinsicFunctions.h"
#include <sstream>

Cloud::Input::InputManager::InputManager()
    : m_directInput(0)
    , m_directInputKeyboard(0)
    , m_directInputMouse(0)
    , m_controllerInputDeadzone(0)
{
    ClMemZero(&m_currentKeystate, sizeof(m_currentKeystate.SizeOf()));
    ClMemZero(&m_previousKeyState, sizeof(m_previousKeyState.SizeOf()));

    ClMemZero(&m_currectMouseState, sizeof(DIMOUSESTATE));
    ClMemZero(&m_previousMouseState, sizeof(DIMOUSESTATE));

    ClMemZero(m_controllers.GetBuffer(), m_controllers.SizeOf());
}

CLbool Cloud::Input::InputManager::Initialise(const Settings& settings)
{
    m_settings = settings;

#pragma warning(suppress: 26490)
    auto result = DirectInput8Create(m_settings.m_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<LPVOID*>(&m_directInput), 0);
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


    m_controllerInputDeadzone = 0.24f * CLfloat(0x7FFF); // Default to 24% of the +/- 32767 range.

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
    UpdateControllers();
}

CLbool Cloud::Input::InputManager::GetKeyUp(Key key)
{
    return (m_currentKeystate[(CLuint)key] & 0x80) == 0;
}

CLbool Cloud::Input::InputManager::GetKeyDown(Key key)
{
    return (m_currentKeystate[(CLuint)key] & 0x80) != 0;
}

CLbool Cloud::Input::InputManager::GetKeyPressed(Key key)
{
    return (m_currentKeystate[(CLuint)key] & 0x80) && !(m_previousKeyState[(CLuint)key] & 0x80);
}

CLbool Cloud::Input::InputManager::GetKeyReleased(Key key)
{
    return !(m_currentKeystate[(CLuint)key] & 0x80) && (m_previousKeyState[(CLuint)key] & 0x80);
}

CLbool Cloud::Input::InputManager::GetMouseUp(CLuchar key)
{
    //auto buttons = std::as_span(m_currectMouseState.rgbButtons);
    //return (buttons[key] & 0x80) == 0;
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

ClFloat2 Cloud::Input::InputManager::GetPadLeftStick(CLuint controllerId)
{
    auto& controller = m_controllers[controllerId];
    if (controller.m_connected)
    {
        const auto& gamepadState = controller.m_currentState.Gamepad;
        ClFloat2 state( (CLfloat)gamepadState.sThumbLX / 32768.0f,
                        (CLfloat)gamepadState.sThumbLY / 32768.0f);
        
        return state;
    }

    return ClFloat2(0.0f, 0.0f);
}

ClFloat2 Cloud::Input::InputManager::GetPadRightStick(CLuint controllerId)
{
    auto& controller = m_controllers[controllerId];
    if (controller.m_connected)
    {
        const auto& gamepadState = controller.m_currentState.Gamepad;
        ClFloat2 state( (CLfloat)gamepadState.sThumbRX / 32768.0f,
                        (CLfloat)gamepadState.sThumbRY / 32768.0f);
        
        return state;
    }

    return ClFloat2(0.0f, 0.0f);
}

CLfloat Cloud::Input::InputManager::GetPadLeftTrigger(CLuint controllerId)
{
    auto& controller = m_controllers[controllerId];
    if (controller.m_connected)
    {
        const auto& gamepadState = controller.m_currentState.Gamepad;        
        return (CLfloat)gamepadState.bLeftTrigger / 255.0f;
    }

    return 0.0f;
}

CLfloat Cloud::Input::InputManager::GetPadRightTrigger(CLuint controllerId)
{
    auto& controller = m_controllers[controllerId];
    if (controller.m_connected)
    {
        const auto& gamepadState = controller.m_currentState.Gamepad;
        return (CLfloat)gamepadState.bRightTrigger / 255.0f;
    }

    return 0.0f;
}

CLbool Cloud::Input::InputManager::GetPadButtonUp(CLuint controllerId, PadButton button)
{
    CLbool state = (m_controllers[controllerId].m_currentState.Gamepad.wButtons & (CLuint)button) != 0;
    return !state;
}

CLbool Cloud::Input::InputManager::GetPadButtonDown(CLuint controllerId, PadButton button)
{
    CLbool state = (m_controllers[controllerId].m_currentState.Gamepad.wButtons & (CLuint)button) != 0;
    return state;
}

CLbool Cloud::Input::InputManager::GetPadButtonPressed(CLuint controllerId, PadButton button)
{
    CLbool currentState  = (m_controllers[controllerId].m_currentState.Gamepad.wButtons  & (CLuint)button) != 0;
    CLbool previousState = (m_controllers[controllerId].m_previousState.Gamepad.wButtons & (CLuint)button) != 0;
    return currentState && !previousState;
}

CLbool Cloud::Input::InputManager::GetPadButtonReleased(CLuint controllerId, PadButton button)
{
    CLbool currentState  = (m_controllers[controllerId].m_currentState.Gamepad.wButtons  & (CLuint)button) != 0;
    CLbool previousState = (m_controllers[controllerId].m_previousState.Gamepad.wButtons & (CLuint)button) != 0;
    return !currentState && previousState;
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

void Cloud::Input::InputManager::UpdateControllers()
{
    DWORD dwResult = NO_ERROR;
    for( DWORD i = 0; i < c_maxControllers; i++ )
    {
        auto& controller = m_controllers[i];
        
        controller.m_previousState = controller.m_currentState;
        dwResult = XInputGetState(i, &controller.m_currentState);

        if( dwResult == ERROR_SUCCESS )
        {
            controller.m_connected = true;

            auto& gamepadState = controller.m_currentState.Gamepad;
            if (gamepadState.sThumbLX < m_controllerInputDeadzone &&
                gamepadState.sThumbLX > -m_controllerInputDeadzone &&
                gamepadState.sThumbLY < m_controllerInputDeadzone &&
                gamepadState.sThumbLY > -m_controllerInputDeadzone)
            {
                gamepadState.sThumbLX = 0;
                gamepadState.sThumbLY = 0;
            }

            if (gamepadState.sThumbRX < m_controllerInputDeadzone &&
                gamepadState.sThumbRX > -m_controllerInputDeadzone &&
                gamepadState.sThumbRY < m_controllerInputDeadzone &&
                gamepadState.sThumbRY > -m_controllerInputDeadzone)
            {
                gamepadState.sThumbRX = 0;
                gamepadState.sThumbRY = 0;
            }
        }
        else
        {
            controller.m_connected = false;
        }
    }
}