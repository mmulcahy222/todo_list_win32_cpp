This may have been the 3rd or 4th hardest thing I've ever done in my list, aside from the Code Injection program I did in April 2020 [https://mmulcahy222.github.io/#windowsapi_treeview_search] & [https://mmulcahy222.github.io/#treeview_search_gui_cpp] and an experience I had in Magento for an E-Commerce company in 2012 related to Varien Administration Panels for Magento (which is since easy now).

So this is a TODO LIST!!!!!

Oh, but not just ANY To Do list. Not a React.JS To Do list in Javascript, which I could do in my sleep. You see, this is a To Do list done in C++ & Win32 API. No wrappers, no QT, no MFC, no C#, none of that.

This is the insane but dedicated masochistic way of doing it, which is direct Win32 API calls at user32.dll, kernel32.dll, and commctrl32.dll. I did it as a way to experiment, to LEARN, to explore, to get BETTER as a human being. And to do something that would actually serve use to me in the future.

How does one learn when all the details are hidden? I went as far as I'm willing to go, because the only way to drill down more in the details is for people to impractically look at direct kernel calls at ntdll.dll & undocumented functions with the NT suffixed on these API calls, which has no documentation and one has to guess oneselves.....OR to do assembly which is counterproductive except as to just read out of curiosity. Because how far can one go with this unless they're starting to look into electrical engineering, Karnaugh Tables, and Transistors/Logic Gate/Capacitors & Phosphorus/Boson doping on silicon to guide elections and then string theory at the yottometers?

If one can do C++, they can do anything abstract easier, and more abstract in the higher levels, which is why I elected to punish myself this way. Not just any C++ like wrapped QT (paid) or MFC or other wrappers, but the Win32 API. 

The other advantage to Win32 API is that it offers near infinitely more flexibility & capability, at the cost of time. Undoubtedly use cases will crop up where the limits of high-level abstrasct and honestly easier but slower interpreted languages will be made apparent. Proficiency in Win32 will help in this regard.

Let's get to the program itself, shall we?

We have two classes which guide this.

![](images/todo_windows.gif)

| Class | Purpose |
| ------ | ------ |
| AgendaItem | A instance of a line inside of this program |
| AgendaList | A list that contains AgendaItems, which is attached to the window (or HWND parent_hwnd). HWND stands for handle |

AgendaItem contains a function to DRAW the items called draw(HWND&), and a function to calculate the position of the items, using another function called calculate_positions()

| Agenda Item Function | Purpose |
| ------ | ------ |
| draw | Draw the buttons of ONE row/item |
| calculate_positions | calculate the widths & positions of every "control" (boxes/buttons) based off of the ID of the row |

AgendaList contains the inner vector of AgendaItem pointers, and the functions are as follows:

| Agenda List Function | Purpose |
| ------ | ------ |
| create_agenda_item | mainly done in the initialization |
| delete_agenda_item |  |
| add_new_agenda_item_below | When clicking on the + button on the row, add a new item in the vector and set the focus on the new row. Similar to Jupyter cells  |
| redraw | Uses a callback to destroy all the controls in the main parent_hwnd called DestroyChildCallback inside of EnumChildWindows (which recursively gets rid of all of them) which will repaint everything new. Called in both delete_agenda_item & add_new_agenda_item |
| save_into_text_file |  |

There were some regrettable design decisions that may have came about when doing this, with the SetWindowsLongPtr functions & GetWindowsLongPtr functions, and composition within classes. All the classes needed awareness of each other, just like a State Object needs awareness (composition) of the Context object in the State Design pattern, and vice versa. They may have been other better ways to skin this cat, whether it was through parameters, or other API calls. 

Also, the text is SAVE when the focus is removed off of the text boxes. It seems these events could either be detected in WM_COMMAND as a HIWORD(wParam), or in WM_NOTIFY, and this happened to be WM_COMMAND, like so

case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        //clicking on the text edit box
        case 1:
        {
            //HIWORD(wParam) is the control-defined notification code according to the docs at https://docs.microsoft.com/en-us/windows/win32/menurc/wm-command
            //The notification codes for this edit box is located in https://docs.microsoft.com/en-us/windows/win32/controls/en-killfocus
            //I was unable to use WM_NOTIFY, it only worked with WM_COMMAND

A lot more of the code is explained in the attached picture below in comments!

If you have any questions, please ask!

todo_windows.cpp

![](images/todo_windows_code.png)