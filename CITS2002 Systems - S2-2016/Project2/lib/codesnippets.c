void appendList(DIR_ELEMENT * dirList, char *fileName, struct stat fileData)
{
    if(dirList == NULL) {           							// IF empty list...
        dirList = malloc( sizeof(DIR_ELEMENT) );
        if(dirList != NULL) {
			dirList->fileName	=  fileName;
			dirList->next		=  NULL;
        }
		else {
            perror("append_item");
            exit(EXIT_FAILURE);
		}
    }
    else {                       								// Else if list is not empty, append to back of list
        DIR_ELEMENT *dir_Ptr = dirList;

        while(dir_Ptr->next != NULL) { 							// Move pointer until next is null (end of list)
            dir_Ptr  =  dir_Ptr->next;
        }
        dir_Ptr->next = malloc( sizeof(DIR_ELEMENT) );
        if(dir_Ptr->next == NULL) {
            perror("append_item");
            exit(EXIT_FAILURE);
        }
        dir_Ptr				=  dir_Ptr->next;   					// append after the last item
        dir_Ptr->fileName	=  fileName;
		dir_Ptr->fileData	= fileData;
        dir_Ptr->next		=  NULL;
    }
}

// TO GO INTO SORT.C / EXEC.C

static bool ignoreCheck(char const *name)
{
  return ( (ignore_mode != IGNORE_MINIMAL && name[0] == '.'))
}

