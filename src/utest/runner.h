#pragma once

#include "generator.h"
#include "section.h"

#include <unity.h>

namespace utest {

struct TestRunnerBase : supp::IntrusiveForwardListNode<> {
    virtual ~TestRunnerBase() = default;
    virtual void run() = 0;
};

supp::IntrusiveForwardList<TestRunnerBase>& testRunners();

template <void (*TestFn)()>
struct TestRunner : TestRunnerBase {
    TestRunner(const char* name, unsigned long line) : name{name}, line{line} {
        testRunners().pushBack(this);
    }

    void run() override {
        RootSectionContext section_ctx;

        for (;;) {
            for (;;) {
                section_ctx.reset();

                UnityDefaultTestRun(TestFn, name, line);

                if (!advanceGenerators()) {
                    break;
                }
            }

            if (!section_ctx.advance()) {
                break;
            }
        }
    }

    const char* name{};
    unsigned long line{};
};

}  // namespace utest

#define TEST(name)                                             \
    void name();                                               \
    ::utest::TestRunner<&name> runner_##name{#name, __LINE__}; \
    void name()

#define TEST_F(fixture, name)                                  \
    struct t_##name : fixture {                                \
        void run();                                            \
    };                                                         \
    void name() {                                              \
        t_##name t;                                            \
        t.run();                                               \
    }                                                          \
    ::utest::TestRunner<&name> runner_##name{#name, __LINE__}; \
    void t_##name::run()

#define UTEST_RUN_ALL_TESTS                           \
    [] {                                              \
        UNITY_BEGIN();                                \
        while (!::utest::testRunners().empty()) {     \
            ::utest::testRunners().popFront()->run(); \
        }                                             \
        return UNITY_END();                           \
    }
