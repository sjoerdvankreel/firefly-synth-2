FF1 and FF2 are pretty much the same in spirit, and they share a lot of algorithms on the DSP side.<br/>
Nevertheless, both are different enough that I felt a new version was warranted, and they cannot share patches either.

Long story short, main reasons for the rebuild are:
* Full CLAP support including polyphonic modulation
* Write SIMD-vectorized friendly code, enhancing the plugin performance
* Provide direct-from-audio-engine data from realtime to UI, allowing for superior realtime visualization

# The Just-Different
Stuff that both can do, just in different ways.

* Microtuning.<br/>
FF1 allows for before/after modulation tuning, FF2 allows for more options on what to retune.
* CLAP threadpool.<br/>
FF1 uses it, FF2 does not.<br/>
Rationale: FF1 adapts to the host block size, FF2 uses fixed internal block size. This also means FF2 needs PDC.<br/>
In the end, FF1 will scale better within a single plugin instance, FF2 will scale better in general.
* Mod matrix.<br/>
FF1 uses separate matrices for audio-to-audio, CV-to-audio and CV-to-CV.<br/>
FF2 uses a single matrix for CV-to-anything, and separates out the audio-to-audio matrices into dedicated mixer sections.
* Module slot handling.<br/>
FF1 generally has more slots for osci/fx/lfo/envelope.<br/>
FF2 cuts down on those but provides sub-slots within each module, providing more options, but reducing on routing possibilities.<br/>
For example all sub-slots within an FX slot are oversampled together, and sub-slots within an LFO are accessible as individual mod sources.

# The Bad
Stuff that FF1 can do, and FF2 can NOT do.

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
FF2 does contain new section-attack/section-release shortening controls to emulate those short transitions for leads etc.<br/>
If you want a monosynth, get a proper monosynth. By now I feel they are different enough to not try and force one into a polysynth.

totally new features
filter tuning
comb filter by hz + ktrk
easy access
less point and click more drag the slider
different:
clap threadpool
microtuning
10 slots vs 4x4
oversampling 124 vs 14
more osci and global uni
exp bipo/split envelopes
loads more mod sources/targets
more osci types
faster
better ui realtime
better clap
exponential fm
dedicated fm osci
mod-any-to-any/no audio/cv-split