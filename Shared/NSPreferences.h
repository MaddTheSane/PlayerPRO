// Private classes from the AppKit framework; used by Safari and Mail.

#import <Cocoa/Cocoa.h>

@protocol NSPreferencesModule <NSObject>
- (BOOL) preferencesWindowShouldClose;
- (BOOL) moduleCanBeRemoved;
- (void) moduleWasInstalled;
- (void) moduleWillBeRemoved;

- (void) didChange;
- (void) initializeFromDefaults;
- (void) willBeDisplayed;
- (void) saveChanges;
@property (readonly) BOOL hasChangesPending;
- (NSImage *) imageForPreferenceNamed:(NSString *) name;
- (NSBox *) viewForPreferenceNamed:(NSString *) name;
@end

@interface NSPreferences : NSObject {
	NSWindow *_preferencesPanel;
	NSBox *_preferenceBox;
	NSMatrix *_moduleMatrix;
	NSButtonCell *_okButton;
	NSButtonCell *_cancelButton;
	NSButtonCell *_applyButton;

	NSMutableArray *_preferenceTitles;
	NSMutableArray *_preferenceModules;

	NSMutableDictionary *_masterPreferenceViews;
	NSMutableDictionary *_currentSessionPreferenceViews;

	NSBox *_originalContentView;
	BOOL _isModal;

	float _constrainedWidth;
	id _currentModule;
	void *_reserved;
}
+ (NSPreferences*) sharedPreferences;
+ (void) setDefaultPreferencesClass:(Class) class;
+ (Class) defaultPreferencesClass;

- (void) addPreferenceNamed:(NSString *) name owner:(id) owner;

@property (readonly) NSSize preferencesContentSize;

- (void) showPreferencesPanel;
- (void) showPreferencesPanelForOwner:(id) owner;
- (NSInteger) showModalPreferencesPanelForOwner:(id) owner;
- (NSInteger) showModalPreferencesPanel;

- (IBAction) ok:(id) sender;
- (IBAction) cancel:(id) sender;
- (IBAction) apply:(id) sender;

@property (readonly, copy) NSString *windowTitle;
@property (readonly) BOOL usesButtons;
@end

@interface NSPreferencesModule : NSObject <NSPreferencesModule> {
	IBOutlet NSBox *_preferencesView;
	NSSize _minSize;
	BOOL _hasChanges;
	void *_reserved;
}
+ (id) sharedInstance;
@property (readonly, copy) NSString *preferencesNibName;

- (void) setPreferencesView:(NSView *) view;

- (NSView *) viewForPreferenceNamed:(NSString *) name;
- (NSImage *) imageForPreferenceNamed:(NSString *) name;

- (NSString *) titleForIdentifier:(NSString *) identifier;

@property (readonly) BOOL hasChangesPending;
- (void) saveChanges;
- (void) willBeDisplayed;
- (void) initializeFromDefaults;
- (void) didChange;

@property NSSize minSize;
@property (readonly, getter=isResizable) BOOL resizable;
@end
