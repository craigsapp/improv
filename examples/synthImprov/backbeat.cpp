//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Thu Sep 22 22:51:04 GMTDT 2005
// Last Modofied: 6 October 2005
// Filename:      ...sig/doc/examples/improv/synthImprov/backbeat/backbeat.cpp
// Syntax:        C++; synthImprov 2.0
//  
// Description:   Records reverse conducting data for the Mazurka Project.
// 
//

#include "synthImprov.h" 
#include <ctype.h>

#ifdef OLDCPP
   #include <fstream.h>
   #include <iomanip.h>
#else
   #include <fstream>
   #include <iomanip>
   using namespace std;
#endif

// include files for timing info
#include <time.h>
#include <stdio.h>
#include <sys/timeb.h>


/*----------------- beginning of improvization algorithms ---------------*/

int measureno = 2;
int pickupQ = 0;     // toggle for changing the starting beat position
int beatno = 1;
int beatsperbar = 3;
int starttime = -1;
ofstream outputfile;

/*--------------------- maintenance algorithms --------------------------*/

void description(void) {
   printboxtop();
   psl(
   "  BACKBEAT - by Craig Stuart Sapp <craig@ccrma.stanford.edu> - 21 Sep 2005");
   psl("");
   printintermediateline();
   psl(" Press upper case 'C' to set the CPU speed of your computer first,");
   psl(" then press the space bar for beats and any other key for a beat");
   psl(" in a new measure.  Triple meter is the default beat counting method.");
   psl(" Press the a number key to select a different number of beats per bar.");
   psl("");
   psl(" The data is echoed to the screen and also to a file called test.dat.");
   psl(" Rename test.dat after exiting the data entry program to save the data");
   psl(" before running this program again, since it will erase the previous");
   psl(" contents of test.dat.");
   printboxbottom();
} 


void initialization(void) { 
   description();
   outputfile.open("test.dat");
}


void finishup(void) {
   cout << "*-\t*-\t*-\t*-\n";
   outputfile << "*-\t*-\t*-\t*-\n";
   outputfile.close();
}


/*-------------------- main loop algorithms -----------------------------*/

void mainloopalgorithms(void) { 

}
      

/*-------------------- triggered algorithms -----------------------------*/

void keyboardchar(int key) { 
   static int lasttime = -1;
   int atime = 0;
   int dtime = 0;
   int value;

   if (key == 'p') {
      pickupQ = 1;
      cout << "Select pickup beat number." << endl;
      return;
   }

   if (!isdigit(key)) {
      pickupQ = 0;
   }

   if ((pickupQ == 1) && isdigit(key)) {
      // set the starting beat of the pickup
      value = key - '0';
      if (value < 1) {
         value = 1;
      } else if (value > beatsperbar) {
         value = beatsperbar;
      }
      beatno = value;
      cout << "Pickup beat set to : " << value << endl;
      pickupQ = 0;
      return;
   }

   if (!isdigit(key)) {
      if (starttime <= 0) {
         starttime = t_time;
         cout << "**kern\t**beat\t**abstime\t**deltatime\n";
         outputfile << "**kern\t**beat\t**abstime\t**deltatime\n";
      }
      if (lasttime < 0) {
         lasttime = 0;
      }
      atime = t_time - starttime;
      dtime = atime - lasttime;
      lasttime = atime;
   }


   switch (key) {
      case ' ':        // beat marker
         cout << "4\t" << beatno << "\t" << atime << "\t" << dtime << endl;
         outputfile << "4\t" << beatno << "\t" << atime << "\t" << dtime << endl;
         break;

      case '1': beatsperbar = 1; cout << "Beats per bar = 1" << endl; break;
      case '2': beatsperbar = 2; cout << "Beats per bar = 2" << endl; break;
      case '3': beatsperbar = 3; cout << "Beats per bar = 3" << endl; break;
      case '4': beatsperbar = 4; cout << "Beats per bar = 4" << endl; break;
      case '5': beatsperbar = 5; cout << "Beats per bar = 5" << endl; break;
      case '6': beatsperbar = 6; cout << "Beats per bar = 6" << endl; break;
      case '7': beatsperbar = 7; cout << "Beats per bar = 7" << endl; break;
      case '8': beatsperbar = 8; cout << "Beats per bar = 8" << endl; break;
      case '9': beatsperbar = 9; cout << "Beats per bar = 9" << endl; break;
      case '0': break;
      default:
         cout << "=" << measureno << "\t=" << measureno << "\t=" << measureno << "\n";
         outputfile << "=" << measureno << "\t=" << measureno 
                    << "\t=" << measureno << "\t=" << measureno << "\n";
         measureno++;
         cout << "4\t" << beatno << "\t" << atime << "\t" << dtime << endl;
         outputfile << "4\t" << beatno << "\t" << atime << "\t" << dtime << endl;
   }

   if (!isdigit(key)) {
      beatno++;
      if (beatno > beatsperbar) {
         beatno = 1;
      }
   }

   
}


/*------------------ end improvization algorithms -----------------------*/



