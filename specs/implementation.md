# Custom Keybinds: behavior and implementation

Describes the retained payload **0.1.0**. The behavior prose describes the retained payload; current candidate source and metadata are identified below. Gameplay qualification is a separate requirement.


<!-- source-only-sync -->
## Current source candidate

Source version **0.1.0** is saved from workshop revision `ca32bbf79ce0840d5166a232bdb3a9fdcd761254`. The retained installable payload remains **0.1.0** with its original bytes. This source update does not rebuild or requalify that payload. The current source and metadata define the candidate; gameplay, rendered behavior and co-op remain unqualified for rebuilt bytes.

The authored behavior and lifecycle descriptions below are retained from the published payload. They are not new qualification evidence for the source candidate. The integration table below follows the current candidate metadata.
<!-- /source-only-sync -->

## Behavior

The INI remaps up to 26 named vanilla actions by replacing their key arrays, mouse indices and displayed labels. Ctrl+Home reloads; Ctrl+End restores captured vanilla values and mod-action defaults. The mod-bindings section addresses namespaced native Input SDK actions, including modifier chords. A shared resolver can apply configured values to actions registered later; standalone DLL-local registries keep isolated defaults.

## State and lifecycle

The file stores preferences; applied bindings are runtime state. Original arrays and labels use managed roots and restore on observed world exit/reset. Reference fields receive actual object references; mouse indices use scalar values. Unknown actions/keys are skipped. Focus/typing/UI gates apply to reload and reset; vanilla input retains its own normal admission.

## Integration and configuration

No additional feature-mod runtime dependency is declared. Included shared headers are source reuse, not extra DLL dependencies.

The [module metadata](../source/mods/custom-keybinds/mod.json) owns the version and declared integration contract. Native modules start at `MoonLab_ModStart`. Game/Unity integrations target the pinned Steam Windows x64 IL2CPP build.

## Implementation sources

- [mod.c](../source/mods/custom-keybinds/mod.c)
- [support.h](../source/mods/custom-keybinds/support.h)

## Verification boundary

No movement/look-axis remapping, gamepad mapping or arbitrary modifier chords in the vanilla reflection adapter. Native mod chords use the separate Input SDK contract. Full in-game remapping and interaction coverage require qualification.

`python3 build.py --check` verifies the exported source hashes. It does not execute the mod or qualify the behaviors above. Installation and independent build prerequisites are in the [README](../README.md).
