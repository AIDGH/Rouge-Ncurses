#!/bin/bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
./build-macos.sh
app="build-macos/Rouge Dungeon.app"
mkdir -p "$app/Contents/MacOS" "$app/Contents/Resources"
clang packaging/MacLauncher.m -framework AppKit -o "$app/Contents/MacOS/RougeDungeonLauncher"
cp packaging/AppIcon.icns "$app/Contents/Resources/AppIcon.icns"
/usr/libexec/PlistBuddy -c 'Clear dict' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundleExecutable string RougeDungeonLauncher' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundleIdentifier string com.aidgh.rougedungeon.local' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundleName string Rouge Dungeon' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundlePackageType string APPL' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundleIconFile string AppIcon' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add CFBundleVersion string 1' "$app/Contents/Info.plist"
/usr/libexec/PlistBuddy -c 'Add LSUIElement bool true' "$app/Contents/Info.plist"
codesign --force --sign - "$app"
printf 'Ready: %s/%s\n' "$PWD" "$app"
