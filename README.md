# LibVCTestSO

Testing libVersioningCompiler to load external shared libraries

## Howto

First, do once:

```bash
./install_libVC.sh
```

To install libVersioningCompiler in ./libVC. Then to compile & run, just do:

```bash
./build_test_binary.sh && ./deploy.sh && ./build/run_loadSo_test
```
