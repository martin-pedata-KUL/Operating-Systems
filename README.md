# Session 2:
- You noted a few things on pointers down in the notebook (green section)
- You did not do the part on debugging


# Session 3:
- No additional book notes
- Watch out for the issue in the "remove" function -> If you remove at the beginning, the next node should not point at list->head, but at NULL.
- Don't forget that when freeing the memory through a for-loop the size of the list is constantly reduced by 1. So do not define a variable "size" before the loop, but rather use dpl_size() at every loop iteration.
- Check out the implementation for dpl_get_reference. Your older code used only one while loop with a counter and condition (current->next != NULL && count <= index).
- Everytime you wish to change something, you need to pass a pointer to it as a parameter to the helper function. Hence why dpl_free() has a double pointer to a list as a parameter, because in that function we wish to free everything, including the pointer to the list.
