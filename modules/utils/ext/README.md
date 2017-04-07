
External libraries
==================

- RapidCheck

  C++ framework for property based testing inspired by QuickCheck.

  Official repository: https://github.com/emil-e/rapidcheck.

  Files based the following fork (adds fixes for compiler warnings):
  https://github.com/dergraaf/rapidcheck/commit/90d2e32cbfb178cc3d70372d53143e87c61598bb

- Google Test:

  Google's C++ test framework! Provides an XUnit test framework.

  Files Bases on:
  https://github.com/google/googletest/releases/tag/release-1.8.0

  Fused source is generated by running:
  ```
  $ googletest-release-1.8.0/googlemock/scripts/fuse_gmock_files.py googletest-1.8.0-fused
  ```