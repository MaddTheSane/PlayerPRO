
/* MODList.c */
void DoHelpMODList(short **, short *);
short MLAddRow(short, short);
void MLDelRow(short, short);
void DrawMODListItem(short);
pascal short MyOMatchNextList(Ptr, Ptr, short, short);
pascal short MyOMatchNextAlphabetically(Ptr, Ptr, short, short);
void MyResetTypeSelection(void);
void MyKeySearchInList(short);
void EraseMODInfo(void);
void DoGrowMODList(DialogPtr);
OSErr PathNameFromDirIDTRUE(long, short, StringPtr);
void SaveMyMODListSTCf(FSSpec);
void SaveMyMODList(FSSpec);
void SaveMODList(void);
void FindName(Str255, Str255);
void OpenMODListSTCf(FSSpec);
void OpenMODList2(Str255, short);
//Boolean PLSearch(Ptr, short, short, PointPtr , PrivateList *);
short IsAlreadyHere(Str255);
void NMoveMusicCell(short, short);
void MODListSelectThisMusic(Str255);
void AddMODList(Boolean, Str255, short, long);
void UpdateMODListInfo(void);
void DrawNumberMusic(void);
void UpdateMODListWindow(DialogPtr);
Boolean IsMyTypeMODList(DragReference);
pascal OSErr MyTrackingMODList(short, WindowPtr, void *, DragReference);
pascal OSErr MyReceiveMODList(WindowPtr, void *, DragReference);
Boolean NDragMusicFile(RgnHandle, EventRecord *, Point, short);
void DragMODSelect(void);
void CreateMODListWindow(void);
void CloseMODListWindow(void);
void DrawPathName(void);
void ShowMusicInfo(Str255, short, long, short);
OSErr DoShowInfo(Point);
Boolean OpenFirst2(short);
void DoChangeMODName(void);
void UpdateFileInformations(DialogPtr);
short FindTYPE(Str255, MenuHandle);
OSType GetTYPE(short, MenuHandle);
void FileInfoIcon(DialogPtr);
void FileInformations(short);
void SortMusicList(void);
void DoItemPressMODList(short, DialogPtr);
void ClearMODList(void);
void SetMODListWindowName(void);
OSErr CheckFileAvailable(short);
void DoKeyPressMODList(short);
void DoLoadOtherMusic(Boolean);
void SetcurMusicListFile(Boolean);
OSErr GereMusicListChanged(void);
void DoNullMODList(void);
void ReconstructShowInfo(void);
void COPYMODList(void);
void PASTEMODList(void);