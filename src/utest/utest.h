#pragma once

#include "runner.h"  // IWYU pragma: keep
#include "test.h"    // IWYU pragma: keep

#include <unity.h>

#if defined(ARDUINO)

#define TESTS_MAIN                              \
    logging::Logger global;                     \
    void setup() {                              \
        delay(2000);                            \
        global.setLevel(logging::Level::Trace); \
        logging::setGlobal(&global);            \
        ::globalSetUp();                        \
        auto _ = UTEST_RUN_ALL_TESTS();         \
        ::globalTearDown();                     \
    }                                           \
    void loop() {}

#else

#define TESTS_MAIN                              \
    logging::Logger global;                     \
    int main() {                                \
        global.setLevel(logging::Level::Trace); \
        logging::setGlobal(&global);            \
        ::globalSetUp();                        \
        auto code = UTEST_RUN_ALL_TESTS();      \
        ::globalTearDown();                     \
        return code;                            \
    }

#endif
