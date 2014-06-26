//
//  main.m
//  updateInfoPlist
//
//  Created by C.W. Betts on 10/23/13.
//  Code based on the RubyCocoa script used by the MPlayerX team
//  Original code can be found at http://blog.mplayerx.org/blog/2013/05/10/use-version-number-with-xcode-and-git/
//
//

#import <Foundation/Foundation.h>

int main(int argc, const char * argv[])
{
	@autoreleasepool {
		NSCharacterSet *nlCharSet = [NSCharacterSet newlineCharacterSet];
		NSPipe *soPipe;
		if (argc != 3) {
			fprintf(stderr, "Usage: %s \"path to plist\" kindOfApplication", argv[0]);
			abort();
			return EXIT_FAILURE;
		}
		NSString *plistLocation = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:argv[1] length:strlen(argv[1])];
		NSMutableDictionary *plistDict = [[NSMutableDictionary alloc] initWithContentsOfFile:plistLocation];
		if (!plistDict) {
			fprintf(stderr, "Could not open dictionary!\n");
			return EXIT_FAILURE;
		}
		
		{
			NSTask *versionTask = [[NSTask alloc] init];
			[versionTask setLaunchPath:@"/bin/bash"];
			versionTask.arguments = @[@"version.sh"];
			versionTask.standardOutput = soPipe = [NSPipe new];
			
			[versionTask launch];
			[versionTask waitUntilExit];
		}
		
		NSData *outData = [soPipe.fileHandleForReading readDataToEndOfFile];
		NSString *outString = [[NSString alloc] initWithData:outData encoding:NSUTF8StringEncoding];
		outString = [outString stringByTrimmingCharactersInSet:nlCharSet];
		if ([outString isEqualToString:@"unknown"]) {
			fprintf(stderr, "Version is invalid!\n");
			return EXIT_FAILURE;
		}
		if (strcasecmp(argv[2], "player") == 0) {
			plistDict[@"CFBundleVersion"] = outString;
		} else {
			plistDict[@"CFBundleVersion"] = [NSString stringWithFormat:@"%@.%@", plistDict[@"CFBundleVersion"], outString];
		}
		plistDict[@"CFBundleShortVersionString"] = [NSString stringWithFormat:@"%@.%@", plistDict[@"CFBundleShortVersionString"], outString];
		
		{
			NSTask *hashTask = [[NSTask alloc] init];
			hashTask.launchPath = @"/usr/bin/git";
			hashTask.arguments = @[@"rev-list", @"--max-count=1", @"HEAD"];
			hashTask.standardOutput = soPipe = [NSPipe new];
			
			[hashTask launch];
			[hashTask waitUntilExit];
		}
		
		NSData *commitHash = [soPipe.fileHandleForReading readDataToEndOfFile];
		outString = [[[NSString alloc] initWithData:commitHash encoding:NSUTF8StringEncoding] stringByTrimmingCharactersInSet:nlCharSet];
		plistDict[@"MPXCommitHash"] = outString;
		[plistDict writeToFile:[NSString stringWithFormat:@"%@vers.plist", plistLocation] atomically:NO];
	}
	return EXIT_SUCCESS;
}

