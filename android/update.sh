if [ "$#" -ne 1 ]; then
    echo "1 arguments required: [work dir]"
    exit
fi

pushd "${1}"

rm -rf trippin/app/src/main/assets/*
rm -rf trippin/app/jni/src/*

cp -R ../Android.mk trippin/app/jni/src/
cp -R ../../src/* trippin/app/jni/src/
cp -R ../../config trippin/app/src/main/assets/
cp -R ../../maps trippin/app/src/main/assets/
cp -R ../../sprites trippin/app/src/main/assets/
cp -R ../../sounds trippin/app/src/main/assets/
cp -R ../../autoplay trippin/app/src/main/assets/

# SDLActivity invokes nativeRunMain JNI function
# nativeRunMain, defined in SDL_android.c, looks for SDL_main function
# Troubleshoot using objdump, for ex objdump -T app/build/intermediates/ndkBuild/debug/obj/local/arm64-v8a/libmain.so | grep SDL_main
sed -i.bak 's/int main/int SDL_main/g' trippin/app/jni/src/trippin/trippin.cpp

popd