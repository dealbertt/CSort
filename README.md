# CSort

Breakdown of the project so far:  
- Object/Class ArrayItem with fields/properties such as: value of value_type(in my case might just be uint32_t or something like that)  
    - OnAccess method which plays a sound depending on the  value of said ArrayItem.  
    - OnComparison method to compares different values, and also to increase the compare count, aswell as playing sounds depending on the value of both elements that are being compared  
    - Method to return the value of said ArrayItem  
  
- SortArray class:
    - contains a vector of ArrayItems  
    - Max value of the array for scaling display (makes a lot of sense)  
    - struct Access with fields: index, color, sustain and priority (not really sure about the last two fields)  
    - Two variables of type access to keep track of the last swap  
    - bool isSorted to indicate if the array has been sorted   
    - pointer to delay function (have to look deeper into this)  
    - public mutex for accesses    



Uses wx in C++ for displaying, i guess i have to use sdl for this one  

Still not too sure about how the sync between the sorting, the visual representation of the sorting and the audio is done  

He launches a thread in charge of executing the actual sorting algorithm   


