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
* Show modulation bounds:<br/>
Highlights the minimum and maximum values currently applied to a parameter by the mod matrix and direct modulators.
* Show tweaked from:<br/>
Highlights the parameters that differ from either default values, last loaded DAW session state, or last loaded patch state.
* Knob visuals from engine:<br/>
Highlights a parameters current audio engine state.<br/>
For per-voice parameters, takes into account the minimum and maximum values across all voices.
* Plot visuals from engine:<br/>
Draws the current state of the audio engine into the graph plots, taking all modulation into account.<br/>
For per-voice modules, this is an additional line per voice.<br/>
When off, graph plots reflect only the current state of the parameters as shown in the GUI.

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

Portamento section: allows selection of regular (on) mode (constant pitch, variable time) or automatic mode (constant time, variable pitch) and optional tempo-syncing.<br/>
Because FF2 does not provide monophonic mode, the portamento controls also come with the option to shorten the per-voice amplitude envelope attack and release sections.
Best illustrated with an example:
* Set portamento mode to "Section"
* Set both Amp Attack and Amp Release to 25%
* Play A-B-C-[pause]-D-E-F
* Attack time of A and D is normal.
* Release time of C and F is normal.
* Attack times of B, C, E, and F are shortened.
* Release times of A, B, D, and E are shortened.

This feature works strictly by tracking note-on/off events.<br/>
Section "shortening" will kick in only for overlapping notes, or notes that are both triggered and released at the exact same time.

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

* Tuning:<br/>
When on, enables microtuning.<br/>
Requires an MTS-ESP master to be connected.
* Tune On Note:<br/>
When on, microtuning is applied at incoming MIDI.<br/>
When off, microtuning is applied continuously.<br/>
When on, the "tune master pitchbend" switch has no effect for voice-level tuning, but it does still affect the master pitch sources in the global mod matrix.
* Master pitch bend:<br/>
Includes master PB in either the tuned or untuned part of pitch calculations.<br/>
When off, setting master PB to +12 semis results in 1 octave up (so, double the frequency).<br/>
When on, and master PB is set to +12 semis, playing C4 results in the exact same pitch as playing C5 with microtuning enabled (and master PB set to 0).
* Voice coarse:<br/>
Includes voice coarse pitch parameter in either the tuned or untuned part of pitch calculations.<br/>
This is primarily intended to cooperate with global unison, which can alter the coarse pitch of individual voices.
* Tune master matrix/tune voice matrix:<br/>
These affect whether voice-level pitch and global last/low/high-key pitch sources in the matrix are retuned.<br/>
See the matrix section for more details.

<a id="5C91D5A0-3EC1-4142-935A-3FE83B60C04E"></a>
### Voice Mixer
Controls the signal path of the per-voice audio engine.

* Oscillator to voice FX:<br/>
Routes the output of individual oscillators to the per-voice effect (VFX) section.
* Voice FX to voice FX:<br/>
Routes the output of per-voice effect modules to other per-voice effect modules.<br/>
FX modules are processed in order, so you can only route VFX 1 to 2/3/4, 2 to 3/4 etc.
* Oscillator mix:<br/>
This is an internal intermediate stage that combines the output of all oscillators.<br/>
You can control how much each osci contributes to oscillator mix, and then route the combined mix to each of the VFX modules.
* Oscillator to out:<br/>
Directly routes individual oscis to voice output.<br/>
If you use this in combination with osci to oscillator mix, an osci output will be added to the voice signal twice.
* VFX to out:<br/>
Directly routes individual voice-level FX to voice output.<br/>
If you use this in combination with oscillator mix to VFX, an effect output will be added to the voice signal twice.

The amplitude parameter controls the voice level as a whole.<br/>
Amp env to amp controls how much the per-voice amplitude envelope affects this parameter.<br/>
By default this is set to 100% so the entire voice just follows the voice amp envelope.<br/>
You can set it to 0% and have the voice play at full scale regardless of the voice amp envelope.<br/>
This is useful if you want to assign different envelopes to individual oscis.<br/>
In any case the voice amp envelope still controls voice lifetime - when it's done, the voice is killed,<br/>
regardless of any other potentially still active envelopes.

Voice amplitude envelope target: this controls at which stage the voice amp envelope is applied.<br/>
The most natural setting for this is "VMix Out", but it can be set to individual oscillators, osci mix, or individual FX as well.<br/>
The primary use case for this is to cooperate with per-voice echo. Per-voice echo can be applied to the voice as a whole,<br/>
but also to individual oscis and FX. In which case, it is necessary to change the target accordingly, to prevent the voice echo<br/>
from being cut short by the voice amplitude envelope. See the Voice Echo section for more details.

Balance control: this is just a straightforward stereo balancer with an optional LFO applied to it.<br/>
Osc mix to out: controls how much the internal osci mixer stage contributes to the final voice output.

<a id="09ED2CCE-3AC5-455E-935C-A684BEBC154D"></a>
### Global Mixer
Controls the signal path of the global audio engine.

* Voice Mix to GFX routes the voice mixdown to the global effect (GFX) section.
* External Audio to GFX routes external audio to the global effect (GFX) section.<br/>
For the FX build, external audio is the plugin's primary input.<br/>
For the instrument build, external audio is the plugin's sidechain input.
* Global FX to global FX routes the output of global effect modules to other global effect modules.
* GFX to out routes individual global FX to master output.
* VMix to out routes voice mixdown to master output.
* External audio to out routes external audio to master output.