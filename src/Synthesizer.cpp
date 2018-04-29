//
// Programmer:    Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: 5 January 1998
// Last Modified: 5 January 1998
// Last Modified: Tue Mar 13 14:12:11 PST 2001 (added 0x80 message filtering)
// Filename:      ...sig/code/src/control/Synthesizer/Synthesizer/cpp
// Web Address:   http://www-ccrma.stanford.edu/~craig/improv/src/Synthesizer.cpp
// Syntax:        C++
//
// Description:   A class for handling Synthesizer Input and Output
//	  	  especially the input, the MidiOutput class is
//		  sufficient for synthesizer output alone.
//

#include "Synthesizer.h"


//////////////////////////////
//
// Synthesizer::Synthesizer
//

Synthesizer::Synthesizer(void) : MidiIO() {
   for (int channel=0; channel<16; channel++) {
      for (int contno=0; contno<128; contno++) {
         Controller[channel][contno].setSize(DEFAULT_CONT_SIZE);
      }
   }
   zeroControllers();
   note.setSize(1024);
}


Synthesizer::Synthesizer(int outputDevice, int inputDevice) :
      MidiIO(outputDevice, inputDevice) {
   for (int channel=0; channel<16; channel++) {
      for (int contno=0; contno<128; contno++) {
         Controller[channel][contno].setSize(DEFAULT_CONT_SIZE);
      }
   }
   zeroControllers();
   note.setSize(1024);
}
  


//////////////////////////////
//
// Synthesizer::~Synthesizer
//

Synthesizer::~Synthesizer() {
   // do nothing
}



//////////////////////////////
//
// Synthesizer::controller -- returns the current state of the controller
//	by default, but can check the history back to DEFAULT_CONT_SIZE - 1
//	default value: channel = 0.
//	default value: index = 0.
//

int Synthesizer::controller(int controlNumber, int channel, int index) {
   return Controller[channel][controlNumber][index];
}



//////////////////////////////
//
// Synthesizer::extractNote -- returns the next note
//	in the incoming note buffer.
//

smf::MidiEvent Synthesizer::extractNote(void) {
   smf::MidiEvent event;
   note.extract(event);
	return event;
}



//////////////////////////////
//
// Synthesizer::getNoteCount -- return the number of note 
//	messages waiting in the note buffer.  Can't extract
//	a note if this function returns a 0.
//

int Synthesizer::getNoteCount(void) const {
   return note.getCount();
}



//////////////////////////////
//
// Synthesizer::operator[] -- returns the note message
//	relative to the currently inserted note.
//

smf::MidiEvent& Synthesizer::operator[](int index) {
   return note[index];
}



//////////////////////////////
//
// Synthesizer::processIncomingMessages --
//

void Synthesizer::processIncomingMessages(void) {
   smf::MidiEvent event;
   while (MidiInput::getCount() > 0) {
      MidiInPort::extract(event);
      interpretMessage(event);
   }
}



//////////////////////////////
//
// Synthesizer::zeroControllers  -- set history and current values
//	of controllers to zero.
//

void Synthesizer::zeroControllers(void) {
   for (int contno=0; contno<128; contno++) {
      for (int channel=0; channel<16; channel++) {
         for (int element=0; 
               element<Controller[channel][contno].getSize(); element++) {
            Controller[channel][contno][element] = 0;
         }
      }
   }
}



///////////////////////////////////////////////////////////////////////////
//
// private functions
//


//////////////////////////////
//
// Synthesizer::interpretMessage -- all note information gets sent
//    to a circular buffer.  All cont messages get sorted into
//    separate slots according to channel.
//

void Synthesizer::interpretMessage(smf::MidiEvent& aMessage) {
   if ((aMessage.getCommandByte() & 0xf0) == 0x90) {         // a Note-on message
      note.insert(aMessage);
   } else if ((aMessage.getCommandByte() & 0xf0) == 0x80) {  // a Note-off message
      note.insert(aMessage);
   } else if ((aMessage.getCommandByte() & 0xf0) == 0xb0) {  // a controller message
      Controller[aMessage.getCommandByte() & 0x0f][aMessage.getP1()].insert(aMessage.getP2());
   }
   // ignore all other messages
}




// md5sum: 87b0cba2949293ed4f76f4cfbd940d87 Synthesizer.cpp [20020518]
