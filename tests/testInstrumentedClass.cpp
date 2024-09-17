#include "InstrumentedClass.h"

#include <gtest/gtest.h>

TEST(InstrumentedClass, TestConstruction) {
    my_library::InstrumentedClass ic{"a"};
    EXPECT_EQ("a", ic.id());
}

TEST(InstrumentedClass, TestCopyConstruction) {
    my_library::InstrumentedClass ic{"a"};
    my_library::InstrumentedClass ic2{ic};
    EXPECT_EQ("C(a)", ic2.id());
}

TEST(InstrumentedClass, TestAssignment) {
    my_library::InstrumentedClass ic{"a"};
    my_library::InstrumentedClass ic2{"b"};
    ic2 = ic;
    EXPECT_EQ("c=(a)", ic2.id());
}

TEST(InstrumentedClass, TestMoveConstruction) {
    my_library::InstrumentedClass ic{"a"};
    my_library::InstrumentedClass ic2{std::move(ic)};
    EXPECT_EQ("M(a)", ic2.id());
}

TEST(InstrumentedClass, TestMoveAssignment) {
    my_library::InstrumentedClass ic{"a"};
    my_library::InstrumentedClass ic2{"b"};
    ic2 = std::move(ic);
    EXPECT_EQ("m=(a)", ic2.id());
}