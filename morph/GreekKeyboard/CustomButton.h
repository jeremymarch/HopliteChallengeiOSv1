//
//  CustomButton.h
//
//  Created by Chris Jones.
//  Copyright 2011 Chris Jones. All rights reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#import <UIKit/UIKit.h>

@interface CustomButton : UIButton {
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
@property  BOOL deleteButton;
@property  int device;

- (id)initWithText:(NSString *)text AndDevice: (int) device;
@end