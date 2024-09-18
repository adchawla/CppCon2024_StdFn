#include "InstrumentedClass.h"
#include "ConditionalStream.h"

#include <gtest/gtest.h>
#include <future>

using namespace my_library;
using namespace std;

void fn(InstrumentedClass byValue, InstrumentedClass & byRef, const InstrumentedClass & byCRef) {
    OSTREAM << byValue.id() << ", " << byRef.id() << ", " << byCRef.id() << endl;
}

void fn2(InstrumentedClass byValue, const InstrumentedClass & byCRef) {
    OSTREAM << byValue.id() << ", " << byCRef.id() << endl;
}

TEST(CompareConstructs, testLambda) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    [byValue = move(byValue), byRef= std::move(byRef), byCRef = move(byCRef)] () mutable  {
        fn(move(byValue), byRef, byCRef);
    }();
}

TEST(CompareConstructs, testBind) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

    // bind can't directly work with function having reference parameters
    bind(fn2, move(byValue), move(byCRef))();
}

TEST(CompareConstructs, testAsync) {
    InstrumentedClass byValue("byValue");
    InstrumentedClass byRef("byRef");
    InstrumentedClass byCRef("byCRef");

#if WIN32
    async(launch::deferred, fn, move(byValue), std::move(byRef), std::move(byCRef)).get()
#else
    async(launch::deferred, fn2, move(byValue), move(byCRef)).get();
#endif
}
