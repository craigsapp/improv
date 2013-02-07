//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Wed Feb  6 13:59:17 PST 2013
// Last Modified: Wed Feb  6 13:59:20 PST 2013
// Filename:      ...sig/examples/all/smfinfo.cpp
// Web Address:   http://sig.sapp.org/examples/museinfo/humdrum/smfinfo.cpp
// Syntax:        C++; museinfo
//
// Description:   Converts a single melody MIDI file/track into an ASCII text 
//	          format with starting time and pitch.
//

#include "Array.h"
#include "Options.h"

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

// user interface variables
Options options;
int     fixQ  = 0;          // used with -f option

// function declarations:
void      checkOptions      (Options& opts, int argc, char** argv);
void      example           (void);
void      usage             (const char* command);
void      fixTrackByteCounts(const char* filename);
long      getFileContents   (Array<unsigned char>& contents, ifstream& infile);
void      printInfo         (const char* filename);
int       processTrack      (Array<unsigned char>& contents, int offset);

//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
   checkOptions(options, argc, argv);

   int i;
   const char* filename;
   for (i=1; i<=options.getArgCount(); i++) {
      filename = options.getArg(i);
      if (fixQ) {
         fixTrackByteCounts(filename);
      } else {
         printInfo(filename);
      }
   }
   return 0;
}


//////////////////////////////////////////////////////////////////////////


//////////////////////////////
//
// fixTrackByteCounts -- Fix the byte sizes of a Standard MIDI file.
//

void fixTrackByteCounts(const char* filename) {

   ifstream infile(filename, ios::in | ios::binary);
   Array<unsigned char> contents;
   if (!getFileContents(contents, infile)) {
      // something went wrong, ignore the file.
      return;
   }
   infile.close();

   if (contents[0] != 'M') { return; } // not a MIDI file
   if (contents[1] != 'T') { return; } // not a MIDI file
   if (contents[2] != 'h') { return; } // not a MIDI file
   if (contents[3] != 'd') { return; } // not a MIDI file

   int trackcount = (contents[10] << 8) | contents[11];
   int trackoffset = 14;
   int realtrackcount = 0;
   while (trackoffset = processTrack(contents, trackoffset)) {
      realtrackcount++;
   }
   cout << "TRACKS PROCESSED " << realtrackcount << endl;

   ofstream outfile(filename, ios::out | ios::binary);
   outfile.write((char*)contents.getBase(), contents.getSize());
   outfile.close();

}



//////////////////////////////
//
// processTrack -- parse through a track and then count the number
//      of bytes in the track.
//

int processTrack(Array<unsigned char>& contents, int offset) {
   if ((contents[offset+0] != 'M') || (contents[offset+1] != 'T') ||
       (contents[offset+2] != 'r') || (contents[offset+3] != 'k')) {
      cerr << "Invalid MIDI track: " 
           << contents[offset+0]
           << contents[offset+1]
           << contents[offset+2]
           << contents[offset+3]
           << endl;
      exit(1);
   }
   int  tracksize = (contents[offset+4] << 24) | (contents[offset+5] << 16) 
         | (contents[offset+6] << 8) | (contents[offset+7]);
   int  testsize;
   unsigned char testbyte    = 0;
   unsigned char commandbyte = 0;
   int  current     = offset + 8;
   while (current < contents.getSize()) {
      // jump over time value:
      while (contents[current] >= 0x80) {
         // skip over continuation bit VLVs
         current++;
      }
      current++;

      // jump over MIDI (or meta) data.
      testbyte = contents[current];
      if (testbyte >= 0x80) {
         commandbyte = testbyte;
         current++;
      }
      switch (commandbyte & 0xf0) {
         case 0x80: current += 2; break;  // note-off: 2 data bytes
         case 0x90: current += 2; break;  // note-on: 2 data bytes
         case 0xA0: current += 2; break;  // aftertouch: 2 data bytes
         case 0xB0: current += 2; break;  // continuous controller: 2 data bytes
         case 0xC0: current += 1; break;  // patch change: 1 data bytes
         case 0xD0: current += 1; break;  // channel pressure: 1 data bytes
         case 0xE0: current += 2; break;  // pitch-bend: 2 data bytes
         case 0xF0: 
            switch (commandbyte & 0x0f) {
               case 0x0f: // meta message
                  if ((commandbyte == 0xff) &&
                      (contents[current+0] == 0x2f) &&
                      (contents[current+1] == 0x00)) {
                     // end-of-track marker, fix size in track header and return
                     testsize = (current+2) - (offset+8);
                     if (testsize == tracksize) {
                        // track size was correct
                        if (current+2 >= contents.getSize()) {
                           return 0;
                        } else  {
                           return current+2;
                        }
                     }
                     // fix track size:
                     cout << "changing track size from " << tracksize 
                          << " to " << testsize << endl;
                     contents[offset+4] = (char)(0xff & (testsize >> 24));
                     contents[offset+5] = (char)(0xff & (testsize >> 16));
                     contents[offset+6] = (char)(0xff & (testsize >>  8));
                     contents[offset+7] = (char)(0xff & (testsize));
                     if (current+3 >= contents.getSize()) {
                        return 0;
                     } else  {
                        return current+3;
                     }
                  }
                  current += 2;
                  current += contents[current];
                  current++;
                  break;
               default:
                  cerr << "ERROR CANNOT HANDLE 0xF0 type command yet" << endl;
                  exit(1);
            }
            break;
      }
   }

   cerr << "GOT HERE UNEXPECTEDLY" << endl;
   exit(1);
}



