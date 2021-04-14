d=`date "+%Y%m%d_%H%M"`

WORK_DIR="work_${d}"

rm -rf "${WORK_DIR}"
mkdir "${WORK_DIR}"
pushd "${WORK_DIR}"

curl -L https://www.libsdl.org/release/SDL2-2.0.12.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz | tar -xzv
curl -L https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.1.tar.gz | tar -xzv
curl -L https://github.com/nlohmann/json/archive/v3.8.0.tar.gz | tar -xzv

mv SDL2-2.0.12/android-project/ .

mv SDL2* android-project/app/jni/
mv json* android-project/app/jni/

mkdir android-project/app/src/main/assets
cp -R ../../src/* android-project/app/jni/src/
cp -R ../../sprites android-project/app/src/main/assets/
cp -R ../../config android-project/app/src/main/assets/
cp -R ../../maps android-project/app/src/main/assets/
cp -R ../../sounds android-project/app/src/main/assets/
cp ../Android.mk android-project/app/jni/src/
cp ../Application.mk android-project/app/jni/
cp ../nlohmann_Android.mk android-project/app/jni/json-3.8.0/Android.mk
sed -i.bak 's/\(.*<activity.*\)/\1 android:screenOrientation="landscape"/g' android-project/app/src/main/AndroidManifest.xml
sed -i.bak 's/\(.*<application .*\)/\<uses-permission android:name="android.permission.INTERNET" \/\> \1/g' android-project/app/src/main/AndroidManifest.xml

popd