//
//  uninstallWrapper.m
//  Uninstall Lorica
//
//  Created by Rasmus Maagaard on 2/19/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "uninstallWrapper.h"

@implementation uninstallWrapper

- (IBAction) execute: sender {
	NSLog(@"Executing Lorica uninstall tool");
	
	NSString *message = [NSString string];
	AuthorizationRef authorizationRef = NULL;
	NSString *command = [[NSBundle mainBundle] pathForResource:@"uninstall.tool" ofType:nil];	
	
	[uninstall setEnabled: NO];
	
	int create, execute;
	create = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);
	execute = AuthorizationExecuteWithPrivileges(authorizationRef, [command cStringUsingEncoding:  NSASCIIStringEncoding] , kAuthorizationFlagDefaults, NULL, NULL);

	if(create || execute) {
		message = @"Unable to execute uninstall tool with privileges.";
		goto exit;
	}
		
	int status, pid;
	pid = wait(&status);
	
	if(pid == -1 || ! WIFEXITED(status)) {
		message = @"The uninstall tool failed to execute.";
	} else {
		switch (WEXITSTATUS(status)) {
			case 0:
				message = @"Lorica was succesfully uninstalled.";
				break;
			case 1:
				message = @"The uninstall tool was not started with administrator rights.";
				break;
			case 2:
				message = @"Lorica is not installed!";
				break;
			default:
				message = @"The uninstall tool exited with an unexpected error code.";
				break;
		}
	}
	
	exit:
	NSBeginAlertSheet(@"Information", nil, nil, nil, window, self, @selector(sheetDidEnd:returnCode:contextInfo:), nil, nil, message);
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	[NSApp terminate: nil];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication *) theApplication {
	return YES;
}

- (void) awakeFromNib {
	[window makeKeyAndOrderFront: nil];
}

@end
