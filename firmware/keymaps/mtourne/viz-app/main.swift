// Cocot Live: a small floating macOS panel that shows the cocot46plus cheat
// sheet and lights up keys as they are pressed. It lives in the menu bar: the
// keyboard icon shows the active layer and held modifiers, a click shows or
// hides the panel (closing the panel only hides it), and right-click has Quit.
//
// The keyboard reports key events over raw HID (see send_to_visualizer in
// keymap.c) only while it gets a ping at least every 5 s; this app pings every
// 2 s while it runs. The drawing is visualizer.html (built by draw.sh); the app
// feeds it the reports by calling its show() function.
//
// Build: ./build.sh  (makes "Cocot Live.app" next to this file)

import Cocoa
import IOKit.hid
import WebKit

let vendorID = 0x1727, productID = 0x0003
let usagePage = 0xFF60, usage = 0x61  // QMK raw HID
let reportSize = 32

final class Keyboard {
    private let manager = IOHIDManagerCreate(kCFAllocatorDefault, IOOptionBits(kIOHIDOptionsTypeNone))
    private var device: IOHIDDevice?
    // Report buffer owned for the app's lifetime (IOKit writes into it).
    private let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: reportSize)
    private var pingTimer: Timer?
    var onReport: (([UInt8]) -> Void)?
    var onStatus: ((String) -> Void)?

    func start() {
        let matching: [String: Any] = [
            kIOHIDVendorIDKey: vendorID, kIOHIDProductIDKey: productID,
            kIOHIDPrimaryUsagePageKey: usagePage, kIOHIDPrimaryUsageKey: usage,
        ]
        IOHIDManagerSetDeviceMatching(manager, matching as CFDictionary)
        let context = Unmanaged.passUnretained(self).toOpaque()
        IOHIDManagerRegisterDeviceMatchingCallback(manager, { context, _, _, device in
            Unmanaged<Keyboard>.fromOpaque(context!).takeUnretainedValue().attach(device)
        }, context)
        IOHIDManagerRegisterDeviceRemovalCallback(manager, { context, _, _, _ in
            Unmanaged<Keyboard>.fromOpaque(context!).takeUnretainedValue().detach()
        }, context)
        IOHIDManagerScheduleWithRunLoop(manager, CFRunLoopGetMain(), CFRunLoopMode.defaultMode.rawValue)
        let result = IOHIDManagerOpen(manager, IOOptionBits(kIOHIDOptionsTypeNone))
        onStatus?(result == kIOReturnSuccess ? "waiting for the keyboard" : "no access to the keyboard (check Input Monitoring)")
    }

    private func attach(_ device: IOHIDDevice) {
        self.device = device
        let context = Unmanaged.passUnretained(self).toOpaque()
        IOHIDDeviceRegisterInputReportCallback(device, buffer, reportSize, { context, _, _, _, _, report, length in
            let bytes = Array(UnsafeBufferPointer(start: report, count: length))
            Unmanaged<Keyboard>.fromOpaque(context!).takeUnretainedValue().onReport?(bytes)
        }, context)
        ping()
        pingTimer = Timer.scheduledTimer(withTimeInterval: 2, repeats: true) { [weak self] _ in self?.ping() }
        onStatus?("connected")
    }

    private func detach() {
        pingTimer?.invalidate()
        device = nil
        onStatus?("keyboard unplugged")
    }

    // Tell the keyboard someone is watching; it only reports while pinged.
    private func ping() {
        guard let device else { return }
        let ping = [UInt8](repeating: 0x56, count: reportSize)
        IOHIDDeviceSetReport(device, kIOHIDReportTypeOutput, 0, ping, reportSize)
    }
}

final class AppDelegate: NSObject, NSApplicationDelegate, WKNavigationDelegate, NSWindowDelegate {
    private var panel: NSPanel!
    private var statusItem: NSStatusItem!
    private var connected = false
    private var web: WKWebView!
    private let keyboard = Keyboard()
    private var pageReady = false

