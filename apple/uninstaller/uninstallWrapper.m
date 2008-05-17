/*
 *    Lorica source file.
 *    Copyright (C) 2007-2008 OMC Denmark ApS.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

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
