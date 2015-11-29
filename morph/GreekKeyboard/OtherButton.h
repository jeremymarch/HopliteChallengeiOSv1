//
//  OtherButton.h
//  morph
//
//  Created by Jeremy on 11/7/15.
//  Copyright © 2015 Jeremy March. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface OtherButton : UIButton {
    BOOL _selected;
    BOOL _toggled;
    BOOL _delete;
    int _device;
    
    
    float width; //includes left and right padding
    float height; //includes top and bottom padding
    float hPadding;
    float vPadding;
    float topMargin;
    float buttonDownAddHeight;
}

@property  BOOL selected;
@property  BOOL toggled;
@property  BOOL diacriticButton;
@property  BOOL mfButton;
@property  BOOL mfPressed;
@property  BOOL submitButton;
@property  int device;
@property (nonatomic, retain) NSString *font;

- (id)initWithText:(NSString *)text AndDevice: (int) device AndFont:(NSString*)font;
@end
