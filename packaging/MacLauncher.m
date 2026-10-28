#import <AppKit/AppKit.h>
int main(void) {
    @autoreleasepool {
        NSURL *project=[[[NSBundle mainBundle] bundleURL] URLByDeletingLastPathComponent];
        project=[project URLByDeletingLastPathComponent];
        NSURL *script=[project URLByAppendingPathComponent:@"Run-Rouge-Dungeon.command"];
        if (![[NSFileManager defaultManager] fileExistsAtPath:script.path] || ![[NSWorkspace sharedWorkspace] openURL:script]) {
            [NSApplication sharedApplication];
            NSAlert *alert=[NSAlert new];alert.messageText=@"Rouge Dungeon could not start";
            alert.informativeText=@"Keep this app inside the project's build-macos folder. You can place an alias in the Dock or Applications. The game opens in Terminal and uses your existing saves.";
            [alert runModal];return 1;
        }
    }
    return 0;
}
