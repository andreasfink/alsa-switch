# alsa-switch
Switching of Alsa audio devices

the purpose of alsa-switch is to control the flow of multiple audio-streams going through a system while supporting priority streams


to use alsa-switch, you need to create a named pipe first:

mkfifo  control1


Now you have a named pipe you can use to control the audio stream
if you now invoke audio-switch like this:

audio-switch SE16_LE 8000 microphone speaker  control1

it would create an audio stream at 8kHz sampling rate going from the audio device "microphone" to the audio device "speaker". You can control the stream by writing a byte to the "control1" named pipe

commands to send:
   '1'...'9'	priority 1...9 announcement
   '0'          end of priority announcement

typical use case

   you have audio source1...9		
   you have virtual speakers 1...9
   you mix virtual speaker 1...9 to physical speaker in alsa (/etc/asound.conf)

   you create control files 1...9

  now all sound sources are playing audio and all is mixed together
  if source 2 now has a level 8 priority annoucement it would write  '8' into "control1"
  this would mute all audio streams which have no priority announcement or a priority announcement '9'

  1 is the highest priority
  9 is the lowest priority
  0 is no priority

