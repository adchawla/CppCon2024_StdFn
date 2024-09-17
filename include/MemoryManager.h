#pragma once
#include <utility>
#include <vector>

namespace my_library {
    struct MemoryManagerDelegate {
        virtual void notifyAllocation(void * ptr, size_t size) noexcept {};
        virtual void notifyDeallocation(void * ptr) noexcept {};
        virtual ~MemoryManagerDelegate() = default;
    };

    struct RecordMemoryManagerDelegate final : MemoryManagerDelegate {
        void notifyAllocation(void * ptr, size_t size) noexcept override;
        void notifyDeallocation(void * ptr) noexcept override;

        [[nodiscard]] const std::vector<std::pair<void *, size_t>> & allAllocations() const {
            return allAllocations_;
        }

        [[nodiscard]] const std::vector<std::pair<void *, size_t>> & currentAllocations() const {
            return currentAllocations_;
        }

    private:
        std::vector<std::pair<void *, size_t>> allAllocations_;
        std::vector<std::pair<void *, size_t>> currentAllocations_;
    };

    struct MemoryManager {
        static void setDelegate(MemoryManagerDelegate * delegate);
    };
} // namespace my_library