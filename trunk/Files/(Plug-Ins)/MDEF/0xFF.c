pascal void main(short msg, MenuHandle whichMenu, Rect *menuRect, Point hitPt, short *itemID );
	#define HIM		14
	#define WIM		15

void GetMDEFRect( Rect *aRect, Rect *menuRect, short row, Boolean HiLo);

void GetMDEFRect( Rect *aRect, Rect *menuRect, short row, Boolean HiLo)
{
	if( !HiLo) aRect->left 	= menuRect->left;
	else aRect->left 	= menuRect->left + WIM + 1;
	
	aRect->right 	= aRect->left + WIM;
	
	aRect->top 		= menuRect->top + row*HIM;
	aRect->bottom 	= aRect->top + HIM;
}

pascal void main(short msg, MenuHandle whichMenu, Rect *menuRect, Point hitPt, short *itemID )
{
	short		j, i, o, mouseItem, mSize, Ypos;
	Str255		str, bstr;
	Rect		tempRect;
	Boolean		HiLo;
	RGBColor	theColor;
	
	TextFont( 0);	TextSize( 0);

	switch (msg)
	{
	case mDrawMsg:
		theColor.red = 56797;
		theColor.green = 56797;
		theColor.blue = 56797;
		
		ForeColor( blackColor);
		RGBBackColor( &theColor);
		
		for( i = 0;  i < 2;  i++)
		{
			for( j = 0;  j < 16;  j++)
			{
				GetMenuItemText( whichMenu, j + 1, str);
				
				GetMDEFRect( &tempRect, menuRect, j, i);
				
				if( i == 1) o = ( (*whichMenu)->menuID-1) & 0x0F;
				else
				{
					o = ( (*whichMenu)->menuID-1) & 0xF0;
					o >>= 4;
				}
				
				if( o == j) ForeColor( redColor);
				else ForeColor( blackColor);
				
				TETextBox( str+1, str[ 0], &tempRect, teJustCenter);
			}
		}
		
		ForeColor( blackColor);
		
		MoveTo( menuRect->left + (menuRect->right - menuRect->left) / 2, menuRect->top);
		LineTo( menuRect->left + (menuRect->right - menuRect->left) / 2, menuRect->bottom-HIM);
		
		MoveTo( menuRect->left, menuRect->bottom-HIM*3);
		LineTo( menuRect->right, menuRect->bottom-HIM*3);
	break;
	
	case mChooseMsg:
		tempRect = *menuRect;
		tempRect.bottom -= HIM*3;
		if( !PtInRect( hitPt, &tempRect))
		{
			mouseItem = 0;
			HiLo = false;
			
			if(Button()) goto TEST;
		}
		else
		{
			if( (*whichMenu)->menuID != 2000)
			{
				*itemID = (*whichMenu)->menuID;
				(*whichMenu)->menuID = 2000;
				if( *itemID > 0)
				{
					GetMDEFRect( &tempRect, menuRect, (*itemID)-1 & 0x000F, true);			InvertRect( &tempRect);
					GetMDEFRect( &tempRect, menuRect, (((*itemID)-1) & 0x00F0)>>4, false);	InvertRect( &tempRect);
				}
				
				// **** Draw bottom ****
				
				GetMenuItemText( whichMenu, 1 + ((*itemID)-1 & 0x000F), bstr);
				GetMenuItemText( whichMenu, 1 + ((((*itemID)-1) & 0x00F0)>>4), str);
				str[ 0]++;
				str[ 2] = bstr[ 1];
				
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM*3 + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				
				{
				short tempValue = *itemID-1;
				NumToString( tempValue, str);
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM*2 + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				}
				
				{
				short tempValue = (char) (*itemID-1);
				NumToString( tempValue, str);
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				}
			}
			
			HiLo = (hitPt.h - menuRect->left) / WIM;
			mouseItem = (hitPt.v - menuRect->top) / HIM;
						
			if( !HiLo)
			{
				mouseItem <<= 4;
				
				if( *itemID != 0) mouseItem += ((*itemID)-1) & 0x000F;
			}
			else
			{
				if( *itemID != 0) mouseItem += ((*itemID)-1) & 0x00F0;
			}
			mouseItem++;
			
			TEST:
			
			if( mouseItem == 0)
			{
				if( *itemID > 0)
				{
					GetMDEFRect( &tempRect, menuRect, (*itemID)-1 & 0x000F, true);			InvertRect( &tempRect);
					GetMDEFRect( &tempRect, menuRect, (((*itemID)-1) & 0x00F0)>>4, false);	InvertRect( &tempRect);
				}
				*itemID = 0;
				
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM*3+1, menuRect->right, menuRect->bottom);
				EraseRect( &tempRect);
			}
			else if( mouseItem != *itemID)
			{
				if( *itemID > 0)
				{
					GetMDEFRect( &tempRect, menuRect, (*itemID)-1 & 0x000F, true);			InvertRect( &tempRect);
					GetMDEFRect( &tempRect, menuRect, (((*itemID)-1) & 0x00F0)>>4, false);	InvertRect( &tempRect);
				}
				
				GetMDEFRect( &tempRect, menuRect, (mouseItem-1) & 0x000F, true);		InvertRect( &tempRect);
				GetMDEFRect( &tempRect, menuRect, ((mouseItem-1) & 0x00F0)>>4, false);	InvertRect( &tempRect);
				
				*itemID = mouseItem;
				
				// **** Draw bottom ****
				
				GetMenuItemText( whichMenu, 1 + ((*itemID)-1 & 0x000F), bstr);
				GetMenuItemText( whichMenu, 1 + ((((*itemID)-1) & 0x00F0)>>4), str);
				str[ 0]++;
				str[ 2] = bstr[ 1];
				
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM*3 + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				
				{
				short tempValue = *itemID-1;
				NumToString( tempValue, str);
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM*2 + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				}
				
				{
				short tempValue = (char) (*itemID-1);
				NumToString( tempValue, str);
				SetRect( &tempRect, menuRect->left, menuRect->bottom-HIM + 1, menuRect->right, menuRect->bottom);
				TETextBox( str+1, str[ 0], &tempRect, teCenter);
				}
			}
		}
	break;
	
	case mSizeMsg:
		mSize = 0;
		for( j = 0;  j< 16;  j++ )
		{
			GetMenuItemText( whichMenu, j, str);
			if( StringWidth( str) > mSize) mSize = StringWidth( str);
		}
		(*whichMenu)->menuWidth  = 2 * mSize + 2;
		(*whichMenu)->menuHeight = 19 * HIM;
	break;


	case mPopUpMsg:
	{
		GDHandle	aH;
		
		
		(*whichMenu)->menuID = *itemID;
		
		GetMouse( &hitPt);
		
		Ypos = (((*itemID)-1) & 0x00F0)>>4;
		
		menuRect->top = hitPt.v - (Ypos * HIM + HIM/2);
		menuRect->left = hitPt.h - WIM/2;
		
		menuRect->right = menuRect->left + 2*WIM + 1;
		menuRect->bottom = menuRect->top + 19*HIM;
		
		aH = GetDeviceList();
		do
		{
			if( aH != NULL)
			{
				if( PtInRect( hitPt, &(*(*aH)->gdPMap)->bounds))
				{
					Rect 		ar = (*(*aH)->gdPMap)->bounds;
					
					if( menuRect->bottom > ar.bottom) 	OffsetRect( menuRect, 0, ar.bottom - menuRect->bottom);
					if( menuRect->top < ar.top)			OffsetRect( menuRect, 0, ar.top - menuRect->top);
					if( menuRect->right > ar.right) 	OffsetRect( menuRect, ar.right - menuRect->right, 0);
					if( menuRect->left < ar.left)		OffsetRect( menuRect, ar.left - menuRect->left, 0);
				}
			}
			
			aH = GetNextDevice( aH);
		}
		while( aH != NULL);
	}
	break;
	}
	
	TextFont( 0);	TextSize( 0);

	
}	/* end of myMenuDefFn */
