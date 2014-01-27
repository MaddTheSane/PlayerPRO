#include <Carbon/Carbon.h>
#include "Shuddup.h"

void ChangeMapping(ICInstance inst, Handle prefh, long pos, OSType type, unsigned char* extension, ICMapEntry *entry, Boolean add)
{
	OSStatus		err;
	
	entry->flags = 5;	//ICmap_binary_mask + ICmap_data_fork_bit;
	entry->fileType = type;
	entry->fileCreator = 'SNPL';
	entry->postCreator = 'SNPL';
	
	pStrcpy(entry->extension, extension);
	pStrcpy(entry->MIMEType, "\paudio/x-");
	
	entry->MIMEType[ 9] = extension[ 2];
	entry->MIMEType[10] = extension[ 3];
	if (extension[ 0] > 2) entry->MIMEType[11] = extension[ 4];
	if (extension[ 0] > 3) entry->MIMEType[12] = extension[ 5];
	
	entry->MIMEType[ 0] += extension[ 0];
	
	pStrcpy(entry->creatorAppName, "\pPlayerPRO");
	pStrcpy(entry->postAppName, "\pPlayerPRO");
	pStrcpy(entry->entryName, "\pPlayerPRO");
	
	if (add)
	{
		err = ICAddMapEntry(inst, prefh, entry);
		if (err) Debugger();
	}
	else
	{
		err = ICSetMapEntry(inst, prefh, pos, entry);
		if (err) Debugger();
	}
}

void SetInternetMapping(void)
{
	OSStatus		err;
    ICInstance		inst;
    long			startSel, count, i, pos = 0;
    long			endSel;
	ICAttr			attr;
	Handle			prefh;
	ICMapEntry		entry;
	Boolean			madk, madi, madh, mad, xm, mod, s3m, it;
	
	madk = madi = madh = mad = xm = mod = s3m = it = false;
	
    err = ICStart(&inst, 'SNPL');           // Use your creator code if you have one!
    
    if (err == noErr)
    {
//    	err = 1;
        if (err == noErr)
        {
              /*  startSel = 0;
                endSel = urlStr[0];
                
                err = ICLaunchURL(inst, "\p", (char *) &urlStr[1], urlStr[0], &startSel, &endSel);*/
                
                prefh = NewHandle(1);
                
                err = ICFindPrefHandle(inst, kICMapping, &attr, prefh);
                if (err) Debugger();
                
                err = ICCountMapEntries(inst, prefh, &count);
                if (err) Debugger();
                 
                for (i = 1; i < count; i++)
                {
                	 err = ICGetIndMapEntry(inst, prefh, i, &pos, &entry);
                	 if (err) Debugger();
                	 
                	 switch (entry.fileType)
                	 {
                	 	case 'MADK':
                	 		madk = true;
                	 		mad = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.madk", &entry, false);
                	 	break;
                	 	
                	 	case 'MADI':
                	 		madi = true;
                	 		mad = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.madi", &entry, false);
                	 	break;
                	 	
                	 	case 'MADH':
                	 		madh = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.madh", &entry, false);
                	 	break;
                	 	
                	 	case 'MAD ':
                	 		mad = true;
                	 		madk = true;
                	 		ChangeMapping(inst, prefh, pos, 'MADK', "\p.mad", &entry, false);
                	 	break;
                	 	
                	 	case 'XM  ':
                	 		xm = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.xm", &entry, false);
                	 	break;
                	 	
                	 	case 'STrk':
                	 		mod = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.mod", &entry, false);
                	 	break;
                	 	
                	 	case 'S3M ':
                	 		s3m = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.s3m", &entry, false);
                	 	break;
                	 	
                	 	case 'IT  ':
                	 		it = true;
                	 		ChangeMapping(inst, prefh, pos, entry.fileType, "\p.it", &entry, false);
                	 	break;
                	 	
                	 	default:
                	 			 if (EqualString(entry.extension, "\p.xm", false, false))		{xm = true;		ChangeMapping(inst, prefh, pos, 'XM  ', "\p.xm", &entry, false);}
                	 		else if (EqualString(entry.extension, "\p.mod", false, false))		{mod = true;	ChangeMapping(inst, prefh, pos, 'STrk', "\p.mod", &entry, false);}
                	 		else if (EqualString(entry.extension, "\p.STrk", false, false))	{mod = true;	ChangeMapping(inst, prefh, pos, 'STrk', "\p.mod", &entry, false);}
                	 		else if (EqualString(entry.extension, "\p.s3m", false, false))		{s3m = true;	ChangeMapping(inst, prefh, pos, 'S3M ', "\p.s3m", &entry, false);}
                	 		else if (EqualString(entry.extension, "\p.it", false, false))		{it = true;		ChangeMapping(inst, prefh, pos, 'IT  ', "\p.it", &entry, false);}
                	 	break;
                	 }
                }
                
                if (xm == false) 	ChangeMapping(inst, prefh, pos, 'XM  ', "\p.xm", &entry, true);
                if (mad == false) 	ChangeMapping(inst, prefh, pos, 'MADK', "\p.mad", &entry, true);
                if (madi == false) 	ChangeMapping(inst, prefh, pos, 'MADI', "\p.madi", &entry, true);
                if (madk == false) 	ChangeMapping(inst, prefh, pos, 'MADK', "\p.madk", &entry, true);
                if (madh == false) 	ChangeMapping(inst, prefh, pos, 'MADH', "\p.madh", &entry, true);
                if (mod == false) 	ChangeMapping(inst, prefh, pos, 'STrk', "\p.mod", &entry, true);
                if (s3m == false) 	ChangeMapping(inst, prefh, pos, 'S3M ', "\p.s3m", &entry, true);
                if (it == false) 	ChangeMapping(inst, prefh, pos, 'IT  ', "\p.it", &entry, true);
                
                err = ICSetPrefHandle(inst, kICMapping, attr, prefh);
                if (err) Debugger();
        }
  //     #endif
        
        ICStop(inst);
    }
}
