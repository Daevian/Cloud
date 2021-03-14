#ifndef RPG_UI_ELEMENT_HEADER
#define RPG_UI_ELEMENT_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class Sprite;
    }
}

namespace RPG
{
    class UIElement
    {
    public:
        UIElement();
        ~UIElement() {};

        bool Initialise(Cloud::Renderer::Sprite* sprite);

        void SetPosition(const Cloud::Math::Float2& position);
        void SetScale(const Cloud::Math::Float2& scale);

    private:
        Cloud::Renderer::Sprite* m_sprite;

    };
}

#endif // RPG_UI_ELEMENT_HEADER