#pragma once

namespace Cloud
{
    namespace Renderer
    {
        class ResourceId
        {
        public:
            struct Hash
            {
                std::size_t operator()(const ResourceId& id) const
                {
                    return id.m_hash;
                }
            };

            ResourceId(const t_char* name);

            bool operator==(const ResourceId& other) const
            {
                return m_hash == other.m_hash;
            }

            uint64 GetHash() const            { return m_hash; }
            const t_char* GetDebugName() const  { return m_name.c_str(); }

        private:
            uint64 m_hash = 0;
            std::string m_name;
        };

        class Resource
        {
        public:
            Resource(const ResourceId& id)
                : m_id(id)
            { }
            virtual ~Resource() {};

            const ResourceId& GetId() const { return m_id; }

        private:
            ResourceId m_id;
        };

        class ResourceContainer
        {
        public:
            ResourceContainer();

            template <typename _Resource>
            inline void AddResource(std::shared_ptr<_Resource>&& resource);

            template <typename _Resource>
            std::shared_ptr<_Resource> FindResource(const ResourceId& id);

        private:
            std::unordered_map<ResourceId, std::shared_ptr<Resource>, ResourceId::Hash> m_resourceMap;
        };

        template <typename _Resource>
        void Cloud::Renderer::ResourceContainer::AddResource(std::shared_ptr<_Resource>&& resource)
        {
            m_resourceMap[resource->GetId()] = std::forward<std::shared_ptr<Resource>>(resource);
        }

        template <typename _Resource>
        std::shared_ptr<_Resource> Cloud::Renderer::ResourceContainer::FindResource(const ResourceId& id)
        {
            auto it = m_resourceMap.find(id);
            if (it != m_resourceMap.end())
            {
                return std::static_pointer_cast<_Resource>(it->second);
            }

            CL_ASSERT_MSG("Couldn't find resource!");
            return nullptr;
        }
    }
}