#include "MemoryManager.h"

#include <atomic>
#include <new>

namespace {
    static my_library::MemoryManagerDelegate k_no_op_memory_delegate;

    my_library::MemoryManagerDelegate * gMemoryDelegate = &k_no_op_memory_delegate;
    std::atomic_bool gSelfAllocation{false};

    struct ScopedSetter {
        ScopedSetter() {
            gSelfAllocation.store(true);
        }

        ~ScopedSetter() {
            gSelfAllocation.store(false);
        }
    };
} // namespace

namespace my_library {

    void RecordMemoryManagerDelegate::notifyAllocation(void * ptr, size_t size) noexcept {
        if (gSelfAllocation.load()) {
            return;
        }
        ScopedSetter setter;
        allAllocations_.emplace_back(ptr, size);
        currentAllocations_.emplace_back(ptr, size);
    }

    void RecordMemoryManagerDelegate::notifyDeallocation(void * ptr) noexcept {
        if (gSelfAllocation.load()) {
            return;
        }
        ScopedSetter setter;
        currentAllocations_.erase(
            std::remove_if(
                currentAllocations_.begin(), currentAllocations_.end(),
                [ptr](const std::pair<void *, size_t> & p) {
                    return p.first == ptr;
                }),
            currentAllocations_.end());
    }

    void MemoryManager::setDelegate(MemoryManagerDelegate * delegate) {
        if (delegate == nullptr) {
            gMemoryDelegate = &k_no_op_memory_delegate;
        } else {
            gMemoryDelegate = delegate;
        }
    }
} // namespace my_library

// Override global new operator
void * operator new(std::size_t size) {
    auto ptr = std::malloc(size);
    gMemoryDelegate->notifyAllocation(ptr, size);
    return ptr;
}

// Override global delete operator
void operator delete(void * ptr) noexcept {
    gMemoryDelegate->notifyDeallocation(ptr);
    std::free(ptr);
}