/* by Giacomo Pizzamiglio, c.p. 10620604, matr. 908364
 * aa 2019/2020
 * v22.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/****************
 * DECLARATIONS *
 ****************/
#define MAX_LINE_LENGTH 1025
/** data types **/

typedef struct in {
    char typeChar;
    int param1, param2;
}input;                          // structure to hold decoded user inputs
typedef struct elT {
    struct elT *prev, *next;
    char *ctt;
}line;                          // text type node
typedef struct liT {
    line *head, *tail;
    int len;
}lineList;                      // text type list
typedef struct elC {
    struct elC *prev, *next;
    input origin;
    lineList backup;
    lineList current;
}command;                       // command type node
typedef struct liC {
    command *head, *tail;
    int len;
}commandList;                   // command type list
/** global variables **/
lineList textList;          // list of lines
commandList histList;        // list of entered commands
command *curCmd = NULL;     // last command applied
int numOfScroll = 0;        // number of scrolls to do in commands history (+ = red0, - = undo)
int numOfCurCmd = 0;        // number of applied commands
int numOfPendCmd = 0;       // number of not applied commands
/** functions **/
// list handling
line *create_line();                                            // create new empty node of text type
char *initialize_line(line *tgtLine, char *newCtt);             // initialize text's node's content with given string
lineList create_lines_from_stdin(int numOfLines);               // create set of lines from read inputs
void insert_lines(lineList *dst, line pos, lineList src);       // insert set of lines into main text list
void append_lines(lineList *dst, lineList src);                 // append set of lines at the end of main text list
void append_line(lineList *dst, line *src);                     // append single line at the end of main text list
line remove_lines(lineList *src, lineList tgt);                 // remove set of lines from main text list
lineList search_lines(lineList src, int idx1, int idx2);        // search set of lines in specific position in text list
line *search_line(lineList src, int idx);                       // search line in specific position in text list
command *create_command();                                      // create new empty node of command type
void append_command(commandList *dst, command *src);            // append single command at the end of main command list
void delete_commands(commandList *src, commandList sublist);    // delete set of commands from main command list
/** handle inputs **/
input get_input();                  // get raw input from stdin and decode it
void execute_input(input decIn);    // execute input (only if it is valid)
/** commands **/
void print_lines(input decIn);          // print set of lines from text
void change_lines(input decIn);         // modify or create lines inside text
void undo_change_lines();    // cancel change_lines command
void redo_change_lines(input decIn);    // restore change_lines command
void delete_lines(input decIn);         // delete set of lines inside text
void undo_delete_lines();    // cancel delete_lines command
void redo_delete_lines(input decIn);    // restore change_lines command
void clear_history();                   // delete pending commands in commands' history
int execute_scrolls(int scrollNum);     // execute scrolls in commands' history


/********
 * MAIN *
 ********/
int main() {
    // setup variables
    char cmdType = '\0';
    // main loop (escape with 'q')
    while(cmdType != 'q') {
        input inCmd = get_input();
        execute_input(inCmd);
        cmdType = inCmd.typeChar;
    }
    return 0;
}
/*************
 * FUNCTIONS *
 *************/
