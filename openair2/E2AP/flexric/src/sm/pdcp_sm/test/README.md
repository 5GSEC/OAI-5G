To build the project

## Profiling
```bash
  gprof ./build/test_pdcp_sm gmon.out > analysis.txt
```
  
## Coverage

```bash
find . -name ".gcno"
```

cd to the path 

```bash
lcov -t "result" -o ex_test.info -c -d .
genhtml -o res ex_test.info
firefox res/index.html
```

## Compiler
For clang, select CLANG from the ccmake and enter the following line

```bash
cmake .. -D CMAKE_C_COMPILER=clang
```

