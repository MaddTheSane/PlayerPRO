//
//  AXCArrayControllerWithDragAndDrop.h
//  XtrasCreator
//
//  Created by Mac-arena the Bored Zo on 2005-11-09.
//  Copyright 2005 Adium Team. All rights reserved.
//

@protocol AXCArrayControllerValidator;

/*! @class AXCArrayControllerWithDragAndDrop
 *  @brief Forwards table-view data-source methods for drag-and-drop validation
 *   acceptance to another object (the drag validator).
 *  @discussion Set an object as the drag validator, and this array controller
 *   as the table view's data source, and this array controller
 *   will forward any requests from the table view for drag validation
 *   information to that object, and also carry the results back.
 */
@interface AXCArrayControllerWithDragAndDrop : NSArrayController
@property (weak) IBOutlet NSObject<AXCArrayControllerValidator> *dragValidator;

#pragma mark -

- (NSDragOperation) tableView:(NSTableView *)tableView validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)operation;
- (BOOL) tableView:(NSTableView *)tableView acceptDrop:(id <NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation;
@end

@protocol AXCArrayControllerValidator <NSObject>
- (NSDragOperation) tableView:(NSTableView *)tableView validateDrop:(id <NSDraggingInfo>)info proposedRow:(NSInteger)row proposedDropOperation:(NSTableViewDropOperation)operation;
- (BOOL) tableView:(NSTableView *)tableView acceptDrop:(id <NSDraggingInfo>)info row:(NSInteger)row dropOperation:(NSTableViewDropOperation)operation;

@optional;
- (BOOL)tableView:(NSTableView *)aTableView writeRowsWithIndexes:(NSIndexSet *)rowIndices toPasteboard:(NSPasteboard*)pboard;
@end