/** lists handling **/
line *create_line() {
    line *newLine = (line*)calloc(1, sizeof(line));
    return newLine;
}
char *initialize_line(line *tgtLine, char *newCtt) {
    if(newCtt != NULL) {
        char *tempCtt = tgtLine->ctt;
        tgtLine->ctt = (char*)calloc(strlen(newCtt) + 1, sizeof(char));
        strcpy(tgtLine->ctt, newCtt);
        return tempCtt;
    }
    return newCtt;
}
line *search_line(lineList src, int idx) {
    line *curLine = NULL;
    if(src.len > 0 && idx > 0 && idx <= src.len) {
        // search from the head
        if(idx < src.len / 2) {
            curLine = src.head;
            int cntInt = 1;
            while(curLine != NULL && cntInt < idx) {
                curLine = curLine->next;
                cntInt++;
            }
        }
        else {
            curLine = src.tail;
            int cntInt = src.len;
            while(curLine != NULL && idx < cntInt) {
                curLine = curLine->prev;
                cntInt--;
            }
        }
    }
    return curLine;
}
lineList create_lines_from_stdin(int numOfLines) {
    char newCtt[MAX_LINE_LENGTH];
    lineList newLines;
    newLines.head = NULL;
    newLines.tail = NULL;
    newLines.len = 0;
    for(int i = 0; i < numOfLines; i++) {
        fgets(newCtt, MAX_LINE_LENGTH, stdin);
        line *newLine = create_line();
        initialize_line(newLine, newCtt);
        append_line(&newLines, newLine);
    }
    return newLines;
}
void insert_lines(lineList *dst, line pos, lineList src) {
    line *srcHead = src.head;
    line *srcTail = src.tail;
    // not empty source
    if(srcHead != NULL && srcTail != NULL) {
        line *prevLine = pos.prev;
        line *nextLine = pos.next;
        // empty destination
        if(prevLine == NULL && nextLine == NULL) {
            srcHead->prev = NULL;
            srcTail->next = NULL;
            (*dst).head = srcHead;
            (*dst).tail = srcTail;
        }
            // prefix
        else if(prevLine == NULL && nextLine == (*dst).head) {
            nextLine->prev = srcTail;
            srcTail->next = nextLine;
            srcHead->prev = NULL;
            (*dst).head = srcHead;
        }
            // suffix
        else if(prevLine == (*dst).tail && nextLine == NULL) {
            prevLine->next = srcHead;
            srcHead->prev = prevLine;
            srcTail->next = NULL;
            (*dst).tail = srcTail;
        }
            // internal
        else if(prevLine != NULL && nextLine != NULL) {
            prevLine->next = srcHead;
            srcHead->prev = prevLine;
            nextLine->prev = srcTail;
            srcTail->next = nextLine;
        }
        // update length
        (*dst).len += src.len;
    }
}
void append_lines(lineList *dst, lineList src) {
    line end;
    end.prev = (*dst).tail, end.next = NULL;
    insert_lines(dst, end, src);
}
void append_line(lineList *dst, line *src) {
    lineList srcList;
    srcList.head = src;
    srcList.tail = src;
    srcList.len = 1;
    append_lines(dst, srcList);
}
line remove_lines(lineList *src, lineList tgt) {
    line *srcHead = tgt.head;
    line *srcTail = tgt.tail;
    line pos;
    pos.prev = NULL, pos.next = NULL;
    // not NULL sublist
    if(srcHead != NULL && srcTail != NULL) {
        line *prevLine = srcHead->prev;
        line *nextLine = srcTail->next;
        // entire list
        if(prevLine == NULL && nextLine == NULL) {
            (*src).head = NULL;
            (*src).tail = NULL;
        }
            // prefix
        else if(prevLine == NULL && nextLine != NULL) {
            nextLine->prev = NULL;
            (*src).head = nextLine;
            pos.prev = NULL;
            pos.next = nextLine;
        }
            // suffix
        else if(prevLine != NULL && nextLine == NULL) {
            prevLine->next = NULL;
            (*src).tail = prevLine;
            pos.prev = NULL;
            pos.next = prevLine;
        }
            // internal
        else if(prevLine != NULL && nextLine != NULL) {
            prevLine->next = nextLine;
            nextLine->prev = prevLine;
            pos.prev = prevLine;
            pos.next = nextLine;
        }
        // update length
        (*src).len -= tgt.len;
    }
    return pos;
}
lineList search_lines(lineList src, int idx1, int idx2) {
    lineList result;
    result.head = NULL, result.tail = NULL, result.len = 0;
    if(src.len > 0 && idx1 > 0 && idx1 <= src.len) {
        result.head = search_line(src, idx1);
        if(result.head != NULL) {
            result.tail = result.head;
            result.len++;
            int cntInt = idx1;
            while(cntInt < idx2 && result.tail->next != NULL) {
                result.tail = result.tail->next;
                result.len++;
                cntInt++;
            }
        }
    }
    return result;
}
command *create_command() {
    command *newCmd = (command*)calloc(1, sizeof(command));
    return newCmd;
}
void append_command(commandList *dst, command *src) {
    // if destination is not empty
    if((*dst).len > 0) {
        src->prev = (*dst).tail;
        (*dst).tail->next = src;
        (*dst).tail = src;
        (*dst).len++;
    }
        // if destination is empty
    else if((*dst).len == 0) {
        (*dst).head = src;
        (*dst).tail = src;
        (*dst).len = 1;
    }
}
void delete_commands(commandList *src, commandList sublist) {
    // delete only if tgtNode is not empty
    if(sublist.len > 0) {
        // if there is a previous node link the next to it
        command *prevCmd = sublist.head->prev;
        if(prevCmd != NULL) {
            prevCmd->next = sublist.tail->next;
        }
            // if there is not a previous node set next node as root
        else {
            (*src).head = sublist.tail->next;
        }
        // if there is a nex node link the previous node to it
        command *nextCmd = sublist.tail->next;
        if(nextCmd != NULL) {
            nextCmd->prev = prevCmd;
        }
        else {
            (*src).tail = sublist.head->prev;
        }
        // delete tgtNode
        command *cmd = sublist.head;
        while(cmd != sublist.tail) {
            (*src).len--;
            command *tempCmd = cmd;
            cmd = cmd->next;
            free(tempCmd);
        }
        (*src).len--;
        free(cmd);
    }
}
/** handle inputs **/
input get_input() {
    input decIn;
    char buff[MAX_LINE_LENGTH];
    char *rest;
    scanf("%s", buff);
    getchar();
    decIn.typeChar = buff[strlen(buff) - 1];
    if(decIn.typeChar == 'c' || decIn.typeChar == 'd' || decIn.typeChar == 'p') {
        decIn.param1 = (int)strtol(buff, &rest, 10);
        decIn.param2 = (int)strtol(&rest[1], &rest, 10);
    }
    else if(decIn.typeChar == 'r' || decIn.typeChar == 'u') {
        decIn.param1 = (int)strtol(buff, &rest, 10);
        decIn.param2 = 0;
    }
    else {
        decIn.param1 = 0;
        decIn.param2 = 0;
    }
    return decIn;
}
void execute_input(input decIn) {
    switch(decIn.typeChar) {
        // change lines
        case 'c': {
            numOfScroll = execute_scrolls(numOfScroll);
            clear_history();
            numOfCurCmd++;
            change_lines(decIn);
            break;
        }
            // delete lines
        case 'd': {
            numOfScroll = execute_scrolls(numOfScroll);
            clear_history();
            numOfCurCmd++;
            delete_lines(decIn);
            break;
        }
            // print lines
        case 'p': {
            numOfScroll = execute_scrolls(numOfScroll);
            print_lines(decIn);
            break;
        }
            // undo commands
        case 'u': {
            if(decIn.param1 < numOfCurCmd) {
                numOfScroll -= decIn.param1;
                numOfPendCmd += decIn.param1;
                numOfCurCmd -= decIn.param1;
            }
            else {
                numOfScroll -= numOfCurCmd;
                numOfPendCmd += numOfCurCmd;
                numOfCurCmd = 0;
            }
            break;
        }
            // redo commands
        case 'r': {
            if(decIn.param1 < numOfPendCmd) {
                numOfScroll += decIn.param1;
                numOfCurCmd += decIn.param1;
                numOfPendCmd -= decIn.param1;
            }
            else {
                numOfScroll += numOfPendCmd;
                numOfCurCmd += numOfPendCmd;
                numOfPendCmd = 0;
            }
            break;
        }
        case 'q':
            return;
        default:
            break;
    }
}
/** commands **/
void print_lines(input decIn) {
    int idx1 = decIn.param1;
    int idx2 = decIn.param2;
    line *curLine = search_line(textList, idx1);
    for(int i = idx1; i <= idx2; i++) {
        if(curLine != NULL) {
            printf("%s", curLine->ctt);
            curLine = curLine->next;
        }
        else {
            printf(".\n");
        }
    }
}
void change_lines(input decIn) {
    int idx1 = decIn.param1;
    int idx2 = decIn.param2;
    if(idx1 <= textList.len + 1) {
        command *newCmd = create_command();
        // load new lines
        lineList newLines = create_lines_from_stdin(idx2 - idx1 + 1);
        // if there aren't lines in target position
        if(idx1 == textList.len + 1) {
            newCmd->current.head = textList.tail;       // save current position
            newCmd->current.tail = NULL;
            newCmd->current.len = newLines.len;         // save newLines length
            append_lines(&textList, newLines);
        }
            // if there are lines in target position
        else {
            // remove old lines and store them as backup
            lineList curLines = search_lines(textList, idx1, idx2);
            remove_lines(&textList, curLines);
            newCmd->current.head = curLines.head->prev; // save current position
            newCmd->current.tail = curLines.tail->next;
            newCmd->current.len = newLines.len;         // save newLines length
            line curPos;
            curPos.prev = newCmd->current.head;
            curPos.next = newCmd->current.tail;
            // save current position
            // insert new lines
            insert_lines(&textList, curPos, newLines);
            // store backup
            newCmd->backup = curLines;
        }
        // update history
        char valid = (char)getchar();
        if(valid == '.') {
            newCmd->origin = decIn;
            append_command(&histList, newCmd);
        }
        // move history index
        curCmd = histList.tail;
    }
}
void undo_change_lines() {
    command *cmd = curCmd;
    // load current lines
    line curPos;
    curPos.prev = cmd->current.head;
    curPos.next = cmd->current.tail;
    lineList curLines;
    if(curPos.prev != NULL) {
        curLines.head = curPos.prev->next;
    }
    else {
        curLines.head = textList.head;
    }
    if(curPos.next != NULL) {
        curLines.tail = curPos.next->prev;
    }
    else {
        curLines.tail = textList.tail;
    }
    curLines.len = cmd->current.len;
    cmd->current.len = cmd->backup.len;
    remove_lines(&textList, curLines);
    // if there is a backup, then load and insert it
    if(cmd->backup.len > 0) {
        lineList backupLines = cmd->backup;
        cmd->backup.head = NULL;
        cmd->backup.tail = NULL;
        cmd->backup.len = 0;
        insert_lines(&textList, curPos, backupLines);
    }
    // update backup
    cmd->backup = curLines;
    // move history index
    curCmd = cmd->prev;
}
void redo_change_lines(input decIn) {
    int idx1 = decIn.param1;
    command *cmd = curCmd;
    // load backup lines
    lineList backupLines = cmd->backup;
    cmd->backup.head = NULL;
    cmd->backup.tail = NULL;
    cmd->backup.len = 0;
    // if current lines exist, then load and swap
    if(idx1 <= textList.len) {
        line curPos;
        curPos.prev = cmd->current.head;
        curPos.next = cmd->current.tail;
        lineList curLines;
        if(curPos.prev != NULL) {
            curLines.head = curPos.prev->next;
        }
        else {
            curLines.head = textList.head;
        }
        if(curPos.next != NULL) {
            curLines.tail = curPos.next->prev;
        }
        else {
            curLines.tail = textList.tail;
        }
        curLines.len = cmd->current.len;
        cmd->current.len = backupLines.len;
        remove_lines(&textList, curLines);
        insert_lines(&textList, curPos, backupLines);
        // update backup
        cmd->backup = curLines;
    }
        // if current lines don't exist
    else if(idx1 == textList.len + 1) {
        cmd->current.len = backupLines.len;
        append_lines(&textList, backupLines);
    }
    // move history index
    curCmd = cmd;
}
void delete_lines(input decIn) {
    int idx1 = decIn.param1;
    int idx2 = decIn.param2;
    command *newCmd = create_command();
    newCmd->current.head = NULL;
    newCmd->current.tail = NULL;
    newCmd->current.len = 0;        // save new lines length
    // if target lines exist
    if(idx1 <= textList.len) {
        // load target lines
        lineList curLines = search_lines(textList, idx1, idx2);
        newCmd->current.head = curLines.head->prev;     // save current position
        newCmd->current.tail = curLines.tail->next;
        remove_lines(&textList, curLines);
        // store backup
        newCmd->backup = curLines;
    }
    // update history
    newCmd->origin = decIn;
    append_command(&histList, newCmd);
    // move history index
    curCmd = histList.tail;
}
void undo_delete_lines() {
    command *cmd = curCmd;
    if(cmd->backup.len > 0) {
        // load backup lines
        lineList backupLines = cmd->backup;
        cmd->backup.head = NULL;
        cmd->backup.tail = NULL;
        cmd->backup.len = 0;
        cmd->current.len = backupLines.len;
        // load current position
        line curPos;
        curPos.prev = cmd->current.head;
        curPos.next = cmd->current.tail;
        // insert backup
        insert_lines(&textList, curPos, backupLines);
    }
    // move history index
    curCmd = cmd->prev;
}
void redo_delete_lines(input decIn) {
    int idx1 = decIn.param1;
    command *cmd = curCmd;
    // if target lines exist
    if(idx1 <= textList.len) {
        // load target lines
        lineList curLines;
        if(cmd->current.head != NULL) {
            curLines.head = cmd->current.head->next;
        }
        else {
            curLines.head = textList.head;
        }
        if(cmd->current.tail != NULL) {
            curLines.tail = cmd->current.tail->prev;
        }
        else {
            curLines.tail = textList.tail;
        }
        curLines.len = cmd->current.len;
        cmd->current.len = cmd->backup.len;
        cmd->backup.len = 0;
        remove_lines(&textList, curLines);
        // store removed lines as backup
        curCmd->backup = curLines;
    }
    curCmd = cmd;
}
void clear_history() {
    if(numOfPendCmd > 0) {
        commandList listToDelete;
        listToDelete.head = curCmd != NULL ? curCmd->next : histList.head;
        listToDelete.tail = histList.tail;
        listToDelete.len = 1;
        delete_commands(&histList, listToDelete);
        numOfPendCmd = 0;
    }
}
int execute_scrolls(int scrollNum) {
    int sgn = scrollNum > 0 ? 1 : 0;
    int num = scrollNum < 0 ? -scrollNum : scrollNum;
    int cntInt = 0;
    while(cntInt < num) {
        if(sgn) {
            curCmd = curCmd != NULL ? curCmd->next : histList.head;
            if (curCmd->origin.typeChar == 'c') {
                redo_change_lines(curCmd->origin);
            }
            else if (curCmd->origin.typeChar == 'd') {
                redo_delete_lines(curCmd->origin);
            }
            curCmd = curCmd == NULL ? histList.tail : curCmd;
        }
        else {
            if (curCmd->origin.typeChar == 'c') {
                undo_change_lines();
            }
            else if (curCmd->origin.typeChar == 'd') {
                undo_delete_lines();
            }
        }
        cntInt++;
    }
    return 0;
}