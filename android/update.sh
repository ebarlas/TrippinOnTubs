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
cp -R ../../autoplay android-project/app/src/main/assets/

# SDLActivity invokes nativeRunMain JNI function
# nativeRunMain, defined in SDL_android.c, looks for SDL_main function
# Troubleshoot using objdump, for ex objdump -T app/build/intermediates/ndkBuild/debug/obj/local/arm64-v8a/libmain.so | grep SDL_main
sed -i.bak 's/int main() {/int SDL_main(int argc, char *argv[]) {/g' android-project/app/jni/src/game/trippin.cpp

popd