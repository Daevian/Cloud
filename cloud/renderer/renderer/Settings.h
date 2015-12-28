#ifndef CLOUD_RENDERER_SETTINGS_HEADER
#define CLOUD_RENDERER_SETTINGS_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class Settings
        {
        public:
            static CLbool Create();
            static void Destroy();

            static Settings& Instance() { return *s_instance; }

            Json::Value& GetRoot() { return m_root; }

        private:
            Settings();
            ~Settings();

            CLbool Initialise();
            void Shutdown();

            static Settings* s_instance;

            Json::Value m_root;
        };
    }
}

#endif // CLOUD_RENDERER_SETTINGS_HEADER