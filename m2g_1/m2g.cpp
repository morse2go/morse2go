/* m2g.cpp -- class file
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

#include <stdlib.h>
#include <cstring>
#include "m2g.h"
#include <arduino.h>

// morse code functions

// -----------  morse codes -----------------
inline mcodes::mcodes() {
  cnt = 0; 
}

inline int mcodes::loadcode(char* buf) { 
	char * p;
	char * p1;
	char buf1[60];
	char typcode[10];
	char ktok[10];
	char vtok[10];
	long key;
	int i, lineno;
        char *line[MAXMCODES];

        // parse lines
        lineno=0;
        p = strtok(buf, ">>");
        while(p != NULL) {
                line[lineno] = p;
                lineno++;
                p = strtok(NULL, ">>");
        }
        
	for (i=0; i < lineno; i++) {

		strcpy(buf1, line[i]);

		p1 = strtok(buf1, ",");
		strcpy(typcode, p1);  // type of code, eg, mcode

		p1 = strtok(NULL, ",");
		strcpy(ktok, p1);
		key = strtol(ktok, NULL, 10);  // key of code, eg, 12, 2111 (a, b)
	
		p1 = strtok(NULL, ",");
		strcpy(vtok, p1); // value of code, eg A, B

		mkey[cnt] = key;  
		mval[cnt] = vtok[0];
		cnt++;
	}
}

inline int mcodes::sortcode(){ 

   q_sort(mkey, mval, 0, cnt-1);
  
}

// quick sort - code copied from 
// (http://) p2p.wrox.com/visual-c/66347-quick-sort-c-code.html 

inline void mcodes::q_sort(long *mkeys, char *mvals, int left, int right) {
  long pivot, l_hold, r_hold;
  char pivot_v;
 
  l_hold = left;
  r_hold = right;
  pivot = mkeys[left];
  pivot_v = mvals[left];
    
  while (left < right)
  {    
    while ((mkeys[right] >= pivot) && (left < right))
      right--;
    if (left != right)
    {
      mkeys[left] = mkeys[right];
      mvals[left] = mvals[right];
      left++;
    }
    while ((mkeys[left] <= pivot) && (left < right))
      left++;
    if (left != right)
    {
      mkeys[right] = mkeys[left];
      mvals[right] = mvals[left];
      right--;
    }
  }
  mkeys[left] = pivot;
  mvals[left] = pivot_v;
  pivot = left;
  pivot_v = left;
  left = l_hold;
  right = r_hold;
  if (left < pivot)
    q_sort(mkeys, mvals, left, pivot-1);
  if (right > pivot)
    q_sort(mkeys, mvals, pivot+1, right);
}

// binary search -- adapted from
// (http://) rosettacode.org/wiki/Binary_search
inline int mcodes:: getcode(long key, char *val){ 
    int low = 0, high = cnt-1, mid;
        
    while(low <= high) {
         mid = (low + high) / 2;
         if(mkey[mid] < key) {
                        low = mid + 1; 
                }
                else if(mkey[mid] == key) {
                    *val = mval[mid];
                     return 0;
                }
                else if(mkey[mid] > key) {
                     high = mid-1;
                }
        }
        *val = '?';
        Serial.print("not found: ");
        Serial.println(key);
        return -1;
}

inline int mcodes:: dumpcodes(char *str){
   char buf[100];
  
   for (int i = 0; i < cnt; i++) {
       sprintf(buf, "%d- %d : %c\n", i, mkey[i], mval[i]);
       strcat(str, buf);
   }
}

// -----------  short codes -----------------
inline scodes::scodes() {
  cnt = 0; 
}

// load short codes into class
inline int scodes::loadcode(char *str) { 
	char * p;
	char * p1;
        char * p2;
	char *line[MAXSCODES];
	char buf1[200];
	char typcode[10];
	char ktok[10];
	char vtok[200];
	int lineno, i; 

	// parse lines 
	lineno=0;
	p = strtok(str, ">>"); 
	while(p != NULL) {
	    line[lineno] = p;
	    lineno++;
	    p = strtok(NULL, ">>");
	}

	for (i=0; i< lineno; i++) {

		strcpy(buf1, line[i]);
                memset(ktok, 0, 10);
                memset(vtok, 0, 200);

		p1 = strtok(buf1, ",");
		strcpy(typcode, p1);  // type of code, eg, scode

		p1 = strtok(NULL, ",");
		strcpy(ktok, p1+1);  // key of code, eg, ":ih", ":it" 
                int klen = strlen(ktok);
                for (int k = 0; k < klen; k++)  // convert to uppercase
                    ktok[k] = toupper(ktok[k]); 
	
		p1 = strtok(NULL, ",");
		strcpy(vtok, p1);  // value of code, eg, "I am Thirsty"

		strncpy(skey[cnt], ktok, 2);  
		strncpy(sval[cnt], vtok, 79);

        	cnt++;
	}
}

inline int scodes::sortcode(){ 
}

// send k (key), return v (value)
inline int scodes:: getcode(char *k, char *v){ 
  	int i, rc, lenk, lencode;
	int fnd = -1;
        char buf[100];
        	
        lenk = strlen(k); 
	for (i=0; i< cnt && fnd < 0; i++) {
		if (strncmp(k, skey[i], lenk) == 0) { // code found
			strcpy(v, sval[i]);
			fnd = i;
		}
	}
	if (fnd < 0) {
            Serial.begin(9600);
            Serial.print("not found: ");
            Serial.println(k);
	    strcpy(v, "?");
            rc = fnd;
        }
        else
            rc = strlen(v);
	return (rc);
}

// -----------  parameter codes - timing variables -----------------
inline pcodes::pcodes() {
  for (int i=0; i < MAXPCODES; i++) {
    pdo[i] = 0;
    pda[i] = 0;
    plt[i] = 0;
    pcl[i] = 0;
  }
  cnt = 0; 
}

// load paramter codes into class
// input is Google Doc, pcode section
// push one set of codes onto stack
inline int pcodes::loadcode(char *str) { 
	char * p;
	char * p1;
        char * p2;
	char *line[MAXPCODES];
	char buf1[60];
	char typcode[10];
	char ktok[10];
	char vtok[10];
	unsigned val;
        int len;
	int lineno, i; 
        
	// parse lines 
        cnt = 0; 
	lineno=0;
	p = strtok(str, ">>"); 
	while(p != NULL) {
	    line[lineno] = p;
	    lineno++;
	    p = strtok(NULL, ">>");
	}

	for (i=0; i< lineno; i++) {

		strcpy(buf1, line[i]);
                memset(ktok, 0, 10);
                memset(vtok, 0, 10);

		p1 = strtok(buf1, ",");
		strcpy(typcode, p1);  // type of code - pcode

		p1 = strtok(NULL, ",");
		strcpy(ktok, p1+1);
                len = strlen(ktok);
                for (int k = 0; k < len; k++)  // convert to uppercase
                    ktok[k] = tolower(ktok[k]); 
	
		p1 = strtok(NULL, ",");
		strcpy(vtok, p1);
                val = atol(vtok);

                if (!strcmp(ktok, "do"))
                  pdo[cnt] = val;
                else if (!strcmp(ktok, "da"))
                  pda[cnt] = val;
                else if (!strcmp(ktok, "lt"))
                  plt[cnt] = val;
                else if (!strcmp(ktok, "cl"))
                  pcl[cnt] = val;
        }
	cnt++;
}

// load user input paramter codes into class
// input is microSD card - file USR_PARM.CSV
inline int pcodes::loadparmcode(char *str) { 
	char *p, *p1;
	char *line[MAXPCODES];
        char buf[BUFPCODE];
	char buf1[40];
        char v_str[10];
	int lineno, i, j; 
        
	// parse lines - store data in class
	lineno=0;
        strcpy(buf, str);
	p = strtok(buf, "\n"); 
	while(p != NULL) {
	    line[lineno] = p;
	    lineno++;
	    p = strtok(NULL, "\n");
	}

	for (i=0; i< lineno; i++) {
	  strcpy(buf1, line[i]);

          if (strlen(buf1) > 5) { // ignore blank lines
	    p1 = strtok(buf1, ",");
	    strcpy(v_str, p1);
            pdo[cnt] = atol(v_str);
          
	    p1 = strtok(NULL, ",");
	    strcpy(v_str, p1);
            pda[cnt] = atol(v_str);

	    p1 = strtok(NULL, ",");
	    strcpy(v_str, p1);
            plt[cnt] = atol(v_str);

	    p1 = strtok(NULL, ",");
	    strcpy(v_str, p1);
            pcl[cnt] = atol(v_str);
          
            cnt++;
          } 
        }
}

// lookup parameter codes
// return current value of cnt 
// if mode -1 pop stack, return cnt 
// if mode -2, return top of stack
//    mode >= 0 lookup only, return values in v[], \
// always returna cnt value
inline int pcodes:: getcode_pop(int mode, unsigned v[]){ 
  	int i, j, rc;

        // stack is empty - nothing to do -- underflow condition
        if (cnt == 0)
          return cnt;

        // pop last element of stack and return 
        if (mode == -1) {
          pdo[cnt-1] = 0;
          pda[cnt-1] = 0;
          plt[cnt-1] = 0;
          pcl[cnt-1] = 0;
          cnt--;
          return cnt;
        }
                    
        if (mode == -2) 
          j = cnt -1;
        else if (mode >= 0 && mode < cnt)
          j = mode;
        else j = -1;
                            
        if (j >= 0 && j < cnt) {          
          v[0] = pdo[j];
          v[1] = pda[j];
          v[2] = plt[j];
          v[3] = pcl[j];
        }
	return (cnt);
}

// push
// add another pcode set to the stack
inline int pcodes::push(unsigned v[]){ 

  pdo[cnt] = v[0];
  pda[cnt] = v[1];  
  plt[cnt] = v[2];  
  pcl[cnt] = v[3];
  cnt++;
}
 
inline int pcodes::clear(){ 

    for (int i=0; i < MAXPCODES; i++) {
         pdo[i] = 0;
         pda[i] = 0;
         plt[i] = 0;
         pcl[i] = 0;
    }
    cnt = 0;
}


// ---------------- character functions ------------------

inline char_stk::char_stk() {
  clear();
}

inline int char_stk::clear() {
  for (int i=0; i < MAXDD; i++)
    ditdah[i] = 0;
  ptr = 0;
}

inline int char_stk::push(int c) {
  if (ptr < MAXDD)
    ditdah[ptr++] = c;    
}

inline int char_stk::pop() {
  if (ptr >= 0) {
    ditdah[ptr] = 0;
    ptr--;
  } 
}

// get value of character, eg, 12 or 2112
inline long char_stk::get_charval() {
  int mult = 1;
  int i;
  long ch = 0;
  
  for (i = ptr-1; i > -1; i--) {
    ch = ch + (ditdah[i] * mult);
    mult = mult * 10;
  }
  return ch;
}

// -------------- word functions ----------------

inline word_stk::word_stk() {
  clear();
}

// clear class
inline int word_stk::clear() {
  memset(words, 0, MAXCHAR);
  ptr = 0; 
  prev_ptr = 0; 
}

// push a word into word stack
inline int word_stk::push(char c) {
  words[ptr] = c;
  if (ptr < MAXCHAR)
    ptr++;
}

// push several words into word stack
inline int word_stk::push_words(char* str) {
  
  int lenstr = strlen(str);
  if ((ptr + lenstr + 2) < MAXCHAR) {
      strcat(words, str);
      strcat(words, " ");
      ptr += lenstr;
      prev_ptr += lenstr;
  }
}

// current word finished - go to next word
inline int word_stk::nextword() {
    if (ptr < MAXCHAR) {
      words[ptr] = ' ';
      ptr++;
    }
}

// -- backspace -- 
// pop the last character in the word stack
inline int word_stk::pop() {
  int done = 0;  // break flag
  int ptr1;
  char buf[25];
  int i, len;

  if (DEBUG) {
    // before
    Serial.println("\n-- before --");
    sprintf(buf, "%d: %s", ptr, words);
    Serial.println (buf); 
  }
  
  // erase char
  if (ptr > 0) {
    ptr--;
    words[ptr] = 0;
  }
  if (ptr == 0) 
    clear();
    
  if (DEBUG) {
    // after
    Serial.println("\n-- before --");
    sprintf(buf, "%d: %s", ptr, words);
    Serial.println (buf); 
  } 
}

// get words
inline int word_stk::get_words(char * w) {
    strcpy(w, words);
}

/// save value of ptr
inline int word_stk::save_ptr(int i) {
    prev_ptr = ptr + i;
}

// get value of ptr (i==o) or prev_ptr i < 0)
inline int word_stk::get_ptr(int i) {
    if (i < 0)
      return prev_ptr;
    else
      return ptr;
}

// get value of previous word
inline int word_stk::get_pword(char* pwrd) {

  static char buf[SIZPWORD];
  int len, pp;

  memset(buf, 0, SIZPWORD);
  if (prev_ptr == 0)
      pp = prev_ptr;
  else
      pp = prev_ptr + 1; 

  for (int i = pp, j= 0; i < ptr; i++, j++)
    if (j < SIZPWORD && words[i] != ' ')
      buf[j] = words[i];
     
  strcpy(pwrd, buf);
}

// trim lenght of words stack
inline int word_stk::trim_words() {
  static char buf2[MAXCHAR];
  int i, i1, j;
  memset(buf2, 0, MAXCHAR);

  for (j = 20; words[j] == ' ' && j < MAXCHAR; j++) ; // skip to a space at least 20 chars from left 
  for (i1 = j, i=0; words[i1] > 0 && i1 < MAXCHAR; i1++, i++)
      buf2[i] = words[i1];
  memset(words, 0, MAXCHAR);
  strcpy(words, buf2);
 
  i = strlen(words);
  ptr = prev_ptr = i; 
}


