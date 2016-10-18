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
            CLbool       m_connected;
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

        CLbool Initialise(const Settings& settings);
        void Shutdown();

        void Update();

        CLbool GetKeyUp(Key key);
        CLbool GetKeyDown(Key key);
        CLbool GetKeyPressed(Key key);
        CLbool GetKeyReleased(Key key);

        CLbool GetMouseUp(CLuchar key);
        CLbool GetMouseDown(CLuchar key);
        CLbool GetMousePressed(CLuchar key);
        CLbool GetMouseReleased(CLuchar key);

        ClFloat2 GetPadLeftStick(CLuint controllerId);
        ClFloat2 GetPadRightStick(CLuint controllerId);
        CLfloat  GetPadLeftTrigger(CLuint controllerId);
        CLfloat  GetPadRightTrigger(CLuint controllerId);
        CLbool   GetPadButtonUp(CLuint controllerId, PadButton button);
        CLbool   GetPadButtonDown(CLuint controllerId, PadButton button);
        CLbool   GetPadButtonPressed(CLuint controllerId, PadButton button);
        CLbool   GetPadButtonReleased(CLuint controllerId, PadButton button);

    private:
        void UpdateKeyboard();
        void UpdateMouse();
        void UpdateControllers();

        IDirectInput8* m_directInput;
        IDirectInputDevice8* m_directInputKeyboard;
        IDirectInputDevice8* m_directInputMouse;

        Utils::StaticArray<CLbyte, 256> m_currentKeystate;
        Utils::StaticArray<CLbyte, 256> m_previousKeyState;

        DIMOUSESTATE2 m_currectMouseState;
        DIMOUSESTATE2 m_previousMouseState;
        static const CLint c_mouseKeyCount = 8; // Matching up with the amount of mouse buttons in DIMOUSESTATE2.

        static const CLuint c_maxControllers = 4;
        Utils::StaticArray<ControllerState, c_maxControllers> m_controllers;
        CLfloat m_controllerInputDeadzone;

        Settings m_settings;
    };
}
}

#endif // CLOUD_INPUT_INPUT_MANAGER_HEADER