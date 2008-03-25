//
//  uninstallWrapper.h
//  Uninstall Lorica
//
//  Created by Rasmus Maagaard on 2/19/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface uninstallWrapper : NSObject {
	IBOutlet id window;
	IBOutlet id uninstall;
	IBOutlet id authorize;
}

- (IBAction) execute: sender;

@end