    func applicationDidFinishLaunching(_ notification: Notification) {
        let frame = NSRect(x: 0, y: 0, width: 1000, height: 540)
        panel = NSPanel(contentRect: frame,
                        styleMask: [.titled, .closable, .resizable, .fullSizeContentView, .nonactivatingPanel],
                        backing: .buffered, defer: false)
        panel.titleVisibility = .hidden
        panel.titlebarAppearsTransparent = true
        panel.isMovableByWindowBackground = true
        panel.level = .floating  // always on top
        panel.collectionBehavior = [.canJoinAllSpaces, .fullScreenAuxiliary]
        panel.isOpaque = false
        panel.backgroundColor = .clear
        panel.hidesOnDeactivate = false
        // Only the close button: closing the panel hides it (see windowShouldClose).
        panel.standardWindowButton(.miniaturizeButton)?.isHidden = true
        panel.standardWindowButton(.zoomButton)?.isHidden = true

        let blur = NSVisualEffectView(frame: frame)
        blur.material = .hudWindow
        blur.blendingMode = .behindWindow
        blur.state = .active
        blur.wantsLayer = true
        blur.layer?.cornerRadius = 14
        blur.layer?.masksToBounds = true
        panel.contentView = blur

        // The page (which already shows one board per layer, cropped), restyled
        // for the panel with a compact status bar: a
        // connection dot, a chip with the active layer in its LED colour
        // (hidden on the base layer, like the OLED). The page itself shows the held
        // modifiers between the halves.
        let style = """
            document.body.style.cssText = 'background: transparent; margin: 0 14px 12px; color: #f4f4f4; font: 13px -apple-system, system-ui; -webkit-user-select: none; cursor: default;';
            const css = document.createElement('style');
            css.textContent = `
              #bar { display: flex; align-items: center; gap: 10px; height: 24px; margin: 0 0 6px; }
              #dot { width: 8px; height: 8px; border-radius: 50%; background: #8a8a8a; }
              #dot.on { background: #3ddc84; box-shadow: 0 0 6px #3ddc84; }
              #chip { padding: 2px 10px; border-radius: 10px; font-weight: 600; font-size: 12px; letter-spacing: 0.6px; display: none; }
              .key rect { transition: fill 60ms; }
              #hud { color: #fff; }
              #hud span { opacity: 0.18; }
              #hud span.on { opacity: 1; text-shadow: 0 0 8px rgba(255, 255, 255, 0.6); }`;
            document.head.appendChild(css);
            document.getElementById('bar').innerHTML =
              '<span id="dot"></span><span id="chip"></span>' +
              '<span id="status" hidden></span><span id="layer" hidden></span>';
            const CHIPS = { 2: ['NAV', '#fde68a', '#5a4300'], 3: ['NUM', '#bbf7d0', '#14532d'],
                            4: ['FUN', '#f5d0fe', '#701a75'], 5: ['MOUSE', '#a5f3fc', '#164e63'] };
            const showReport = window.show;
            window.show = (d) => {
              showReport(d);
              if (d[0] !== 0x4B) return;
              const chip = document.getElementById('chip'), layer = CHIPS[d[4]];
              chip.style.display = layer ? 'inline-block' : 'none';
              if (layer) { chip.textContent = layer[0]; chip.style.background = layer[1]; chip.style.color = layer[2]; }
            };
            window.setStatus = (text) => {
              const dot = document.getElementById('dot');
              dot.className = text === 'connected' ? 'on' : '';
              dot.title = text;
            };
            """
        let config = WKWebViewConfiguration()
        config.userContentController.addUserScript(WKUserScript(source: style, injectionTime: .atDocumentEnd, forMainFrameOnly: true))
        web = WKWebView(frame: NSRect(x: 0, y: 0, width: frame.width, height: frame.height - 26), configuration: config)
        web.autoresizingMask = [.width, .height]
        web.setValue(false, forKey: "drawsBackground")
        web.navigationDelegate = self
        blur.addSubview(web)
        if let page = Bundle.main.url(forResource: "visualizer", withExtension: "html") {
            web.loadFileURL(page, allowingReadAccessTo: page.deletingLastPathComponent())
        }

        keyboard.onReport = { [weak self] bytes in
            self?.run("show([\(bytes.map(String.init).joined(separator: ","))])")
            if bytes.count > 5, bytes[0] == 0x4B { self?.updateMenuBar(layer: bytes[4], mods: bytes[5]) }
        }
        keyboard.onStatus = { [weak self] text in
            self?.run("setStatus('\(text)')")
            self?.connected = text == "connected"
            self?.updateMenuBar(layer: 0, mods: 0)
        }

        panel.delegate = self
        statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)
        if let button = statusItem.button {
            button.image = NSImage(systemSymbolName: "keyboard", accessibilityDescription: "Cocot Live")
            button.imagePosition = .imageLeading
            button.target = self
            button.action = #selector(statusClicked)
            button.sendAction(on: [.leftMouseUp, .rightMouseUp])
        }
        updateMenuBar(layer: 0, mods: 0)

        panel.center()
        panel.orderFrontRegardless()
    }

    func webView(_ webView: WKWebView, didFinish navigation: WKNavigation!) {
        pageReady = true
        keyboard.start()
    }

    private func run(_ script: String) {
        guard pageReady else { return }
        web.evaluateJavaScript(script, completionHandler: nil)
    }

    // Menu bar text: the layer (nothing on the base layer, like the OLED) and
    // the held modifiers; the icon is dimmed while the keyboard is away.
    private func updateMenuBar(layer: UInt8, mods: UInt8) {
        guard let button = statusItem?.button else { return }
        let names: [UInt8: String] = [2: "NAV", 3: "NUM", 4: "FUN", 5: "MOUSE"]
        let symbols: [(UInt8, String)] = [(0x11, "⌃"), (0x44, "⌥"), (0x88, "⌘"), (0x22, "⇧")]
        let held = symbols.filter { mods & $0.0 != 0 }.map(\.1).joined()
        let text = [names[layer] ?? "", held].filter { !$0.isEmpty }.joined(separator: " ")
        button.title = text.isEmpty ? "" : " " + text
        button.appearsDisabled = !connected
    }

    @objc private func statusClicked() {
        if NSApp.currentEvent?.type == .rightMouseUp {
            let menu = NSMenu()
            menu.addItem(withTitle: panel.isVisible ? "Hide panel" : "Show panel", action: #selector(togglePanel), keyEquivalent: "").target = self
            menu.addItem(.separator())
            menu.addItem(withTitle: "Quit Cocot Live", action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q")
            statusItem.menu = menu
            statusItem.button?.performClick(nil)
            statusItem.menu = nil  // so a left click toggles again
        } else {
            togglePanel()
        }
    }

    @objc private func togglePanel() {
        if panel.isVisible { panel.orderOut(nil) } else { panel.orderFrontRegardless() }
    }

    // The close button only hides the panel; the menu bar icon brings it back.
    func windowShouldClose(_ sender: NSWindow) -> Bool {
        panel.orderOut(nil)
        return false
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool { false }
}

let app = NSApplication.shared
let delegate = AppDelegate()
app.delegate = delegate
app.setActivationPolicy(.accessory)  // no Dock icon; quit from the menu bar icon
app.run()
