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

mkdir src

folders="engine game net sprite ui trippin"

for folder in `echo "$folders"`
do
  cp -R ../../src/${folder} src/
done

mkdir build
pushd build

cmake .. -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=12.4

cp ../../Info.plist ./CMakeFiles/trippin.dir/Info.plist

popd
popd
