#include <iostream>
#include <Windows.h>
#include <Commctrl.h>
#include <math.h>
#include <vector>
#include <fstream>

using namespace std;
HHOOK _hook;
HMENU menu;

class AgendaList; // let the compiler know such a class will be defined, namely inside of AgendaItem which contains a pointer to an AgendaList

using namespace std;
//AgendaList prototypeg

int window_width = 400;
int window_height = 400;
int global_index = 0;
string agenda_items_filename = "agenda_items.txt";

const HFONT levi_windows_font = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Levi Windows"));

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestroyChildCallback( HWND , LPARAM);

//resets every single control, for the add & delete operations
BOOL CALLBACK DestroyChildCallback( HWND   hwnd, LPARAM lParam)
{
  if (hwnd != NULL) {
    DestroyWindow(hwnd);
  }
  return TRUE;
}



class AgendaItem
{
    //get a static class variable to keep track of the number of agenda items
private:
    int id;
    static int last_id;
    
    string text;

    AgendaList *parent_list;
    int agenda_item_margin = 10;
    int agenda_item_padding = 20;
    int agenda_item_height = 60;
    int agenda_item_x = 0;
    double agenda_item_y = id * agenda_item_height;
    //I don't know why i had to do the (2 * agenda_item_padding) twice
    int agenda_item_width = window_width - (2 * agenda_item_margin) - (2 * agenda_item_margin);
    int groupbox_x;
    int groupbox_y;
    int groupbox_height;
    int groupbox_width;
    int text_box_x;
    int text_box_y;
    int text_box_height;
    int text_box_width;
    int add_button_x;
    int add_button_y;
    int add_button_height;
    int add_button_width;
    int close_button_x;
    int close_button_y;
    int close_button_height;
    int close_button_width;

    HWND groupbox;
    HWND textbox;
    HWND button_add;
    HWND button_delete;
    vector<HWND> agenda_item_controls;

public:
    AgendaItem(HWND& parent_hwnd, string text_in)
    {
        this->text = text_in;
        this->id = last_id;
        last_id++;
        calculate_positions();
        draw(parent_hwnd);
        ///get the userdata from a previous call of SetWindowLongPtr(parent_hwnd, GWLP_USERDATA, (LONG_PTR)this);
        ///that will represent the Parent Agenda List
        this->parent_list = (AgendaList*)GetWindowLongPtr(parent_hwnd, GWLP_USERDATA);
    }
    void calculate_positions()
    {
        agenda_item_y = id * agenda_item_height;
        groupbox_x = round(agenda_item_x + agenda_item_margin);
        groupbox_y = agenda_item_y + agenda_item_margin;
        groupbox_height = round(agenda_item_height);
        groupbox_width = round(agenda_item_width);
        text_box_x = round(agenda_item_x + agenda_item_padding);
        text_box_y = round(agenda_item_y + agenda_item_margin + agenda_item_padding);
        text_box_height = round(agenda_item_height * 0.33);
        text_box_width = round(agenda_item_width * 0.6);
        add_button_x = round(agenda_item_width * 0.75);
        add_button_y = round(agenda_item_y + agenda_item_margin + agenda_item_padding);
        add_button_height = round(agenda_item_height * 0.33);
        add_button_width = round(agenda_item_width * 0.07);
        close_button_x = round(agenda_item_width * 0.85);
        close_button_y = round(agenda_item_y + agenda_item_margin + agenda_item_padding);
        close_button_height = round(agenda_item_height * 0.33);
        close_button_width = round(agenda_item_width * 0.07);
    }
    void print_all_values_in_calculate_positions()
    {
        cout << "agenda_item_x: " << agenda_item_x << endl;
        cout << "agenda_item_y: " << agenda_item_y << endl;
        cout << "agenda_item_height: " << agenda_item_height << endl;
        cout << "agenda_item_width: " << agenda_item_width << endl;
        cout << "groupbox_x: " << groupbox_x << endl;
        cout << "groupbox_y: " << groupbox_y << endl;
        cout << "groupbox_height: " << groupbox_height << endl;
        cout << "groupbox_width: " << groupbox_width << endl;
        cout << "text_box_x: " << text_box_x << endl;
        cout << "text_box_y: " << text_box_y << endl;
        cout << "text_box_height: " << text_box_height << endl;
        cout << "text_box_width: " << text_box_width << endl;
        cout << "add_button_x: " << add_button_x << endl;
        cout << "add_button_y: " << add_button_y << endl;
        cout << "add_button_height: " << add_button_height << endl;
        cout << "add_button_width: " << add_button_width << endl;
        cout << "close_button_x: " << close_button_x << endl;
        cout << "close_button_y: " << close_button_y << endl;
        cout << "close_button_height: " << close_button_height << endl;
        cout << "close_button_width: " << close_button_width << endl;
        cout << "------------------------------------" << endl;
    }
    void draw(HWND& parent_hwnd)
    {
        
        calculate_positions();
        //** NOTE. For some reason, I had to have the buttons in the groupbox have a parent that is not the groupbox, but the parent of the groupbox. The buttons weren't showing up. **//


        //define the text string to display for the groupbox
        string groupbox_text = "Agenda Item #" + to_string(id);

        //define group box with different id's so that we can identify them later
        groupbox = CreateWindowEx(0,
            TEXT("BUTTON"),
            TEXT(groupbox_text.c_str()),
            WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
            groupbox_x,
            groupbox_y,
            groupbox_width,
            groupbox_height,
            parent_hwnd,
            (HMENU)id,
            GetModuleHandle(NULL),
            NULL);

        textbox = CreateWindow("EDIT",                                                       //
            text.c_str(),                                                 // text
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | WS_GROUP, // styles
            text_box_x,                                                   // x
            text_box_y,                                                   // y
            text_box_width,                                               // width
            text_box_height,                                              // height
            parent_hwnd,                                                  // parent
            (HMENU)1,                                                     // menu
            NULL,                                                         // hInstance
            NULL);                                                        // no extra parameters

        button_add = CreateWindow("Button", //
         "+",  // text
         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // styles
         add_button_x, // x
         add_button_y, // y
         add_button_width, // width
         add_button_height, // height
         parent_hwnd, // parent
         (HMENU)2, // menu
         NULL, // hInstance 
         NULL); // no extra parameters

        button_delete = CreateWindow("Button", //
         "x",  // text
         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // styles
         close_button_x, // x
         close_button_y, // y
         close_button_width, // width
         close_button_height, // height
         parent_hwnd, // parent
         (HMENU)3, // menu
         NULL, // hInstance 
         NULL); // no extra parameters


        //Set the UserData for the buttons with the AgendaItem of the AgendaList, via the "this" variable. If I had to do multiple variables, it was possible to put them in a struct. If you know other ways to do this, please let me know.
        //I researched CreateDialogParam, but I hit an impasse
        //
        //in Short, this links the buttons to the AgendaItem
        SetWindowLongPtr(textbox, GWLP_USERDATA, (LONG_PTR)this);
        SetWindowLongPtr(button_add, GWLP_USERDATA, (LONG_PTR)this);
        SetWindowLongPtr(button_delete, GWLP_USERDATA, (LONG_PTR)this);

        //every button uses the font in the variable "font"
        SendMessage(groupbox, WM_SETFONT, (WPARAM)levi_windows_font, TRUE);
        SendMessage(textbox, WM_SETFONT, (WPARAM)levi_windows_font, TRUE);
        SendMessage(button_add, WM_SETFONT, (WPARAM)levi_windows_font, TRUE);
        SendMessage(button_delete, WM_SETFONT, (WPARAM)
        levi_windows_font, TRUE);

        //fill the vector with the buttons
        agenda_item_controls.push_back(groupbox);
        agenda_item_controls.push_back(textbox);
        agenda_item_controls.push_back(button_add);
        agenda_item_controls.push_back(button_delete);


    }
    int get_id()
    {
        return id;
    }
    string get_text()
    {
        return text;
    }
    void set_id(int new_id)
    {
        id = new_id;
    }
    HWND get_textbox()
    {
        return textbox;
    }
    void set_text(string text_in)
    {
        text = text_in;
    }
    vector<HWND> get_agenda_item_controls()
    {
        return agenda_item_controls;
    }
};


