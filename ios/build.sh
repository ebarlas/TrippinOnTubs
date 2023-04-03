d=`date "+%Y%m%d_%H%M"`

WORK_DIR="work_${d}"

rm -rf "${WORK_DIR}"
mkdir "${WORK_DIR}"
pushd "${WORK_DIR}"

cp -R ../../sprites .
cp -R ../../config .
cp -R ../../maps .
cp -R ../../sounds .
cp -R ../../autoplay .

cp ../CMakeLists.txt .

# ios-cmake is a CMake toolchain file for iOS
# it generates Trippin.xcodeproj
curl -s "https://raw.githubusercontent.com/leetal/ios-cmake/master/ios.toolchain.cmake" --output ios.toolchain.cmake

mkdir src

folders="engine game net sprite ui trippin"

for folder in `echo "$folders"`
do
  cp -R ../../src/${folder} src/
done

mkdir build
pushd build

cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=OFF -DDEPLOYMENT_TARGET=14.0

cp ../../Info.plist ./CMakeFiles/trippin.dir/Info.plist

# iosLaunchScreen.storyboard was provided by @RavBug in SDL Discord
# https://github.com/RavEngine/Samples/blob/master/Samples/iosLaunchScreen.storyboard
# https://github.com/RavEngine/Samples/blob/master/Samples/Info.plist.in
cp ../../iosLaunchScreen.storyboard ./CMakeFiles/trippin.dir/iosLaunchScreen.storyboard

popd
popd