A repository for trying out a bunch of ideas that are currently not easy to do in firefly synth.
Or, IOW, stuff that I got wrong on the first (infernal synth) and second (firefly synth) take.

* Architecture
   * ALREADY OK: Definately have the notion of a declarative static topology defined as (module-index/module-slot/param-index/param-slot)
   * Param-slot is useful for counted parameters like the ones in mod matrices.
   * Declarative topo is great for serialization and mapping to/from clap/vst3 parameters.
   * BUT make it completely compile-time static (with constexpr/templates/macros/whatever).
   * Also great for declarative UI, however I'm no longer convinced that 100% declarative UI is the way to go.