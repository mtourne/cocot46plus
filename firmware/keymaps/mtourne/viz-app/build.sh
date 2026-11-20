#!/bin/sh
# Build "Cocot Live.app" (the floating cheat-sheet panel) next to this script.
# Needs the Xcode command line tools (swiftc). Run ../draw.sh first so
# visualizer.html is current; it is copied into the app.
set -eu
HERE=$(cd "$(dirname "$0")" && pwd)
APP="$HERE/Cocot Live.app"

rm -rf "$APP"
mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
swiftc -O "$HERE/main.swift" -o "$APP/Contents/MacOS/CocotLive" \
    -framework Cocoa -framework WebKit -framework IOKit
cp "$HERE/../visualizer.html" "$APP/Contents/Resources/"
cat > "$APP/Contents/Info.plist" <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key><string>Cocot Live</string>
    <key>CFBundleIdentifier</key><string>com.mtourne.cocot-live</string>
    <key>CFBundleExecutable</key><string>CocotLive</string>
    <key>CFBundlePackageType</key><string>APPL</string>
    <key>CFBundleShortVersionString</key><string>1.0</string>
    <key>LSMinimumSystemVersion</key><string>13.0</string>
    <key>LSUIElement</key><true/>
    <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
EOF
codesign --force --sign - "$APP" >/dev/null
echo "Built $APP"
