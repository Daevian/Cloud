#ifndef CLOUD_INPUT_INPUT_KEYS_HEADER
#define CLOUD_INPUT_INPUT_KEYS_HEADER

#include <dinput.h>
#include <Xinput.h>

namespace Cloud
{
namespace Input
{
    enum class Key
    {
        A        = DIK_A,
        D        = DIK_D,
        S        = DIK_S,
        W        = DIK_W,

        Down     = DIK_DOWN,
        Escape   = DIK_ESCAPE,
        Left     = DIK_LEFT,
        Return   = DIK_RETURN,
        Right    = DIK_RIGHT,
        Space    = DIK_SPACE,
        Up       = DIK_UP,
        LeftCtrl = DIK_LCONTROL,

        Grave    = DIK_GRAVE,
    };

    enum class MouseButton
    {
    };

    enum class PadButton
    {
        DpadUp        = XINPUT_GAMEPAD_DPAD_UP,
        DpadDown      = XINPUT_GAMEPAD_DPAD_DOWN,
        DpadLeft      = XINPUT_GAMEPAD_DPAD_LEFT,
        DpadRight     = XINPUT_GAMEPAD_DPAD_RIGHT,
        Start         = XINPUT_GAMEPAD_START,
        Back          = XINPUT_GAMEPAD_BACK,
        LeftThumb     = XINPUT_GAMEPAD_LEFT_THUMB,
        RightThumb    = XINPUT_GAMEPAD_RIGHT_THUMB,
        LeftShoulder  = XINPUT_GAMEPAD_LEFT_SHOULDER,
        RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,
        A             = XINPUT_GAMEPAD_A,
        B             = XINPUT_GAMEPAD_B,
        X             = XINPUT_GAMEPAD_X,
        Y             = XINPUT_GAMEPAD_Y,
    };
}
}

#endif // CLOUD_INPUT_INPUT_KEYS_HEADER