PROJECT_NAME="freetype2"
FUZZER_NAME="ftfuzzer"
RUN_TIME=$((4 * 60 * 60))

echo "Building output folder"
mkdir build/out/ftfuzzerseeds4h

echo "Building image for $PROJECT_NAME"
python3 infra/helper.py build_image "$PROJECT_NAME"

echo "Building fuzzers for $PROJECT_NAME"
python3 infra/helper.py build_fuzzers "$PROJECT_NAME"

echo "Running $FUZZER_NAME for 4 hours"
python3 infra/helper.py run_fuzzer "$PROJECT_NAME" "$FUZZER_NAME" --run_time="$RUN_TIME" 

echo "Creating coverage report



