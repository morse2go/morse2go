/* m2g.h -- header file
 * see morse2go.org for more into
 *
This work is licensed 2014 by Jim Wroten ( www.jimwroten.com ) under a Creative 
Commons Attribution-ShareAlike 4.0 International License. For more information 
about this license, see www.creativecommons.org/licenses/by-sa/4.0/ 
-- Basically, you can use this software for any purpose for free, 
as long as you say where you got it and that if you modify it, you don't remove
any lines above THIS line. 

------------------ Release History  ---------------------------

9/6/14 Initial Release 1.0 
  - basic functionality - user input of 1 or 2 button code
  - morse2go code table input from Google docs - support for mcode, scode and pcode
  - user input of timing values. persistent pcode values saved in usr_parm.csv file on Micro SD
  - basic checking of user input parmameters (pcode) upper and lower limits, 10% max change limits 
  - /U undo - removes last timing parameter entered
  - /D delete - deletes the usr_parm.csv file and reboots. This brings in factory default timings
  - /L list - list current timing paramters 
  - TODO: user input of short codes
  -      user input of short codes

*/

#define MAXDD 6
#define MAXSCODE_TXT 80
#define MAXWORD_TXT 200
#define MAXWORDS 40
#define MAXWORD 15
#define MAXMCODES 52
#define MAXSCODES 40
#define NPARMS 6
#define BUFSZ 1000
#define BUFMCODE 1000
#define BUFSCODE 1200
#define BUFPCODE 100
#define SIZPWORD 15
#define SIZMESG 100
#define NCOL 19
#define DEBUG 0
#define PRVC 2
#define PRFN 3
#define CODE "CODE.CSV"
#define USR_PARM "USR_PARM.CSV"
#define DEBOUNCEDELAY 50 
#define BUFFPIXEL 20
#define LONGPRESS 500
#define SIZEC 17
#define SIZER 30
#define WORDROW 5
#define VOICE 3
#define CKVALUE 12345

// bitwise ops
#define MODE_MORSE 1
#define MODE_SHORT 2
#define MODE_PARM 4

// morse codes 
class mcodes {
	public:
	long mkey[MAXMCODES];
	char mval[MAXMCODES];
	int cnt;
        mcodes();
	int loadcode(char*);
	int sortcode();
	int getcode(long, char *);
        int dumpcodes (char *); // for testing
        
        private:
        void q_sort(long*, char*, int, int); 
};

// short codes 
class scodes {
	public:
	char skey[MAXSCODES][10];
	char sval[MAXSCODES][MAXSCODE_TXT];
	int cnt;
        scodes();
	int loadcode(char *);
	int sortcode();
	int getcode(char *, char *);
};

// parameter codes 
class pcodes {
	public:
	int pvc;  // voice code
        pcodes();
	int loadcode(char *);
        int loadparmcode(char *);
        int putcode(int *);
	int getcode(int *);
        int clear();
};

// char class - stack of dits and dahs
class char_stk {
  public:
    int ditdah[MAXDD]; // stack of dit and dahs
    int ptr; // pointer to top of ditdah stack
    char_stk();
    int push(int); // push a ditdah  
    int pop(); // pop last ditdah
    int clear(); // clear stack
    int size(); // get value of ptr
    long get_charval(int &, int *); // return character value, eg, 12 or 2121, value of ptr and ditdah array
};

// word class -- stack of characters that make up a single word
class word_stk {
  public:
     char words[MAXWORD]; // word stack [characters]
     int ptr; // pointer to next char to be added - also the length
     word_stk();
     int clear(); // clear the stack
     int push(char); // push a char onto word stack
     int pop(); // pop last character that was just pushed
     int get_ptr(); // get ptr
     int get_pword(char *); // get previous word
}; 

// message class - stack of words that display on the screen
class message_stk {
  public:
      char **msg;  // array of char 
      int ptr; 
      message_stk();
      int clear();
      int push(char *);
      int pop(char *); 
      int get_msg(int, char *);
      int get_ptr(); 
};

// EEPROM data
struct EEPromData {
     int ckvalue;  // should be 12345
     int LongPress; // length of a long press (300 - 1500)
     int Voice; // voice number to use (0 - 8)
     int v[10];  // reserved
};

