#!/bin/bash

if [ ! -d src ]; then
  echo "Please run release.sh from project root. Aborting"
  exit 1
fi

source ./release/functions.sh

snapshot=true

while getopts "k:x" opt
do
  case $opt in
    k) gpg_key="$OPTARG"
       ;;
    x) snapshot=false
       ;;
  esac
done

if [ -z $gpg_key ]; then
  echo "Missing required option -k gpg_key_id"
  exit 1
fi

version=$(cat ./VERSION)

branch=$(git branch --list develop)
if [ ! "${branch::1}" == "*" ]; then
  echo "Not on develop branch. Aborting"
  exit 1
fi

if [ $snapshot == true ]; then
  echo "Building snapshot release"
  ./release/build_deb.sh -rsx -k $gpg_key
else
  echo "Building stable release"
  ./release/build_deb.sh -rs -k $gpg_key
fi

changes_file=../*.changes

if [ $snapshot == true ]; then
  dput ppa:rjinman/snapshots "$changes_file"
fi
dput ppa:rjinman/ppa "$changes_file"

git add ./debian/changelog ./VERSION
git commit -m "Updated changelog and VERSION"

git push

git checkout master
git merge develop

if [ $snapshot != true ]; then
  git tag -a "v$version" -m "Pro Office Calculator v$version"
fi

git push --follow-tags
git checkout develop

win_artifact="$(find ./artifacts -name *.msi)"
osx_artifact="$(find ./artifacts -name *.app.zip)"

win_artifact_name="ProOfficeCalculator_${full_version}.msi"
osx_artifact_name="ProOfficeCalculator_${full_version}.app.zip"

mv "$win_artifact" "./artifacts/$win_artifact_name"
mv "$osx_artifact" "./artifacts/$osx_artifact_name"

cat > ./artifacts/manifest << EOL
<artifacts>
  <windows>${win_artifact_name}</windows>
  <osx>${osx_artifact_name}</osx>
</artifacts>
EOL

aws s3 cp ./artifacts/ s3://proofficecalculator.com/downloads/ --recursive