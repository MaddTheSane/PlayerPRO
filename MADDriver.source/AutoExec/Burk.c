#include "Exemple.h"

void Burk( short fRefnum)
{
	if (fRefnum == 0) return;
	else FSClose( fRefnum);
}
