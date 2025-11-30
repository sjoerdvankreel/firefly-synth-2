<a id="04CB533D-5968-47AF-BD54-A64363DBD583"></a>
# GUI

<a id="BC6A28F5-C34B-40B2-BE02-E600DBB5EBF3"></a>
## Top section
Contains everything that is not directly related to generating audio.

<a id="4472F4E6-1315-4FDC-A52E-CAF6D7136DD2"></a>
### Output panel
Shows CPU usage relative to single core, voice usage and output gain with a maximum of 0dB.<br/>
The MTS-ESP indicator shows whether an MTS-ESP master is connected.

<a id="68289DDE-CD5E-400C-B022-3893A3989441"></a>
### Panic button
Kills all active voices and flushes delay lines.<br/>
Comes in handy for stuck notes or when you managed to fill the delay lines with garbage.

<a id="547D2FBC-6E68-4379-AD04-2262C07FA286"></a>
### Patch panel
Shows the name of the last loaded patch.<br/>
Comes with regular load-from/save-to-file, init patch to defaults, and load factory preset options, as well as:

* Session: revert the patch state to the time the DAW session was loaded.
* Reload: revert the patch state to the last time a patch was loaded from file, preset, or default values (init patch).

<a id="ED3DACBE-8FF2-47D2-9EA6-7EF67367B541"></a>
### GUI settings
* Show modulation bounds:<br/>Highlights the minimum and maximum values currently applied to a parameter by the mod matrix and direct modulators.
* Show tweaked from:<br/>Highlights the parameters that differ from either default values, last loaded DAW session state, or last loaded patch state.
* Knob visuals from engine:<br/>Highlights a parameters current audio engine state. For per-voice parameters, takes into account the minimum and maximum values across all voices.
* Plot visuals from engine:<br/>Draws the current state of the audio engine into the graph plots, taking all modulation into account. For per-voice modules, this is an additional line per voice. When off, graph plots reflect only the current state of the parameters as shown in the GUI.

<a id="8440B096-7851-48DE-A1AA-27665935FC33"></a>
### Last tweaked
Shows the name of the last tweaked parameter as well as a textbox that allows for precise-editing that parameters value.

<a id="548A9565-5657-4217-AF82-B53B509139FE"></a>
### Graph plots
Shows a plot of the last tweaked module.<br/>
For example this can be an oscilloscope, filter frequency response, waveshaper response or others, depending on the module being shown.

<a id="C60E14F0-ACB5-417C-8B10-94702B99FF23"></a>
## Main section
This is the primary screen for patch design.<br/>
It contains all parameters for oscillators, filters, effects, envelopes and LFOs as well as all audio routing controls and high-level parameters like microtuning and portamento controls.

<a id="AFA1D831-2F91-4FA4-9CBA-958F4AD32DA8"></a>
### Voice
Contains controls affecting an entire voice (all per-voice oscillators and filters).<br/>
Pitch section: coarse pitch with envelope modulation amount and fine pitch with LFO modulation amount.<br/>

Portamento section: allows selection of regular (on) mode (constant pitch, variable time) or automatic mode (constant time, variable pitch) and optional tempo-syncing.
Because FF2 does not provide monophonic mode, the portamento controls also come with the option to shorten the per-voice amplitude envelope attack and release sections.
Best illustrated with an example:
* Set portamento mode to "Section"
* Set both Amp Attack and Amp Release to 25%
* Play A-B-C-[pause]-D-E-F
* Attack time of A and D is normal.
* Release time of C and F is normal.
* Attack times of B, C, E, and F are shortened.
* Release times of A, B, D, and E are shortened.
This feature works strictly by tracking note-on/off events. Section "shortening" will kick in only for overlapping notes, or notes that are both triggered and released at the exact same time.

<a id="83AA98D4-9D12-4D61-81A4-4FAA935EDF5D"></a>
### Master
Contains controls affecting the plugin as a whole.<br/>
These parameters control all voices together as well as affect global filters and effects.

Pitchbend together with bend range sets the base pitch for the entire plugin.<br/>
By default it is set to control everything, but it can also target specific oscillators.<br/>
By default, MIDI pitchbend is set to modulate master pitchbend at full scale, but this can be changed in the mod matrix.

Likewise, modwheel is by default modulated by MIDI modwheel at full scale by the matrix.<br/>
Modwheel doesn't control anything unless you set up the matrix to do so.

There are 4 auxiliary parameters which also don't target anything by default.<br/>
Use the mod matrix to assign modulation targets to them.<br/>
This is useful for controlling the mod amount of other sources or controlling multiple parameters at once.

<a id="3689411E-F31C-4F8C-BE3D-6F87938A1A1B"></a>
### Settings
Receive MIDI notes controls whether the plugin kicks off a new voice on incoming midi note on messages.<br/>
This is enabled by default for the instrument version of the plug and disabled by default for the fx version.<br/>
Some hosts allow to send midi notes to an fx plugin, others do not.<br/>
If possible, you can do nice things like keyboard tracking global filters in the fx build.

The MIDI and automation smoothing control parameter can be used to smooth sudden changes in incoming MIDI and automation events.<br/>
This parameter affects CLAP modulation events as well.

Microtuning controls:<br/>
These allow some control over what settings that make up the pitch are applied in the tuned or untuned parts of pitch calculation.<br/>
Incoming MIDI notes are always part of the retuned pitch.

* Tuning: when on, enables microtuning. Requires an MTS-ESP master to be connected.
* Tune On Note: when on, microtuning is applied at incoming MIDI.
When off, microtuning is applied continuously. 
When on, the "tune master pitchbend" switch has no effect for voice-level tuning, but it does still affect the master pitch sources in the global mod matrix.
* Master pitch bend: includes master PB in either the tuned or untuned part of pitch calculations.
When off, setting master PB to +12 semis results in 1 octave up (so, double the frequency).
When on, and master PB is set to +12 semis, playing C4 results in the exact same pitch as playing C5 with microtuning enabled (and master PB set to 0).
* Voice coarse: includes voice coarse pitch parameter in either the tuned or untuned part of pitch calculations.
This is primarily intended to cooperate with global unison, which can alter the coarse pitch of individual voices.
* Tune master matrix/tune voice matrix: these affect whether voice-level pitch and global last/low/high-key pitch sources in the matrix are retuned.
See the matrix section for more details.

<a id="5C91D5A0-3EC1-4142-935A-3FE83B60C04E"></a>
### Voice Mixer
Controls the signal path of the per-voice audio engine.
