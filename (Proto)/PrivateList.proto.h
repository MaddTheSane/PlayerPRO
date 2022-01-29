
/* PrivateList.c */
void PLCheckSelect(PrivateList *);
void PLRect(Rect *, Point, PrivateList *);
void PLUpdateItem(Point, PrivateList *);
void PLUpdate(PrivateList *);
void PLScrollInt(short, short, long, PrivateList *);
void PLSetMyIntList(PrivateList *);
pascal void PLactionProcPartition(ControlHandle, short);
short PLGetCtlValue(ControlHandle);
void PLSetCtlValue(ControlHandle, short);
short PLGetMaxYValue(PrivateList *);
short PLGetMaxXValue(PrivateList *);
Boolean PLConvertPoint(Point *, PrivateList *);
Boolean PLGetSelect(Point *, PrivateList *);
void PLGetSelectRect(Rect *, PrivateList *);
void PLSetSelect(short, short, short, short, PrivateList *);
void PLAutoScroll(PrivateList *);
void PLSetControl(PrivateList *);
Boolean PLClick(Point, short, PrivateList *);
void PLScroll(Point, PrivateList *);
void PLDoArrows(short, PrivateList *);
