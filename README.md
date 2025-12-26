# Session 2:
- You noted a few things on pointers down in the notebook (green section)
- You did not do the part on debugging


# Session 3:
- No additional book notes
- Watch out for the issue in the "remove" function -> If you remove at the beginning, the next node should not point at list->head, but at NULL.
- Don't forget that when freeing the memory through a for-loop the size of the list is constantly reduced by 1. So do not define a variable "size" before the loop, but rather use dpl_size() at every loop iteration.
- Check out the implementation for dpl_get_reference. Your older code used only one while loop with a counter and condition (current->next != NULL && count <= index).
- Everytime you wish to change something, you need to pass a pointer to it as a parameter to the helper function. Hence why dpl_free() has a double pointer to a list as a parameter, because in that function we wish to free everything, including the pointer to the list.

# Session 4:
- If you need a variable that is of a defined type at compile time, and that will not change during execution, and that outlives the current scope of the function, use global and static (makes it personal to the current module or .c file).
- Do not forget to malloc everytime you create a new pointer to a list, and everytime you create a list node. Also, whenever you do not know the size of whatever the variable you are creating will be, use malloc. Why not make the list and all its nodes just global variables on stack? Because you need to know the sizes of nodes and length of the list at compile time. Also no free() function. Basically, all list manipulations (insertions, deletions, etc...) become harder.
- fread() to read binary files, fscanf() to read text files. Always check first whether files have been opened (not null), and closed. Check whether you are at the end of the file too. You pass pointers to the local variables you want to overwrite because remember, **whenever you want to change the content of memory you have allocated (i.e. change variable value) outside the current scope you need to pass pointers to it.**.
- You often make mistakes in loop bounds. Careful with that.
- Make sure you free all memory. Ctrl F and search malloc, and make sure somewhere you free all malloc'd variables.
