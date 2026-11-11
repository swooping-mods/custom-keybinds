# Custom Keybinds: behavior and implementation

Describes the exported implementation **0.1.0**. The linked source and metadata define this version; gameplay qualification is a separate requirement.

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
