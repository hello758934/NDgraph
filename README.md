# NDgraph v0.41 ⚡

**Real, imaginary, and calculus graphing for the TI-Nspire CX II CAS.**

Made by Carlos (2026) · Free and open source · [MIT License](LICENSE)

So this project started as a little complex-number grapher called **Complex**. Then I added Cartesian graphing. Then a proper interface. Then calculus.

Yeah, it escalated. 💀

**GOOD NEWS: YOU DON'T HAVE TO COMPILE THE APP ANYMORE.** All four versions below now have downloadable ZIPs containing compiled `.tns` files. Just extract the ZIP, transfer the `.tns` to your calculator, and open it with Ndless.

> **READ BEFORE INSTALLING:** NDgraph is an experimental native Ndless application for the **TI-Nspire CX II CAS**. Make sure you have a compatible version of [Ndless](https://github.com/ndless-nspire/Ndless) installed, and back up important calculator files first. These builds were supplied as compiled by the project author; compatibility and stability can vary by calculator OS and Ndless version.

## Downloads — choose your version

| Version | What's in it? | Download |
| --- | --- | --- |
| **v0.41 (latest)** | Real, Imaginary, and Calculus modes; calculus Help update | **[NDgraphV0.41.zip](https://github.com/user-attachments/files/35046496/NDgraphV0.41.zip)** |
| **v0.31** | Real + Imaginary graph explorer, full-screen Help and event log | [NDgraphV0.31.zip](https://github.com/user-attachments/files/35046341/NDgraphV0.31.zip) |
| **v0.2** | Adds Cartesian graphing alongside the complex plane | [NDgraphV0.2.zip](https://github.com/user-attachments/files/35046339/NDgraphV0.2.zip) |
| **v0.1 (Complex)** | The original complex-plane polynomial-root grapher | [Complex.zip](https://github.com/user-attachments/files/35046338/Complex.zip) |

You can also browse the [GitHub Releases page](https://github.com/hello758934/NDgraph/releases). **The direct downloads above are the updated compiled packages**, even if an older release description still mentions building from source.

## Easy installation — no compiler needed

1. **Back up your calculator** and confirm that [Ndless](https://github.com/ndless-nspire/Ndless) supports your calculator OS.
2. Download the ZIP for the version you want from the table above. **v0.41 is the newest.**
3. **Extract** the ZIP on your computer and locate the compiled `.tns` file inside. The filename may differ between versions.
4. Connect your **TI-Nspire CX II CAS** to your computer using a USB data cable.
5. Open [TI-Nspire CX II Connect](https://nspireconnect.ti.com/) and transfer the extracted `.tns` file to your calculator.
6. Open the transferred file on your calculator with Ndless active.

**That's it. No TNS Tool, no compiling, no suffering. 😭**

**Do not transfer the ZIP itself** or rename a ZIP to `.tns`. Extract the actual compiled file first. If a download doesn't contain a `.tns`, [open an issue](https://github.com/hello758934/NDgraph/issues) with the ZIP name.

## Features (latest version: v0.41)

### Real mode — the Cartesian plane
- Plot explicit functions such as `y=x`, `y=x^2`, and `y=sin(x)`.
- Plot vertical lines and other relations, e.g. `x=2`.
- Plot implicit equations, e.g. `x^2+y^2=4`.
- Plot parametric curves, e.g. `x=cos(t);y=sin(t)`.
- Overlay up to **six individually colored graphs** in this mode.

### Imaginary mode — the complex plane
- Plot polynomial roots, e.g. `z^4=1` or `z^2+1=0`.
- Plot complex parametric curves, e.g. `z=t*(1+i)`.
- Plot complex loci, e.g. `re(z)=im(z)` or `abs(z)=1`.
- Work with real/imaginary parts and supported complex functions.
- Plot up to **six individually colored graphs** in this mode.
- The polynomial root solver supports supported polynomials up to degree 10.

### Calculus mode — added in v0.4, expanded in v0.41
- **Numerical derivatives:** `y=diff(x,x^3)`
- **Definite integrals:** `y=integral(t,0,x,sin(t))` (also `int`)
- **Finite sums:** `y=sum(k,1,5,k*x)`
- **Finite products:** `y=prod(k,1,4,k)`
- Graph supported expressions containing these operations.
- Expanded expression editing for longer calculus formulas (up to **384 characters**).
- Extra functions: `sec`, `csc`, `cot`, `log10`, `log2`, `cbrt`, `round`, `trunc`, `sign`.
- **v0.41:** second full-screen Help page explaining calculus syntax; use **Right/Left** arrows to switch Help pages.

**Important:** Calculus is **numerical**, not a full symbolic computer algebra system. Derivatives and integrals are approximations; sums/products have a 256-term limit.

### Graphing and interface
- **Three modes:** Real → Imaginary → Calculus (cycle with **Tab**).
- **Six graph slots per mode**, each with its own color and visibility toggle.
- **Full-screen Functions tab** — press the physical **Catalog/open-book** key to toggle between equations and the graph.
- **Function-status light** — green for recognized single-valued functions; dark for non-functions or uncertain cases.
- **Trace mode** to inspect supported curves and roots.
- **Auto-fit** to estimate a useful viewing window.
- **Numerical intersections** between the first two visible supported explicit real functions.
- **Equation history** for the current session.
- **Settings** for graph options, axes, grid, angles, and parameter range.
- **Full-screen Help**, plus a bottom **event log** instead of a giant hotkey strip.
- Help-screen footer: `made by carlos 2026`.

## Quick controls

| Key | Action |
| --- | --- |
| **Catalog / open-book** | Switch between the Graph and full-screen Functions tab |
| **Tab** | Cycle Real → Imaginary → Calculus |
| **Up / Down** | Select equation slots, navigate menus, or zoom (depending on screen) |
| **Enter** | Edit or confirm an equation |
| **V** | Toggle selected graph visibility (Functions tab) |
| **C** | Change selected graph color (Functions tab) |
| **Delete** | Clear selected graph slot (when not editing) |
| **T** | Trace |
| **F** | Auto-fit |
| **I** | Toggle supported real-function intersections |
| **R** | Equation history |
| **S** | Settings |
| **H** | Open/close full-screen Help |
| **Left / Right in Help (v0.41)** | Switch between controls and calculus-syntax pages |
| **Esc** | Cancel, go back, or exit |

The event log at the bottom displays messages such as `Switched to imaginary mode`; the permanent shortcut hint is `H Help`.

## Try these equations

| Mode | Equation | Result |
| --- | --- | --- |
| Real | `y=sin(x)` | Sine wave |
| Real | `x^2+y^2=4` | Circle |
| Real | `x=2` | Vertical line |
| Imaginary | `z^4=1` | Four complex roots |
| Imaginary | `z=t*(1+i)` | Diagonal line |
| Imaginary | `abs(z)=1` | Unit circle |
| Calculus | `y=diff(x,x^3)` | Numerical derivative |
| Calculus | `y=integral(t,0,x,sin(t))` | Accumulated integral |
| Calculus | `y=sum(k,1,5,k*x)` | Finite sum |
| Calculus | `y=prod(k,1,4,k)` | Finite product |

## Build from source (optional — developers only)

You **do not need to do this to install NDgraph**. This section is for people who want to modify the program or build it themselves.

**What you need:** a Windows x64 computer, [TNS Tool Portable](https://github.com/acewalt/tns-tool-wasm/releases), and a compatible Ndless setup.

1. Click **Code → Download ZIP** on this repository, or get the appropriate source package.
2. Make sure `.tnsproject.json` is at the top level of the source ZIP (not buried inside another folder).
3. Launch **TNS Tool Portable for Windows x64**.
4. Open **Syntax Doctor**.
5. Use its green **+ File → Open Ndless Project** control. (Not the top-left File menu — that may only show Exit.)
6. Select the source ZIP and click **Build TNS**.
7. If the build succeeds, save the generated `.tns` and transfer it to your calculator.

The repository also includes a `Makefile` and host-side test programs. On a supported desktop C toolchain, `make test` runs the available mathematical tests. Host tests are **not** the same as verifying the native calculator build.

## Limitations and troubleshooting

- **Experimental software:** not every graph, function, or calculator OS configuration is guaranteed to work.
- **Numerical results:** calculus, intersections, implicit plotting, auto-fit, and function classification use approximations.
- **Intersections:** currently intended for the first two visible supported **explicit real** functions, not arbitrary implicit curves.
- **History and settings:** session-oriented; don't assume they'll be saved permanently.
- **ZIP won't open on the calculator?** Extract the compiled `.tns` file first.
- **App won't launch?** Check your Ndless version, calculator OS compatibility, and that you transferred the actual `.tns`.
- **Found a bug?** [Report it here](https://github.com/hello758934/NDgraph/issues), ideally with the NDgraph version, calculator OS, equation, and steps to reproduce.

For the detailed v0.31 graph-explorer documentation, see [USAGE.md](USAGE.md). Some older instructions there describe the source-only workflow; **the compiled downloads in this README supersede those installation instructions**.

## About NDgraph

**Creator:** Carlos (2026)  
**License:** [MIT](LICENSE)  
**Contributing:** [CONTRIBUTING.md](CONTRIBUTING.md)  
**Third-party acknowledgments:** [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)

NDgraph is an independent, AI-assisted, open-source project. It uses Ndless/nSDL tooling and is **not affiliated with or endorsed by Texas Instruments**.

**Made by Carlos — 2026.**

*This was supposed to be a complex-number grapher. 💀*
