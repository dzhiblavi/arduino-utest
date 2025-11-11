#pragma once

#include <supp/IntrusiveForwardList.h>

#include <array>

namespace utest {

struct GeneratorBase : supp::IntrusiveForwardListNode<> {
    virtual ~GeneratorBase() = default;
    virtual bool exhausted() const = 0;
    virtual void advance() = 0;
    virtual void unlink() = 0;
};

supp::IntrusiveForwardList<GeneratorBase>& generatorStack();

// returns true if there are more iterations in generators
bool advanceGenerators();

template <typename T, typename... Ts>
class Generator : GeneratorBase {
 public:
    Generator(T val, Ts... vals) : arr_{val, vals...} {}

    bool exhausted() const override {
        return index_ + 1 == arr_.size();
    }

    void unlink() override {
        linked_ = false;
        index_ = 0;
    }

    void advance() override {
        ++index_;
    }

    T get() {
        if (!linked_) {
            linked_ = true;
            index_ = 0;
            generatorStack().pushFront(this);
        }

        return arr_[index_];
    }

 private:
    size_t index_ = 0;
    std::array<T, 1 + sizeof...(Ts)> arr_;
    bool linked_ = false;
};

}  // namespace utest

#define GENERATE(...)                               \
    [&]() mutable {                                 \
        static ::utest::Generator gen{__VA_ARGS__}; \
        return gen.get();                           \
    }()