//////////////////////////////
//
// getFileContents -- Return the entire contents of a file.  Returns the
// count of items in the data.
//

long getFileContents(Array<unsigned char>& contents, ifstream& infile) {
   contents.setSize(0);
   infile.seekg(0, ios::end);
   long length = infile.tellg();   
   if (length < 22) {
      // cannot possibly be a MIDI file, so return no content.
      return 0;
   }
   contents.setSize(length);
   infile.seekg(0, ios::beg);
   infile.read((char*)contents.getBase(), length);
   return length;
}
   



//////////////////////////////
//
// printInfo -- Fix the byte sizes of a Standard MIDI file.
//

void printInfo(const char* filename) {
   ifstream infile(filename, ios::in | ios::binary);
   Array<unsigned char> contents;
   if (!getFileContents(contents, infile)) {
      // something went wrong, ignore the file.
      return;
   }
   infile.close();

   cout << filename << endl;
   if ((contents[0] != 'M') || (contents[1] != 'T') ||
      (contents[2] != 'h') || (contents[3] != 'd') ) {
      cout << "\tNOT A MIDI FILE\n";
   }
   int miditype   = (contents[8] << 8) | (contents[9]);
   int trackcount = (contents[10] << 8) | (contents[11]);
   int divisions  = (contents[12] << 8) | (contents[13]);
   int smpteframe = -1;
   int subframes  = -1;
   if (contents[12] >= 0x80) {
      // SMPTE timing
      smpteframe = 0xff - contents[12] + 1;
      subframes = contents[13];
   }
   cout << "\ttype:\t\t" << miditype << endl;
   cout << "\ttracks:\t\t" << trackcount << endl;
   if (smpteframe > 0) {
      cout << "\tsmpte:\t\tyes" << endl;
      cout << "\tframes:\t\t" << smpteframe << endl;
      cout << "\tsubframes:\t" << subframes << endl;
   } else {
      cout << "\tsmpte:\t\tno" << endl;
      cout << "\tdivisions:\t" << divisions << endl;
   }

}


//////////////////////////////
//
// example --
//

void example(void) {

}



//////////////////////////////
//
// usage --
//

void usage(const char* command) {

}



//////////////////////////////
//
// checkOptions -- 
//

void checkOptions(Options& opts, int argc, char* argv[]) {
   opts.define("f|fix=b", "fix track chunk byte sizes");

   opts.define("author=b",  "author of program"); 
   opts.define("version=b", "compilation info");
   opts.define("example=b", "example usages");   
   opts.define("h|help=b",  "short description");

   opts.process(argc, argv);
   
   // handle basic options:
   if (opts.getBoolean("author")) {
      cout << "Written by Craig Stuart Sapp, "
           << "craig@ccrma.stanford.edu, 6 Feb 2013" << endl;
      exit(0);
   } else if (opts.getBoolean("version")) {
      cout << argv[0] << ", version: 6 Feb 2013" << endl;
      cout << "compiled: " << __DATE__ << endl;
      exit(0);
   } else if (opts.getBoolean("help")) {
      usage(opts.getCommand());
      exit(0);
   } else if (opts.getBoolean("example")) {
      example();
      exit(0);
   }

   fixQ = opts.getBoolean("fix");
}


// md5sum: d8201865177b80aa72106e3198962cdc midi2melody.cpp [20050403]
