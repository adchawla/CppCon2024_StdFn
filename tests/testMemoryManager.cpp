#include "MemoryManager.h"

#include <gtest/gtest.h>

TEST(MemoryManager, TestAllocations) {
    my_library::RecordMemoryManagerDelegate delegate;
    my_library::MemoryManager::setDelegate(&delegate);

    const char * ptr1 = new char[10];
    void * ptr2 = new char[20];
    delete[] ptr1;

    const auto & allAllocations = delegate.allAllocations();
    EXPECT_EQ(2, allAllocations.size());
    EXPECT_EQ(1, delegate.currentAllocations().size());
    my_library::MemoryManager::setDelegate(nullptr);
}