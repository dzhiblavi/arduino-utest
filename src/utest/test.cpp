#define WEAK __attribute__((weak))

WEAK void globalSetUp() {}
WEAK void globalTearDown() {}
extern "C" WEAK void setUp() {}
extern "C" WEAK void tearDown() {}
