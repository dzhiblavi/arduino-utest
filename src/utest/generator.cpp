#include "generator.h"

namespace utest {

supp::IntrusiveForwardList<GeneratorBase>& generatorStack() {
    static supp::IntrusiveForwardList<GeneratorBase> stack;
    return stack;
}

// returns true if there are more tests to be done
bool advanceGenerators() {
    auto& gen_stack = generatorStack();

    // drain generator stack
    while (!gen_stack.empty()) {
        if (gen_stack.front()->exhausted()) {
            gen_stack.front()->unlink();
            gen_stack.popFront();
        } else {
            gen_stack.front()->advance();
            break;
        }
    }

    return !gen_stack.empty();
}

}  // namespace utest
