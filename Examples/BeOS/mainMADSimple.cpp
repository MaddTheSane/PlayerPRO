#include <Application.h>
#include <View.h>
#include <Window.h>
#include <Bitmap.h>
#include <fcntl.h>
#include <debugger.h>

#include "RDriver.h"
#include "mainMADSimple.h"

//---------------------------------------------

class MADWindow : public BWindow {

public:
				MADWindow(BRect frame, MADDriverClass *d);
virtual void 	MessageReceived(BMessage *message);
				
virtual	bool	QuitRequested();

MADDriverClass *myMADDriverClass;
};

//---------------------------------------------

class MADView : public BView {

public:
				MADView(BRect frame, char *name); 
virtual	void	AttachedToWindow();
virtual	void	Draw(BRect updateRect);
};

//---------------------------------------------

main()
{
	MyApp* curApp = new MyApp('SNPL');
	
	curApp->Run();
	
	delete curApp;

	return 0;
}
//---------------------------------------------


MyApp::MyApp( ulong signature) : BApplication(signature)
{
	myMADDriverClass = new MADDriverClass();
	
	MADWindow		*aWindow;
	MADView			*aView;
	BRect			aRect;

	// set up a rectangle and instantiate a new window
	aRect.Set(40, 40, 400, 80);
	aWindow = new MADWindow(aRect, myMADDriverClass);
	
	// set up a rectangle and instantiate a new view
	// view rect should be same size as window rect but with left top at (0, 0)
	aRect.OffsetTo(B_ORIGIN);
	aView = new MADView(aRect, "MADText");
	
	// add view to window
	aWindow->AddChild(aView);
	
	// make window visible
	aWindow->Show();
}

MyApp::~MyApp()
{
	delete myMADDriverClass;
}

/* Files dropped on the application will end up here */

void MyApp::RefsReceived (BMessage * message)
{
	entry_ref ref;
    
	// get the ref from the message
    if ( message->FindRef("refs", &ref) == B_OK )
    {
		myMADDriverClass->LoadMusic( &ref, 'MADI', true);
	}
}

//-----------------------------------------------------

MADWindow::MADWindow(BRect frame, MADDriverClass *d)
				: BWindow(frame, "MADDriver", B_TITLED_WINDOW, B_NOT_RESIZABLE)
{
	myMADDriverClass = d;
}

void MADWindow::MessageReceived(BMessage *message)
{
	entry_ref ref;
	
	if (message->what == B_SIMPLE_DATA)
	{
		if( message->FindRef("refs", &ref) == B_OK )
		{
			myMADDriverClass->LoadMusic( &ref, 'MADI', true);
		}
	}
	else BWindow::MessageReceived( message);
}

bool MADWindow::QuitRequested()
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(TRUE);
}

//------------------------------------------------------

MADView::MADView(BRect rect, char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL, B_WILL_DRAW)
{
}


void MADView::AttachedToWindow()
{
	SetFontSize(18);
}


void MADView::Draw(BRect)
{
	MovePenTo(BPoint(10, 30));
	DrawString("Drag a music file on this window!");
}
