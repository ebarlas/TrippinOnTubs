if [ "$#" -ne 1 ]; then
    echo "1 arguments required: [work dir]"
    exit
fi

pushd "${1}"

rm -rf android-project/app/src/main/assets/*
rm -rf android-project/app/jni/src/*

cp -R ../Android.mk android-project/app/jni/src/
cp -R ../../src/* android-project/app/jni/src/
cp -R ../../config android-project/app/src/main/assets/
cp -R ../../maps android-project/app/src/main/assets/
cp -R ../../sprites android-project/app/src/main/assets/
cp -R ../../sounds android-project/app/src/main/assets/

popd