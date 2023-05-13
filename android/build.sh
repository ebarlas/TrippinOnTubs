d=`date "+%Y%m%d_%H%M"`

WORK_DIR="work_${d}"

rm -rf "${WORK_DIR}"
mkdir "${WORK_DIR}"
pushd "${WORK_DIR}"

curl -L https://www.libsdl.org/release/SDL2-2.26.5.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.6.3.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.6.3.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.2.0.tar.gz | tar -xzv
curl -L https://github.com/nlohmann/json/archive/v3.11.2.tar.gz | tar -xzv

mv SDL2-2.26.5/android-project/ .

mv android-project trippin

mv SDL2* trippin/app/jni/
mv json* trippin/app/jni/

mkdir trippin/app/src/main/assets
cp -R ../../src/* trippin/app/jni/src/
cp -R ../../sprites trippin/app/src/main/assets/
cp -R ../../config trippin/app/src/main/assets/
cp -R ../../maps trippin/app/src/main/assets/
cp -R ../../sounds trippin/app/src/main/assets/
cp -R ../../autoplay trippin/app/src/main/assets/
cp ../Android.mk trippin/app/jni/src/
cp ../Application.mk trippin/app/jni/
cp ../nlohmann_Android.mk trippin/app/jni/json-3.11.2/Android.mk
sed -i.bak 's/\(.*<activity.*\)/\1 android:screenOrientation="landscape"/g' trippin/app/src/main/AndroidManifest.xml
sed -i.bak 's/\(.*<application .*\)/\<uses-permission android:name="android.permission.INTERNET" \/\> \1/g' trippin/app/src/main/AndroidManifest.xml
sed -i.bak 's/super.onCreate.*/&\n        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {\n            getWindow().getAttributes().layoutInDisplayCutoutMode = WindowManager.LayoutParams.LAYOUT_IN_DISPLAY_CUTOUT_MODE_SHORT_EDGES;\n        }/g' trippin/app/src/main/java/org/libsdl/app/SDLActivity.java

# SDLActivity invokes nativeRunMain JNI function
# nativeRunMain, defined in SDL_android.c, looks for SDL_main function
# Troubleshoot using objdump, for ex objdump -T app/build/intermediates/ndkBuild/debug/obj/local/arm64-v8a/libmain.so | grep SDL_main
sed -i.bak 's/int main/int SDL_main/g' trippin/app/jni/src/trippin/trippin.cpp

popd