FF1 and FF2 are pretty much the same in spirit, and they share a lot of algorithms on the DSP side.<br/>
Nevertheless, both are different enough that I felt a new version was warranted, and they cannot share patches either.

Long story short, main reasons for the rebuild are:
* Full CLAP support including polyphonic modulation.
* Less point-and-click GUI, and more direct-access drag-the-sliders.
* Write SIMD-vectorized friendly code, enhancing the plugin performance.
* Provide direct-from-audio-engine data from realtime to UI, allowing for superior realtime visualization.

# The Just-Different
Stuff that both can do, just in different ways.

* Microtuning.<br/>
FF1 allows for before/after modulation tuning, FF2 allows for more options on what to retune.<br/>
* Echo targeting.<br/>
FF1 allows reverb/delay per global effect slot.<br/>
FF2 has a single dedicated echo module with both, providing a custom target in the processing pipeline<br/>
(and a per-voice echo module as well).
* Mod matrix.<br/>
FF1 uses separate matrices for audio-to-audio, CV-to-audio and CV-to-CV.<br/>
FF2 uses a single matrix for CV-to-anything, and separates out the audio-to-audio matrices into dedicated mixer sections.<br/>
FF2 provides boatloads more modulation sources and targets. Whether that's good or bad is up to you.
* CLAP threadpool.<br/>
FF1 uses it, FF2 does not.<br/>
Rationale: FF1 adapts to the host block size, FF2 uses fixed internal block size. This also means FF2 needs PDC.<br/>
In the end, FF1 will scale better within a single plugin instance, FF2 will scale better in general.
* Module slot handling.<br/>
FF1 generally has more slots for osci/fx/lfo/envelope.<br/>
FF2 cuts down on those but provides sub-slots within each module, providing more options, but reducing on routing possibilities.<br/>
For example all sub-slots within an FX slot are oversampled together, and sub-slots within an LFO are accessible as individual mod sources.

# The Bad
Stuff that FF1 can do, and FF2 can NOT do.

* No drag-and-drop.<br/>
Too complicated with the new multi-tab setup.
* No arpeggiator.<br/>
This actually feels like a real miss, might rebuild.
* No stereo balance for audio mixers (VMix/GMix).<br/>
Just gain only, didn't see the need for balancing.
* No multi-EQ module.<br/>
Pretty sure it does not belong in a general-purpose synth, won't rebuild.<br/>
You can still stack multiple BPF/shelving filters, but more tedious.
* No dedicated distortion module.<br/>
Would like to have it, might rebuild.<br/>
The new multi-slot effect modules allow to build something like it manually, but more tedious.
* No separate smoothing controls for MIDI, BPM, and automation.<br/>
On the radar, but i don't know how important it is.<br/>
FF2 only has 1 global option for all 3 together.<br/>
If-ever, i prefer separate controls for individual automatable parameters and maybe even MIDI CC's.
* No bipolar and/or split envelope segments.<br/>
Just linear and unipolar exponential, I just didn't see the need for the other ones anymore (might reconsider, convince me).
* No monophonic mode, period.<br/>
I also do not intend to build it back, because too complicated.<br/>
FF2 does contain new portamento section-attack/section-release shortening controls to emulate those short transitions for leads etc.<br/>
If you want a monosynth, get a proper monosynth. By now I feel they are different enough to not try and force one into a polysynth.

# The Good
Stuff that FF1 can do, but FF2 does better.

* MSEG editor.<br/>
Both have one, but FF2's comes with more options.
* CLAP support.<br/>
FF1 does global modulators only, FF2 does per-voice modulators as well.
* String oscillator.<br/>
FF2's combined string/noise osci can do color-morphing and pitch-shifting.
* Multi-tap delay.<br/>
FF1 contains a couple of control parameters for creating the delay lines.<br/>
FF2 allows full control over all 8 individual taps.
* Performance.<br/>
YMMV, but ballpark twice as fast.<br/>
Varies per-feature of course, f.e. FF2's hardsync is actually slower (but way better, audio-wise).
* Global unison.<br/>
FF1 does a handful of controls for osci/lfo detune and envelope spreading.<br/>
FF2 contains dozens of per-voice targets with a bunch of controls per target for either automatic or manual voice-spectrum creation.
* Realtime visualization.<br/>
FF1 tries to infer graph plots from a minimal dataset, and does a bad job at doing so.<br/>
FF2 dumps the entire per-voice parameter state (and more) from audio to UI, so does not need to infer anything.<br/>
It just re-uses the existing audio algorithms, and renders them directly to screen.<br/>
* Less point-and-click, more drag-the-slider.<br/>
FF1 needs loads of check-the-box and select-the menu for setting up f.e. the matrices.<br/>
FF2 tries to cut down on that by using the VMix/GMix sections and providing readily-available mod sources on the main screen like "LFO to Fine Pitch".
Of course, FF2 is a fully-featured synth just like FF1 so there's bound to be some point-and-click, especially when you need the mod matrix and/or the global unison.

# The Brand-New

* Basic oscillator algorithms.<br/>
FF2 gained lots of them.
* LFO min/max.<br/>
LFO's gained new min/max options.
* External audio as osci.<br/>
To allow for FM (etc) from the outside world.
* Highlight mod bounds.<br/>
Shows parameter modulation bounds in the GUI.
* Engine knob visuals.<br/>
Per-parameter visualization of what the engine is doing.
* Looping envelopes.<br/>
All envelopes now have loop start/length controls, representing the sustain section.
* Filters on echo.<br/>
All echo modules (feedback delay, multi-tap delay, reverb) gained LPF/HPF options.<br/>
Nothing you couldn't do before, except for the feedback delay filters, which are placed INSIDE the feedback path.<br/>
Allows for some neat effects.
* Filter tuning.<br/>
All SVF and comb filters now can be expressed in pitch as well as frequency, optionally following microtuning.<br/>
Additionally, the comb filters are now specified in Hz instead of microseconds, and they can be keytracked just like the SVF's.
* Dedicated FM oscillator.<br/>
3-op FM osci with 3x3 matrix, unit delay on the feedback path.
* Exponential FM.<br/>
For both the dedicated FM-osci as well as the inter-osci modulation section.
* Per-voice echo.<br/>
Super-cool, super-expensive, and super-complicated. Refer to the manual for details.