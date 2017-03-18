#ifndef CLOUD_RENDERER_DIRECTX_HELPERS_HEADER
#define CLOUD_RENDERER_DIRECTX_HELPERS_HEADER

namespace Cloud
{
    using Microsoft::WRL::ComPtr;

    namespace Renderer
    {
        namespace Dx
        {
            struct Deleter
            {
                template <class DX_OBJECT>
                void operator()(DX_OBJECT* ptr) const
                {
                    if (ptr)
                    {
                        ptr->Release();
                    }
                }
            };

            template <class DX_OBJECT>
            using UniquePtr = std::unique_ptr<DX_OBJECT, Deleter>;

            template <class DX_OBJECT>
            UniquePtr<DX_OBJECT> MakeUnique(DX_OBJECT* obj)
            {
                return Dx::UniquePtr<DX_OBJECT>(obj);
            }
        };

    }
}

#endif // CLOUD_RENDERER_DIRECTX_HELPERS_HEADER