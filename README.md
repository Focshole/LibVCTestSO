# TestIRL

Testing libvc to inject code fixes onto existing binaries

## Howto

First, do once:

```bash
./install_libVC.sh && ./build_test_binary.sh
```
Those commands will build libVC and the rust example library. They will also generate ./externalLibrary/HowToImport.h which will suggest which signature you should use to integrate the shared library.

Then to compile & run

```bash
./build_test_binary_fast.sh && ./deploy.sh && ./build/run_loadSo_test
```
