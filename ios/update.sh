if [ "$#" -ne 1 ]; then
    echo "1 arguments required: [work dir]"
    exit
fi

pushd "${1}"

rm -rf sprites
rm -rf config
rm -rf maps
rm -rf sounds
rm -rf autoplay

cp -R ../../sprites .
cp -R ../../config .
cp -R ../../maps .
cp -R ../../sounds .
cp -R ../../autoplay .

rm -rf src
mkdir src

folders="engine game net sprite ui trippin"

for folder in `echo "$folders"`
do
  cp -R ../../src/${folder} src/
done

popd