#pragma once
ASSUME_NONNULL_BEGIN

// unit testing
//   W_TEST_BUILD is defined for the "test" target product (but not for "debug".)
//   R_UNIT_TEST_ENABLED is defined for "test" and "debug" targets (since DEBUG is.)
//   R_UNIT_TEST(name, body) defines a unit test to be run before main()
#ifndef R_UNIT_TEST_ENABLED
  #if DEBUG
    #define R_UNIT_TEST_ENABLED 1
  #else
    #define R_UNIT_TEST_ENABLED 1
  #endif
#endif
#ifndef R_UNIT_TEST_ENV_NAME
  #define R_UNIT_TEST_ENV_NAME "R_UNIT_TEST"
#endif
#if R_UNIT_TEST_ENABLED
  #define R_UNIT_TEST(name, body) \
  __attribute__((constructor,used)) static void unittest_##name() { \
    if (testing_should_run(#name)) {                            \
      printf("TEST " #name " %s\n", __FILE__);                      \
      body                                                          \
    }                                                               \
    return;                                                         \
  }
#else
  #define R_UNIT_TEST(name, body)
#endif

// testing_on returns true if the environment variable R_UNIT_TEST_ENV_NAME is 1
// or a test name prefix.
bool testing_on();
bool testing_should_run(const char* testname);

ASSUME_NONNULL_END
