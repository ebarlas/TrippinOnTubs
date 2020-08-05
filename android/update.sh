WORK_DIR="work"

pushd "${WORK_DIR}"

rm -rf android-project/app/src/main/assets/*
rm -rf android-project/app/jni/src/*

cp -R ../../src/* android-project/app/jni/src/
cp -R ../../fonts android-project/app/src/main/assets/
cp -R ../../sprites android-project/app/src/main/assets/

popd