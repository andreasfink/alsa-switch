# alsa-switch
Switching of Alsa audio devices

the purpose of alsa-switch is to control the flow of multiple audio-streams going through a system while supporting priority streams

You invoke audio-switch like this:

auido-switch <input-device> <output-device> <control-file>

for example
	audio-switch microphone speaker  control1

This creates a named pipe named "control1", it is ready to connect "microphone" audio device to "speaker" audio device once the control pipe is telling it to do so.
Writing a character to the control channel does this

commands to send:
   '1'...'9'	priority 1...9 announcement
   '0'          end of announcement (channel muted)

typical use case

   you have audio source1...9		
   you have virtual speakers 1...9
   you mix virtual speaker 1...9 to physical speaker in alsa (/etc/asound.conf)

   you run alsa-switch and it will create control files 1...9

  now all sound sources are muted
  if source 2 now has a level 8 priority annoucement it would write  '8' into "control2"
  this would mute all audio streams which have no announcement or a priority announcement of level '9'

  1 is the highest priority
  9 is the lowest priority
  0 is channel is muted

At startup all channels are muted.

Should a higher priority annoucement come in now on source 3, then source 2 would become muted too.
Once source 3 is finished and source 2 is still in priority level 8, the audio from source 2 would be heard again.

