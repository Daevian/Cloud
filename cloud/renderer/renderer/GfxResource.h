#ifndef CLOUD_RENDERER_GFX_RESOURCE_HEADER
#define CLOUD_RENDERER_GFX_RESOURCE_HEADER

namespace Cloud
{
    namespace Renderer
    {
        class GfxResource
        {
        public:

        protected:
            GfxResource() {};
            virtual ~GfxResource() {};

            struct Deleter
            {
                void operator()(GfxResource* ptr) const
                {
                    delete ptr;
                }
            };
        };

    }
}

#endif // CLOUD_RENDERER_GFX_RESOURCE_HEADER