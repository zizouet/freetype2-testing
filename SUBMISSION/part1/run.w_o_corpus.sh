PROJECT_NAME="freetype2"
FUZZER_NAME="ftfuzzer"
RUN_TIME=$((4 * 60 * 60))


FUZZER_ARGS="-rss_limit_mb=2560 -fork=$(nproc) -ignore_crashes=1"

echo "Applying git diff"
git apply oss-fuzz.diff
git apply project.diff

echo "Building output folder"
mkdir build/out/run.w_o_corpus

echo "Building image for $PROJECT_NAME"
python3 infra/helper.py build_image "$PROJECT_NAME"

echo "Building fuzzers for $PROJECT_NAME"
python3 infra/helper.py build_fuzzers "$PROJECT_NAME"

echo "Running $FUZZER_NAME for 4 hours"
python3 infra/helper.py run_fuzzer "$PROJECT_NAME" "$FUZZER_NAME" --corpus-dir build/out/run.w_o_corpus --run_time="$RUN_TIME" -e FUZZER_ARGS ="$FUZZER_ARGS"

echo "Creating coverage report"
python3 infra/helper.py build_fuzzers --sanitizer coverage freetype2
python3 infra/helper.py coverage freetyp2 --corpus-dir build/out/run.w__ocorpus/ --fuzz-target ftfuzzer



