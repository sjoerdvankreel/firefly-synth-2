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

* Voice Mix to GFX:<br/>
Routes the voice mixdown to the global effect (GFX) section.
* External Audio to GFX:<br/>
Routes external audio to the global effect (GFX) section.<br/>
For the FX build, external audio is the plugin's primary input.<br/>
For the instrument build, external audio is the plugin's sidechain input.
* Global FX to global FX:<br/>
Routes the output of global effect modules to other global effect modules.<br/>
FX modules are processed in order, so you can only route GFX 1 to 2/3/4, 2 to 3/4 etc.
* GFX to out:<br/>
Routes individual global FX to master output.
* VMix to out:<br/>
Routes voice mixdown to master output.
* External audio to out:<br/>
Routes external audio to master output.

The amplitude parameter is just master gain with an optional LFO applied to it.<br/>
Likewise the balance control is global stereo balance with an optional LFO applied to it.

<a id="73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6"></a>
### Oscillator

#### Main section
Contains controls common to all oscillator types.

Oscillator type:
* String: combined string/noise osci.
* FM: 3-op FM generator with 3x3 matrix.
* Wave: sine, saw, sqr etc, couple of PWM-capable algorithms, some hard-sync capable algoritms and a DSF generator.
* External Audio: use audio in as an oscillator to allow it as an FM/AM/RM source in the inter-osci modulation section.

Comes with gain control plus optional envelope, fine pitch control plus optional LFO (vibrato), phase offset and coarse pitch controls.
Panning: an osci is inherently monophonic unless you turn on the osci-level unison feature.
Pan control may be used to pan hard-left/hard-right or anything in between.<br/>

Keyboard tracking: by default an oscillator's base pitch (so not counting coarse/fine offset and unison detune) follows the voice's base pitch.
The voice base pitch is basically the incoming MIDI note with pitch bend, voice-level coarse/fine offset, portamento and microtuning applied.
If you turn off keyboard tracking the oscillator base pitch becomes C0.
You can then use the the osci-level coarse and fine pitch controls to affect the osci pitch.<br/>

By itself this gives you a constant pitch, so not really useful.
Instead, this feature is meant to allow building up the osci pitch by the matrix.
You can stack individual components like voice key, pitch bend, portamento offset, voice coarse/fine offset to the osci's coarse pitch to build up the corresponding voice pitch by hand.
This opens up some neat possibilities like leaving out specific pitch components (like PB) for a single osci only, or f.e. to invert the effect of the portamento.<br/>

There is one caveat: if you use microtuning you cannot accurately build up the voice pitch from individual components.
If you want to have a non-tracking oscillator and play it microtuning-aware, you have to use the unified "Voice Pitch" matrix source.
This source combines everything that affects voice-level pitch and then applies microtuning to the final result.
Rationale: i don't know how to individually microtune PB, voice coarse offset, portamento etc and still have the result come out "correct",
i.e., equal to the result of tuning them all together at once. Don't even know if it's possible or not.

#### Unison section
Controls osci-level unison.

These controls apply to all oscillator types:
* Voice count: how often to calculate the osci with the different settings applied, and add it to the output signal.<br/>
* Blend: this gradually decays the unison voice level as it moves away from center position.<br/>
If you don't have a center position (so, even voice count) setting this to zero produces silence.
* Stereo spread: spreads out the unison voices from hard-left to hard-right.

For External Audio, the blend and spread controls don't really do much.<br/>
You can hear the effect of the blend parameter as a change in amplitude only.<br/>
The spread parameter "does something" but you basically cannot hear it as each subvoice is exactly the same, so, it doesn't matter where it is positioned in the stereo spectrum.

Unison detune applies to all oscillator types except External Audio.<br/>
It controls the fine pitch offset relative to center position (so, with odd voice count, center voice has no offset).

Phase offset and phase offset randomization apply to Wave and FM mode only.<br/>
With zero phase offset, the unison output will have a harsh attack to it as all subvoices are initially the same.<br/>
With 100% phase offset, the unison output produces more of a string-like sound but still with a very distinctive phasing effect.<br/>
Use the phase offset randomization control to produce a more natural sound.

#### Wave section

6 sub-slots per oscillator slot. Take your pick of:<br/>
* 2x basic waveform with nothing but a gain control:<br/>
Includes all the common ones like sin, cos, saw, sqr, tri as well as a bunch of more esoteric waveforms.
* 2x pulse-width-modulatable waveform with gain control and PWM control:<br/>
Classic square-to-pulse, triangle-to-saw and a couple others.
* 1x hard-sync capable waveform:<br/>
Choice of saw/sqr/tri. Pitch controls the offset of slave pitch relative to the osci's internal/master pitch.
* 1x DSF generator (additive oscillator with just a couple control parameters):<br/>
Distance (specified in times the osci frequency) and decay control the frequency and amplitude of consecutive sines.<br/>
Overtones mode allows to explicitly specify the number of sines generated.<br/>
Bandwidth mode allows to specify a range between base frequency only (0%) or all the way up to Nyquist/half-sample-rate (100%).

#### FM section

This is a dedicated FM-generator with 3 operators and a 3x3 matrix with unit delay on the feedback path.

The mode control switches between regular (linear) FM and exponential FM.<br/>
In the linear case the modulator affects the phase of the carrier (so yeah it's PM really, like everyone else does it).<br/>
In the exponential case the modulator affects the pitch of the carrier.<br/>
Exponential FM can produce some really deep sounds but it also "shifts" the pitch, so you need to get it back in tune somehow.

Ratio mode: allows to specify the carrier/modulator ratio as either free-form/continuous or rational (1:2, 2:3, 3:4 etc).<br/>
Operator 1 is implicitly defined as "1", and you can select op2:op1 and op3:op2 on top of that.<br/>
So, set both to 2:1, then op3:op1 will be 4:1. Either 4 times the pitch or 4 times the frequency, depending on the mode.





