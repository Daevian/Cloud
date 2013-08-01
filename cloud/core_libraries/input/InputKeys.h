#ifndef CLOUD_INPUT_INPUT_KEYS_HEADER
#define CLOUD_INPUT_INPUT_KEYS_HEADER

#include <dinput.h>

namespace Cloud
{
    namespace Input
    {
        struct Keyboard
        {
            enum Key
            {
                Down    = DIK_DOWN,
                Escape  = DIK_ESCAPE,
                Left    = DIK_LEFT,
                Return  = DIK_RETURN,
                Right   = DIK_RIGHT,
                Space   = DIK_SPACE,
                Up      = DIK_UP,
            };
        };

        struct Mouse
        {
            enum Button
            {

            };
        };
    }
}

#endif // CLOUD_INPUT_INPUT_KEYS_HEADER