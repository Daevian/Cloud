#ifndef CLOUD_INPUT_INPUT_MANAGER_HEADER
#define CLOUD_INPUT_INPUT_MANAGER_HEADER

#define DIRECTINPUT_VERSION 0x0800

#include "InputKeys.h"
#include "../utilities/DefinesTypes.h"
#include "../utilities/StaticArray.h"
#include <dinput.h>

namespace Cloud
{
    namespace Input
    {
        class InputManager
        {
        public:
            struct Settings
            {
                HINSTANCE m_hInstance;
                HWND m_hWnd;
            };

            InputManager();

            CLbool Initialise(const Settings& settings);
            void Shutdown();

            void Update();

            CLbool GetKeyUp(Keyboard::Key key);
            CLbool GetKeyDown(Keyboard::Key key);
            CLbool GetKeyPressed(Keyboard::Key key);
            CLbool GetKeyReleased(Keyboard::Key key);

            CLbool GetMouseUp(CLuchar key);
            CLbool GetMouseDown(CLuchar key);
            CLbool GetMousePressed(CLuchar key);
            CLbool GetMouseReleased(CLuchar key);

        private:
            void UpdateKeyboard();
            void UpdateMouse();

            IDirectInput8* m_directInput;
            IDirectInputDevice8* m_directInputKeyboard;
            IDirectInputDevice8* m_directInputMouse;

            Utils::StaticArray<CLbyte, 256> m_currentKeystate;
            Utils::StaticArray<CLbyte, 256> m_previousKeyState;

            DIMOUSESTATE2 m_currectMouseState;
            DIMOUSESTATE2 m_previousMouseState;
            static const CLint c_mouseKeyCount = 8; // Matching up with the amount of mouse buttons in DIMOUSESTATE2.

            Settings m_settings;
        };
    }
}

#endif // CLOUD_INPUT_INPUT_MANAGER_HEADER