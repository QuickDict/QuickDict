# [QuickDict ![](resources/images/QuickDict-32x32.png)](https://github.com/QuickDict/QuickDict)

QuickDict is a cross-platform dictionary/translation application.

## How to Build
### Prerequisite
    * Qt 5.15.2
    * Tesseract 4.1.1
    * Leptonica 1.81.1
    * Libmobi 0.9
    * OpenCV 4.5.4
    * OpenCC 1.1.2
    * Hunspell 1.7.0
    * QHotkey 1.5.0
    * Axios 0.24.0

### Build on Linux
```sh
git clone --recurse-submodules https://github.com/QuickDict/QuickDict
cd QuickDict
cmake -S . -B build
cmake --build build
ln -s PROJECT_DIR/contrib/ ~/.config/QuickDict/qml/
./build/QuickDict/QuickDict
```

### Build on Windows
```sh
git clone --recurse-submodules https://github.com/QuickDict/QuickDict
cd QuickDict
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake -DOpenCV_DIR=/C/Program\ Files/opencv/build/
cmake --build build
cp -r contrib/* ~/AppData/Local/QuickDict/qml/
./build/debug/QuickDict/QuickDict
```

## License
QuickDict is licensed under the GNU General Public License 3 license. See [LICENSE](LICENSE) for details.
