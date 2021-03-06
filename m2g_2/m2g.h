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

#define MAXDD 20
#define MAXCHAR 80
#define MAXMCODES 60
#define MAXSCODES 40
#define MAXPCODES 5
#define BUFSZ 2300
#define BUFMCODE 1000
#define BUFSCODE 1200
#define BUFPCODE 100
#define SIZPWORD 20
#define SIZMESG 80
#define NCOL 20
#define NROW 4
#define DEBUG 0
#define MSDO 225
#define MSDA 1000
#define MSLT 2000
#define MSCL 5000
#define CODE "CODE.CSV"
#define USR_PARM "USR_PARM.CSV"
#define HELLO_FILE "HELLO.TXT"
#define DEBOUNCEDELAY 50 

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
	char sval[MAXSCODES][80];
	int cnt;
        scodes();
	int loadcode(char *);
	int sortcode();
	int getcode(char *, char *);
};

// parameter codes 
class pcodes {
	public:
	unsigned pdo[MAXPCODES];
	unsigned pda[MAXPCODES];
	unsigned plt[MAXPCODES];
	unsigned pcl[MAXPCODES];
	int cnt;
        pcodes();
	int loadcode(char *);
        int loadparmcode(char *);
	int getcode_pop(int, unsigned *);
        int push(unsigned *);
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
    long get_charval(); // get character value, eg, 12 or 2121
};

// word class -- stack of characters and spaces
class word_stk {
   public:
     char words[MAXCHAR]; // word stack [characters]
     int ptr; // pointer to next char to be added - also the length
     int prev_ptr; // pointer to prev value of ptr, when it was a space -- used to find last token
     word_stk();
     int clear(); // clear the stack
     int push(char); // push a char onto word stack
     int push_words(char *); // push a bunch of characters onto the word stack
     int pop(); // pop last character that was just pushed
     int nextword(); // go to next word
     int get_words(char *); // get string of word stack
     int save_ptr(int); // save pointer in prev_ptr
     int get_ptr(int); // get ptr or prev_ptr
     int get_pword(char *); // get previous word
     int trim_words(); // trim length of words stack 
}; 