//get a static class variable to keep track of the number of agenda items
int AgendaItem::last_id = 0;




class AgendaList
{
private:
    HWND parent_hwnd;
    vector<AgendaItem*> agenda_items;
public:
    AgendaList(HWND& parent_hwnd)
    {
        cout << "AgendaList this: " << this << endl;
        //set userdata to this window
        cout << "this: " << this << endl;
        this->parent_hwnd = parent_hwnd;
    }
    //mainly done in initial creation
    AgendaItem* create_agenda_item(HWND &parent_hwnd, string text_in)
    {
        AgendaItem* item = new AgendaItem(parent_hwnd, text_in);
        agenda_items.push_back(item);
        return item;
    }
    void delete_agenda_item(AgendaItem* agenda_item)
    {
        //error if trying to go out of bounds
        if(agenda_items.size() <= 1)
        {
            MessageBox(parent_hwnd, "You must have at least one agenda list item.", "Error", MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        //find the item in the vector and delete it
        for (int i = 0; i < agenda_items.size(); i++)
        {
            if (agenda_items[i] == agenda_item)
            {
                agenda_items.erase(agenda_items.begin() + i);
            }
        }
    }
    AgendaItem* add_new_agenda_item_below(AgendaItem* agenda_item)
    {
        int id = agenda_item->get_id();
        //new AgendaItem
        AgendaItem *new_agenda_item = new AgendaItem(parent_hwnd, "");
        //set the id of the new AgendaItem to the id of the old one + 1
        new_agenda_item->set_id(id + 1);
        //insert the new AgendaItem into the vector
        agenda_items.insert(agenda_items.begin() + id + 1, new_agenda_item);
        //raise the ID's of all the other AgendaItems after the new one
        for (int i = id + 1; i < agenda_items.size(); i++)
        {
            agenda_items[i]->set_id(agenda_items[i]->get_id() + 1);
        }
        return new_agenda_item;
    }
    int get_num_agenda_items()
    {
        return agenda_items.size();
    }
    void redraw(HWND& parent_hwnd)
    {
        //clears the window
        RECT rect;
        GetClientRect(parent_hwnd, &rect);
        FillRect(GetDC(parent_hwnd), &rect, (HBRUSH)CreateSolidBrush(RGB(240, 240, 240)));
        //Destroy all the child windows in the parent window (Parent HWND Handle)
        EnumChildWindows(parent_hwnd, DestroyChildCallback, (LPARAM)&parent_hwnd);
        //Set the new ids, AND draw the new buttons
        int index = 0;
        for (auto &item : agenda_items)
        {
            item->set_id(index++);
            item->draw(parent_hwnd);
        }    
    }
    vector<AgendaItem*> get_agenda_items()
    {
        return agenda_items;
    }
    BOOL save_into_text_file()
    {
        //open the file
        ofstream file;
        file.open(agenda_items_filename);
        //if the file is not open, return false
        if (!file.is_open())
        {
            return FALSE;
        }
        //write the text from each AgendaItem into the file
        for (auto &item : agenda_items)
        {
            file << item->get_text() << endl;
        }
        //close the file
        file.close();
        return TRUE;
    }
};



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CREATE:
        {
            AgendaList* agenda_list = new AgendaList(hwnd);   
            //Initialize via filename
            ifstream file;
            file.open(agenda_items_filename);
            //FILE DOES NOT EXIST
            if (!file.is_open())
            {
                AgendaItem *a1 = agenda_list->create_agenda_item(hwnd, "test");
                AgendaItem *a2 = agenda_list->create_agenda_item(hwnd, "grell");
                AgendaItem *a3 = agenda_list->create_agenda_item(hwnd, "grain");
            }
            //FILE DOES EXIST
            else
            {
                string line;
                while (getline(file, line))
                {
                    agenda_list->create_agenda_item(hwnd, line);
                }
            }
            //attack agenda_list to the parent main window
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)agenda_list);
            break;
        }
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
                if(HIWORD(wParam) == EN_KILLFOCUS)
                {
                    HWND textbox = (HWND)lParam;
                    //get text
                    char buffer[256];
                    GetWindowText(textbox, buffer, 256);
                    //get the agenda list
                    AgendaList* agenda_list = (AgendaList*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                    //get the current agenda item
                    AgendaItem* agenda_item = (AgendaItem*)GetWindowLongPtr(textbox, GWLP_USERDATA);
                    //set the text of the current agenda item
                    agenda_item->set_text(buffer);
                    agenda_list->save_into_text_file();
                }
                break;
            }
            //add agenda item
            case 2:
            {
                //get the button
                HWND button = (HWND)lParam;
                //get the userdata from the button (agenda item)
                AgendaItem* agenda_item = (AgendaItem*)GetWindowLongPtr(button, GWLP_USERDATA);
                //get agenda list (attached to this HWND durig WM_CREATE)
                AgendaList *agenda_list = (AgendaList *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                cout << "size: " << agenda_list->get_num_agenda_items() << endl;
                AgendaItem* new_agenda_item = agenda_list->add_new_agenda_item_below(agenda_item);
                agenda_list->redraw(hwnd);
                SetFocus(new_agenda_item->get_textbox());
                cout << "size: " << agenda_list->get_num_agenda_items() << endl;
                break;
            }
            //deleting in an agenda item
            case 3:
            {
                //get the button
                HWND button = (HWND)lParam;
                //get the userdata from the button (attached to the agenda item)
                AgendaItem* agenda_item = (AgendaItem*)GetWindowLongPtr(button, GWLP_USERDATA);
                //get agenda list (attached to this HWND durig WM_CREATE)
                AgendaList* agenda_list = (AgendaList*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                cout << "size: " << agenda_list->get_num_agenda_items() << endl;
                agenda_list->delete_agenda_item(agenda_item);
                agenda_list->redraw(hwnd);
                cout << "size: " << agenda_list->get_num_agenda_items() << endl;
                break;      

            }
            }
            break;
        }
        //change the colors of buttons
        case WM_CTLCOLORBTN:
        {
            //get the button
            HWND button = (HWND)lParam;
            //get the userdata from the button (attached to the agenda item)
            AgendaItem* agenda_item = (AgendaItem*)GetWindowLongPtr(button, GWLP_USERDATA);
            //get the color of the button
            COLORREF color = RGB(128,255,128);
            //set the color of the button
            SetBkColor((HDC)wParam, color);
            return (LRESULT)CreateSolidBrush(color);
        }
        case WM_LBUTTONDOWN:
            cout << "Left button down" << endl;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
HWND create_window(int width, int height)
{
    HWND hwnd;
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //the typical gray background color
    HBRUSH gray;
    gray = CreateSolidBrush(RGB(240, 240, 240));
    wc.hbrBackground = gray;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Class";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        "Class", //
        "Agenda",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        width, height,
        NULL,
        NULL, //
        GetModuleHandle(NULL), NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return NULL;
    }
    return hwnd;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd;
    hwnd = create_window(window_width, window_height);
    //divide three sections of that window  
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (IsDialogMessage(hwnd, &msg) == 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}