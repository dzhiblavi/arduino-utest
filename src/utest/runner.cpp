#include "runner.h"

namespace utest {

supp::IntrusiveForwardList<TestRunnerBase>& testRunners() {
    static supp::IntrusiveForwardList<TestRunnerBase> runners;
    return runners;
}

}  // namespace utest
