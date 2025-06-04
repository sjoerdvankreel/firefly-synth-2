# This is NOT a finished product.
* Only builds on windows (for the time being).
* Only osci, inter-osci mod, string osci, per-voice fx and envelopes implemented.
* So no delay, reverb, LFO's, global fx, NO routing matrices (routing is hardcoded), even ENV1 is only connected to per-voice amp, the others route nowhere.

# Why yet another one?

A repository for trying out a bunch of ideas that are currently not easy to do in firefly synth.
Or, IOW, stuff that I got wrong on the first (infernal synth) and second (firefly synth) take.

* Architecture
    * ALREADY OK: Definately have the notion of a declarative static topology defined as (module-index/module-slot/param-index/param-slot)
    * Param-slot is useful for counted parameters like the ones in mod matrices.
    * Declarative topo is great for serialization and mapping to/from clap/vst3 parameters.
    * Also great for automated doc generation and tooltips
    * BUT make it completely compile-time static (with constexpr/templates/macros/whatever).
    * Also great for declarative UI, however I'm no longer convinced that 100% declarative UI is the way to go.
    * CLAP threadpool is the way to go - keep it in!

* File / project organization
    * Don't be so strict about having a base library and a plugin project.
    * Probably better to not have the notion of plugin-base at all, just go with a bunch of reusable components.

* Automation and modulation handling
    * Dense buffers are somewhat of a memory hog but even with (1000 params)*(128 block size)*(64 voices)*(sizeof float) this comes out to "only" 30 mb which is doable, and that's including dense buffers for clap polymod.
    * Plus i'd go crazy trying to do mod matrices with sparse event streams.
    * Deal with ramped (reaper) vs impulse + smoothing (everything else). Maybe even make it a UI switch.
    * Deal with polyphonic modulation from the get-go. It proves hard to retrofit.
    * Drop the idea that "everything should be automatable". There's no point flipping sine to saw by automation, and it complicates stuff.
    * Tricky: some stuff should still be automatable per-voice-start like envelope release time. Figure out a way to do this without upscaling to a full dense buffer.

* DSP
    * Definately keep the notion of internal block size.
    * Has to be, because with dense buffers otherwise memory scales linear with host block size.
    * Make the block size explicit in the DSP code! Clang generates great code for stuff like "float* restrict a,b; for(int i = 0; i < 128; i++) a[i] += b[i];" (actually completely branch free when targeting avx1)
    * Keep taps on what the autovectorizer is doing.
    * Tryout clang on windows, i cannot for the life of me get MSVC to generate code that uses the ymm registers.
    * Oscis cannot run independently! This defeats feedback-fm with less-then-block size feedback (which is the interesting case).
    * On a larger scale, just let the plugin code define the order in which modules run. Better to be explicit about "1. glfo, 2. vlfo, 3. osci, 4. vfilter, 5. gfilter" etc
    * There should be a re-usable AudioProcessor or such which deals with the absolute minimum stuff like voice lifetime management and automation/modulation (preparing the dense buffers)

* Memory management
    * Keep it centralized and keep it strict.
    * Current setup where each "module" (osci, filter, delay line, etc) does it's own allocation doesn't cut it (even though it theoretically doesnt allocate on realtime).
    * Better to have a big-ass struct like "struct all_i_need { array<array<array<float, 128>, 2>, OSCI_COUNT> osci_outs; array<array<array<float, 128>, 2>, FILTER_COUNT> filter_ins; }"
    * Might still get big but at least we know where it's happening
    
* UI
    * All the helper stuff like drag-to-matrix, copy-to etc is good
    * 100% declarative proves to be difficult (surprise).
    * Better to have a bunch of reusable components (in particular, auto-sizing grid)
    * And have plug GUI stitch those together
    * DSP should dominate UI! Need processor-to-controller sync from the very start.
    * By defining [rt-state, rt-mod-state (nondestructive), ui-state, ui-mod-state (nondestructive)] should be possible to keep those in sync
    * Realtime pushes to UI *on each block* (host block not internal), UI discards depending on frame-rate
    * This solves all parameter painting but leaves realtime graphs => difficult, i dont want to exchange buffers, only scalars (so this probably still needs reconstruction on UI side)
    * VST3 IDataExchange seems good for it