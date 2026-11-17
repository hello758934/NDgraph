# NDgraph v0.31

**Real + complex-plane graph explorer for TI-Nspire CX II CAS using Ndless.**

**Status: experimental source project.** The native ARM executable (`NDgraph.tns`) is **not included**. Desktop math tests have passed; the Windows Ndless toolchain and actual calculator hardware have not been verified for this version.

## Features

- **Real and Imaginary modes** (Tab switches mode)
- Full-screen **Functions** tab, opened/closed by the physical **Catalog** key
- Six graph slots per mode, individually colored and hideable; blank input boxes on launch
- Real explicit and implicit equations (`y=sin(x)`, `x=2`, `x^2+y^2=4`)
- Complex polynomial root plotting (`z^4=1`) and complex parametric/locus equations (`z=t*(1+i)`, `re(z)=im(z)`, `abs(z)=1`)
- Basic trig, inverse trig, hyperbolic, logarithm, exponential, absolute value, and square-root operations
- Function-status light at top right (heuristic for some implicit curves)
- Full-screen **Help** with H; bottom event log instead of a hotkey list
- Multi-graph overlays, trace, auto-fit, session history, real explicit-curve numerical intersections, graph settings
- Help footer: `made by carlos 2026`

## Getting started on Windows

1. Back up important calculator documents and ensure a compatible version of **Ndless** is already installed.
2. Open **TNS Tool Portable**. In Syntax Doctor use the green **+ File → Open Ndless Project** control.
3. Import a ZIP containing this project's source files and `.tnsproject.json`; choose **Build TNS**.
4. If compilation succeeds, transfer the compiled `NDgraph.tns` using TI-Nspire CX II Connect, then launch it with Ndless active.
5. If compilation fails, keep the compiler log; do not transfer partial output. **Do not rename a ZIP to `.tns`.**

You can download this repository using **Code → Download ZIP**, then import that ZIP in TNS Tool Portable. Do **not** try to open the ZIP with the normal `.tns` document viewer.

## Controls

| Key | Action |
| --- | --- |
| Catalog / open-book | Toggle full-screen Functions tab / Graph |
| Tab | Switch Real / Imaginary mode |
| H | Toggle full-screen Help (when not editing) |
| Enter | Select/edit and confirm expressions in Functions tab |
| V / C / Delete | Visibility / color / delete graph slot |
| T / F / I | Trace / auto-fit / real explicit-function intersections |
| R / S | History / settings |
| Arrows | Move selection, trace, pan, or zoom depending on screen |
| Esc | Cancel, close a menu, or exit |

See [USAGE.md](USAGE.md) for the complete guide, including tests, limitations, and examples.

## Host testing (optional)

On a computer with GCC and Make:

```sh
make test
```

Test programs are included, but this is **not** a substitute for a real ARM build or calculator testing.

## Limitations

This is not a symbolic CAS. Roots are numerical and limited to supported polynomials up to degree 10. Implicit curve rendering, function-status classification, auto-fit, and intersections use numerical approximations. History/settings are session-only. Tested calculator key mappings and performance may differ from desktop simulations.

## License and credits

MIT — see [LICENSE](LICENSE), including the copyright notice for eligible original contributions. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for Ndless/nSDL acknowledgments and separate licenses. Some project code was produced with AI assistance; copyright eligibility depends on applicable law and human contributions.

Independent hobby project; not affiliated with Texas Instruments.
