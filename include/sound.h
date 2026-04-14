#ifndef SOUND_H
#define SOUND_H

void playNote(unsigned char octave, unsigned char note, unsigned char isSharp);
void playSound(unsigned int frequency);
void stopSound(void);

#endif
