# termacs-java

The **Java binding** for termacs (`sciens.termacs`) — an idiomatic OOP API over
the [termacs-core](https://github.com/nidwe72/termacs-core) C ABI through a thin
JNI shim. See the canonical design spec in
[termacs-core/`termacs.md`](https://github.com/nidwe72/termacs-core/blob/main/termacs.md)
(§8 covers the binding).

## How it depends on core

`termacs-java` builds its own native lib and consumes **termacs-core from
source** via CMake `FetchContent` — **no prebuilt binaries are published**
(spec §15.1). In the dev cockpit, point it at a local core checkout; releases
pin a core git tag.

## Build

```bash
# 1) native JNI lib — pulls termacs-core via FetchContent (pinned tag)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build                       # -> build/libtermacsjni.so

#    ...or build against a local core checkout (no tag round-trip):
#    cmake -S . -B build -DTERMACS_CORE_DIR=/path/to/termacs-core -G Ninja

# 2) Java classes
javac --release 17 -d build/classes src/main/java/sciens/termacs/*.java
```

Requirements: a JDK (17+), CMake ≥ 3.16, a C++17 compiler, ncursesw/terminfo.

## Runtime

`libtermacsjni.so` is one self-contained lib (JNI + C ABI + core + tvision) —
but tvision needs **ncurses/terminfo** at runtime. If no terminfo entry exists
for `$TERM`, the binding fails with an actionable `IllegalStateException`
instead of crashing (the §15.2 guard). Load it with
`-Dtermacs.jni=…/libtermacsjni.so` and `--enable-native-access=ALL-UNNAMED`.

## Test

```bash
javac --release 17 -d build/classes \
    src/main/java/sciens/termacs/*.java src/test/java/sciens/termacs/HeadlessTest.java
java -ea -Dtermacs.jni=build/libtermacsjni.so --enable-native-access=ALL-UNNAMED \
    -cp build/classes sciens.termacs.HeadlessTest
```
