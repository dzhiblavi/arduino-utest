#pragma once

#include <supp/IntrusiveForwardList.h>

namespace utest {

struct Active;
struct Stack;

struct SectionContext : supp::IntrusiveForwardListNode<Active>,
                        supp::IntrusiveForwardListNode<Stack> {
 public:
    SectionContext() = default;

    bool shouldRun() {
        return requests_++ == next_req_;
    }

    void childDone() {
        child_done_ = true;
    }

    void advance() {
        if (child_done_) {
            ++next_req_;
        }

        reset();
    }

    void reset() {
        child_done_ = false;
        requests_ = 0;
    }

    bool hasMore() const {
        return (!child_done_ && requests_ > 0) || requests_ > next_req_ + 1;
    }

    static supp::IntrusiveForwardList<SectionContext, Stack>& stack() {
        static supp::IntrusiveForwardList<SectionContext, Stack> ctxs;
        return ctxs;
    }

    static supp::IntrusiveForwardList<SectionContext, Active>& active() {
        static supp::IntrusiveForwardList<SectionContext, Active> ctxs;
        return ctxs;
    }

 private:
    bool child_done_ = false;
    int requests_ = 0;
    int next_req_ = 0;
};

struct RootSectionContext : SectionContext {
    RootSectionContext() {
        stack().pushFront(this);
    }

    ~RootSectionContext() {
        stack().popFront();
    }

    void reset() {
        auto& a = active();

        while (!a.empty()) {
            a.popFront()->reset();
        }

        a.pushFront(this);
    }

    bool advance() {
        auto& a = active();

        while (!a.empty()) {
            auto* front = a.front();
            a.popFront();

            if (front == this && !front->hasMore()) {
                return false;
            }

            if (front->hasMore()) {
                front->advance();
            }
        }

        active().pushFront(this);
        return true;
    }
};

struct Section {
 public:
    Section(SectionContext* child_ctx, [[maybe_unused]] const char* name)
        : ctx_{SectionContext::stack().front()}
        , child_ctx_{child_ctx}
        , should_run_{ctx_->shouldRun()} {
        if (!shouldRun()) {
            return;
        }

        LINFO("SECTION ", name);
        SectionContext::stack().pushFront(child_ctx);
        SectionContext::active().pushFront(child_ctx);
    }

    ~Section() {
        if (!shouldRun()) {
            return;
        }

        SectionContext::stack().popFront();

        if (!child_ctx_->hasMore()) {
            ctx_->childDone();
        }
    }

    bool shouldRun() const {
        return should_run_;
    }

 private:
    SectionContext* ctx_;
    SectionContext* child_ctx_;
    bool should_run_;
};

}  // namespace utest

#define SECTION(name)                                \
    if (auto s__ =                                   \
            [] {                                     \
                static ::utest::SectionContext ctx;  \
                return ::utest::Section{&ctx, name}; \
            }();                                     \
        s__.shouldRun())
