#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>
#import <CoreGraphics/CoreGraphics.h>
int main(int argc,const char **argv){
    @autoreleasepool {
        if(argc!=2)return 1;
        NSString *glyphs=@"♚♕♙⛐ඞ⛇⚛✺✾☀❂✡⛂⛃⛀⛁⏣⌘@✯✠☫☣☠◉⛒♨☺☻◕℗♾⚙⍟☮◎♞♜♟◆●✦✹✚♠▲■◈";
        NSString *folder=[NSString stringWithUTF8String:argv[1]];
        [[NSFileManager defaultManager] createDirectoryAtPath:folder withIntermediateDirectories:YES attributes:nil error:nil];
        for(NSUInteger i=0;i<glyphs.length;i++){
            CTFontRef font=CTFontCreateWithName(CFSTR("Helvetica"),80,NULL);
            CGColorRef white=CGColorCreateGenericGray(1,1);
            NSAttributedString *text=[[NSAttributedString alloc] initWithString:[glyphs substringWithRange:NSMakeRange(i,1)] attributes:@{(id)kCTFontAttributeName:(id)font,(id)kCTForegroundColorAttributeName:(id)white}];
            CTLineRef line=CTLineCreateWithAttributedString((CFAttributedStringRef)text);
            unsigned char pixels[128*128]={0};CGColorSpaceRef space=CGColorSpaceCreateDeviceGray();
            CGContextRef context=CGBitmapContextCreate(pixels,128,128,8,128,space,(CGBitmapInfo)kCGImageAlphaNone);
            CGContextSetTextPosition(context,12,25);CTLineDraw(line,context);
            NSString *file=[folder stringByAppendingPathComponent:[NSString stringWithFormat:@"%lu.raw",(unsigned long)i]];
            [[NSData dataWithBytes:pixels length:sizeof(pixels)] writeToFile:file atomically:YES];
            CGContextRelease(context);CGColorSpaceRelease(space);CFRelease(line);CFRelease(font);CGColorRelease(white);
        }
    }return 0;
}
