set -e

PROJECT_NAME="light"
BUILD_DIR="build"
CONFIG="Release"
SOURCE_DIR="$(pwd)"
VCPKG_TOOLCHAIN="${VCPKG_TOOLCHAIN:-$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake}"
if [ ! -f "$VCPKG_TOOLCHAIN" ]; then
    echo "vcpkg toolset not found: $VCPKG_TOOLCHAIN"
    exit 1
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake -DCMAKE_TOOLCHAIN_FILE="$VCPKG_TOOLCHAIN" -DCMAKE_BUILD_TYPE=$CONFIG "$SOURCE_DIR"

cmake --build . --config $CONFIG

if [ -f "$SOURCE_DIR/.env" ]; then
    cp "$SOURCE_DIR/.env" "$PROJECT_NAME"
    echo ".env copied next to the binary"
else
    echo ".env not found in source directory"
fi

if [ -f "$PROJECT_NAME" ]; then
    echo "Launch $PROJECT_NAME..."
    ./"$PROJECT_NAME"
else
    echo "Binary $PROJECT_NAME not found"
fi

cd "$SOURCE_DIR"
