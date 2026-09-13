#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")/.."
mkdir -p build
if command -v llvm-symbolizer-21 >/dev/null; then
    export ASAN_SYMBOLIZER_PATH=$(command -v llvm-symbolizer-21)
fi
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -g \
    -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude \
    tests/core_tests.cpp src/ArchetypedECS/ArchetypeEntityManager.cpp \
    src/ArchetypedECS/ArchECS_Utils.cpp src/ThreadPool.cpp -pthread -o build/core_tests
failed=0
for test in remove reuse iterator string entity thread; do
    if timeout 5s build/core_tests "$test" > "build/test-$test.log" 2>&1; then
        echo "PASS $test"
    else
        echo "FAIL $test"
        head -12 "build/test-$test.log"
        failed=1
    fi
done
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -g \
    -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude \
    tests/json_audio_tests.cpp src/JsonParser.cpp src/UnixApi/SoundEngineAlsa.cpp \
    -pthread -lasound -o build/json_audio_tests
build/json_audio_tests
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -g \
    -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude \
    tests/world_tests.cpp src/ArchetypedECS/ArchECS_World.cpp \
    src/ArchetypedECS/ArchECS_Utils.cpp src/ArchetypedECS/ArchetypeInterface.cpp -o build/world_tests
build/world_tests
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -g \
    -fsanitize=address,undefined -fno-omit-frame-pointer -Iinclude \
    tests/camera_tests.cpp -o build/camera_tests
build/camera_tests
exit "$failed"
