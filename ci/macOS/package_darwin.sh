#!/bin/bash
set -ex
REPO_SRC=$(git rev-parse --show-toplevel)
source $REPO_SRC/ci/macOS/macos_config.sh

cd $BUILDDIR

echo "### Copy DLLs to Frameworks folder"
mkdir -p $BUILDDIR/Scopy.app/Contents/Frameworks
find . -name "*.dylib" -d 2 -exec cp "{}" $BUILDDIR/Scopy.app/Contents/Frameworks \;
mkdir -p $BUILDDIR/Scopy.app/Contents/MacOS/plugins/plugins
mkdir -p $BUILDDIR/Scopy.app/Contents/MacOS/translations
cp $BUILDDIR/translations/* $BUILDDIR/Scopy.app/Contents/MacOS/translations
cp -R $BUILDDIR/plugins/regmap/xmls $BUILDDIR/Scopy.app/Contents/MacOS/plugins/plugins
cp -R $STAGING_AREA/libiio/build/iio.framework Scopy.app/Contents/Frameworks/
cp -R $STAGING_AREA/libad9361/build/ad9361.framework Scopy.app/Contents/Frameworks/

libqwtpath=${STAGING_AREA_DEPS}/lib/libqwt.6.4.0.dylib
libqwtid="$(otool -D ${libqwtpath} | tail -1)"

iiorpath="$(otool -D ./Scopy.app/Contents/Frameworks/iio.framework/iio | grep @rpath)"
iioid=${iiorpath#"@rpath/"}

ad9361rpath="$(otool -D ./Scopy.app/Contents/Frameworks/ad9361.framework/ad9361 | grep @rpath)"
ad9361id=${ad9361rpath#"@rpath/"}

libusbpath="$(otool -L ./Scopy.app/Contents/Frameworks/iio.framework/iio | grep libusb | cut -d " " -f 1 | awk '{$1=$1};1')"
libusbid="$(echo ${libusbpath} | rev | cut -d "/" -f 1 | rev)"
cp ${libusbpath} ./Scopy.app/Contents/Frameworks/

m2kpath=${STAGING_AREA_DEPS}/lib/libm2k.dylib
m2krpath="$(otool -D ${m2kpath} | grep @rpath)"
m2kid=${m2krpath#"@rpath/"}
cp ${STAGING_AREA_DEPS}/lib/libm2k.* ./Scopy.app/Contents/Frameworks
sudo install_name_tool -id @executable_path/../Frameworks/${m2kid} ./Scopy.app/Contents/Frameworks/${m2kid}


echo "### Check available python version"
for version in 3.8 3.9 3.10 3.11 3.12
do
	if [ -e /usr/local/opt/python@$version/Frameworks/Python.framework/Versions/$version/Python ] ; then
		pythonpath=/usr/local/opt/python@$version/Frameworks/Python.framework/Versions/$version/Python
		pyversion=$version
		pythonidrpath="$(otool -D $pythonpath | head -2 |  tail -1)"
	fi
done

if [ -z $pyversion ] ; then
	echo "No Python 3.8, 3.9, 3.10, 3.11, 3.12 paths found"
	exit 1
fi
echo " - Found python$version at $pythonpath"
pythonid=${pythonidrpath#"/usr/local/opt/python@${pyversion}/Frameworks/"}
cp -R /usr/local/opt/python@$pyversion/Frameworks/Python.framework Scopy.app/Contents/Frameworks/


echo "### Change the id libqwt inside every dll"
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-gr-util.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-gr-gui.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-gui.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-sigrok-gui.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-core.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/Frameworks/libscopy-iio-widgets.dylib
sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/MacOS/Scopy
ls $BUILDDIR/Scopy.app/Contents/MacOs/plugins/plugins | grep libscopy | while read plugin
do
	sudo install_name_tool -change $libqwtid $libqwtpath $BUILDDIR/Scopy.app/Contents/MacOS/plugins/plugins/$plugin
done


echo "### Fixing scopy plugins"
ls $BUILDDIR/Scopy.app/Contents/MacOs/plugins/plugins | grep libscopy | while read plugin
do
	echo "--- FIXING PLUGIN: $plugin"
	echo $STAGING_AREA_DEPS/lib | dylibbundler --no-codesign --overwrite-files --bundle-deps -cd \
	--fix-file $BUILDDIR/Scopy.app/Contents/MacOS/plugins/plugins/$plugin \
	--dest-dir $BUILDDIR/Scopy.app/Contents/Frameworks/ \
	--install-path @executable_path/../Frameworks/ \
	--search-path $BUILDDIR/Scopy.app/Contents/Frameworks/
done


echo "### Fixing Scopy binary"
echo $STAGING_AREA_DEPS/lib | dylibbundler -ns -of -b \
-x $BUILDDIR/Scopy.app/Contents/MacOS/Scopy \
-d $BUILDDIR/Scopy.app/Contents/Frameworks  \
-p @executable_path/../Frameworks \
-s $BUILDDIR/Scopy.app/Contents/Frameworks

echo "### Fixing the frameworks dylibbundler failed to copy"
echo "=== Fixing iio.framework"
sudo install_name_tool -id @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/iio.framework/iio
sudo install_name_tool -id @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/${iioid}
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/MacOS/Scopy
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-iioutil.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-iio-widgets.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-core.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-pluginbase.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-gr-util.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-gr-gui.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-sigrok-gui.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libscopy-gui.dylib
ls $BUILDDIR/Scopy.app/Contents/MacOs/plugins/plugins | grep libscopy | while read plugin
do
	sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/MacOS/plugins/plugins/$plugin
done

echo "=== Fixing ad9361.framework"
sudo install_name_tool -id @executable_path/../Frameworks/${ad9361id} ./Scopy.app/Contents/Frameworks/ad9361.framework/ad9361
sudo install_name_tool -id @executable_path/../Frameworks/${ad9361id} ./Scopy.app/Contents/Frameworks/${ad9361id}
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/${ad9361id}
sudo install_name_tool -change ${ad9361rpath} @executable_path/../Frameworks/${ad9361id} ./Scopy.app/Contents/Frameworks/libgnuradio-iio*

echo "=== Fixing libusb"
sudo install_name_tool -id @executable_path/../Frameworks/${libusbid} ./Scopy.app/Contents/Frameworks/${libusbid}
sudo install_name_tool -change ${libusbpath} @executable_path/../Frameworks/${libusbid} ./Scopy.app/Contents/Frameworks/iio.framework/iio

echo "=== Fixing python"
sudo install_name_tool -id @executable_path/../Frameworks/${pythonid} ./Scopy.app/Contents/Frameworks/${pythonid}
python_sigrokdecode=$(otool -L ./Scopy.app/Contents/Frameworks/libsigrokdecode* | grep python | cut -d " " -f 1 | awk '{$1=$1};1')
sudo install_name_tool -change ${python_sigrokdecode} @executable_path/../Frameworks/${pythonid} ./Scopy.app/Contents/Frameworks/libsigrokdecode*
python_scopy=$(otool -L ./Scopy.app/Contents/MacOS/Scopy | grep -i python | cut -d " " -f 1 | awk '{$1=$1};1')
sudo install_name_tool -change ${python_scopy} @executable_path/../Frameworks/${pythonid} ./Scopy.app/Contents/MacOS/Scopy
python_libscopycore=$(otool -L ./Scopy.app/Contents/Frameworks/libscopy-core.dylib | grep -i python | cut -d " " -f 1 | awk '{$1=$1};1')
sudo install_name_tool -change ${python_libscopycore} @executable_path/../Frameworks/${pythonid} ./Scopy.app/Contents/MacOS/Scopy

echo "=== Fixing libserialport"
libserialportpath="$(otool -L ./Scopy.app/Contents/Frameworks/iio.framework/iio | grep libserialport | cut -d " " -f 1 | awk '{$1=$1};1')"
libserialportid="$(echo ${libserialportpath} | rev | cut -d "/" -f 1 | rev)"
sudo install_name_tool -change ${libserialportpath} @executable_path/../Frameworks/${libserialportid} ./Scopy.app/Contents/Frameworks/iio.framework/iio

sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libm2k.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libm2k.?.?.?.dylib
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libgnuradio-m2k*
sudo install_name_tool -change ${iiorpath} @executable_path/../Frameworks/${iioid} ./Scopy.app/Contents/Frameworks/libgnuradio-scopy*
sudo install_name_tool -change ${m2krpath} @executable_path/../Frameworks/${m2kid} ./Scopy.app/Contents/Frameworks/libgnuradio-m2k*
sudo install_name_tool -change ${m2krpath} @executable_path/../Frameworks/${m2kid} ./Scopy.app/Contents/Frameworks/libgnuradio-scopy*

echo "=== Fixing iio-emu + libtinyiiod"
cp $REPO_SRC/iio-emu/build/iio-emu ./Scopy.app/Contents/MacOS/
echo $STAGING_AREA_DEPS/lib | dylibbundler -ns -of -b \
	--fix-file $BUILDDIR/Scopy.app/Contents/MacOS/iio-emu \
	--dest-dir $BUILDDIR/Scopy.app/Contents/Frameworks/ \
	--install-path @executable_path/../Frameworks/ \
	--search-path $BUILDDIR/Scopy.app/Contents/Frameworks/

echo "=== Bundle the Qt libraries & Create Scopy.dmg"
sudo macdeployqt Scopy.app -verbose=3
zip -Xvr ScopyApp.zip Scopy.app
sudo macdeployqt Scopy.app -dmg -verbose=3
