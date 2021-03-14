#ifndef CLOUD_INPUT_INPUT_MANAGER_HEADER
#define CLOUD_INPUT_INPUT_MANAGER_HEADER

#define DIRECTINPUT_VERSION 0x0800

#include "InputKeys.h"
#include "../utilities/DefinesTypes.h"
#include "../utilities/StaticArray.h"
#include "../utilities/Float2.h"
#include <Xinput.h>
#include <dinput.h>

namespace Cloud
{
namespace Input
{
    class InputManager
    {
        struct ControllerState
        {
            XINPUT_STATE m_currentState;
            XINPUT_STATE m_previousState;
            bool       m_connected;
        };

    public:
        struct Settings
        {
            Settings()
                : m_hInstance(nullptr)
                , m_hWnd(nullptr)
            {}

            HINSTANCE m_hInstance;
            HWND m_hWnd;
        };

        InputManager();

        bool Initialise(const Settings& settings);
        void Shutdown();

        void Update();

        bool GetKeyUp(Key key);
        bool GetKeyDown(Key key);
        bool GetKeyPressed(Key key);
        bool GetKeyReleased(Key key);

        bool GetMouseUp(t_uchar key);
        bool GetMouseDown(t_uchar key);
        bool GetMousePressed(t_uchar key);
        bool GetMouseReleased(t_uchar key);

        float2 GetPadLeftStick(uint controllerId);
        float2 GetPadRightStick(uint controllerId);
        float  GetPadLeftTrigger(uint controllerId);
        float  GetPadRightTrigger(uint controllerId);
        bool   GetPadButtonUp(uint controllerId, PadButton button);
        bool   GetPadButtonDown(uint controllerId, PadButton button);
        bool   GetPadButtonPressed(uint controllerId, PadButton button);
        bool   GetPadButtonReleased(uint controllerId, PadButton button);

    private:
        void UpdateKeyboard();
        void UpdateMouse();
        void UpdateControllers();

        IDirectInput8* m_directInput;
        IDirectInputDevice8* m_directInputKeyboard;
        IDirectInputDevice8* m_directInputMouse;

        Utils::StaticArray<byte, 256> m_currentKeystate;
        Utils::StaticArray<byte, 256> m_previousKeyState;

        DIMOUSESTATE2 m_currectMouseState;
        DIMOUSESTATE2 m_previousMouseState;
        static const int c_mouseKeyCount = 8; // Matching up with the amount of mouse buttons in DIMOUSESTATE2.

        static const uint c_maxControllers = 4;
        Utils::StaticArray<ControllerState, c_maxControllers> m_controllers;
        float m_controllerInputDeadzone;

        Settings m_settings;
    };
}
}

#endif // CLOUD_INPUT_INPUT_MANAGER_HEADER