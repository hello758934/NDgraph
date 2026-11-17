# NDgraph v0.31 — Graph Explorer (experimental Ndless native project)

**Target:** TI-Nspire CX II CAS with Ndless. **Not** an ordinary TI-Nspire
Lua document. This package is **source code**, not a precompiled `NDgraph.tns`.
Build it with TNS Tool Portable (Windows) to obtain the actual native `.tns`.
Do not rename this ZIP to `.tns` or transfer source files to the calculator.

## Windows / TNS Tool Portable

1. Open TNS Tool Portable, not the separate Ndless Web Compiler helper.
2. Choose **Syntax Doctor**. From its green **+ File** menu, choose
   **Open Ndless Project** (different from the titlebar's File → Exit).
3. Import `NDgraph_v0.31_Windows_Build.zip` and click **Build TNS**.
4. If compilation succeeds, save the resulting **NDgraph.tns** and transfer
   *that compiled file* to your calculator with TI-Nspire CX II Connect.
5. Launch with Ndless active. If compilation fails, keep the error log; do not
   use a partially built `.tns`.

**Compatibility:** C99 + nSDL + Ndless `KEY_NSPIRE_CAT`. The project was checked
with a host C compiler, standard test programs, and an SDL mock run. No files/firmware are deliberately
modified by the app.

## v0.31 interface patch — Log + full-screen Help

- The bottom status strip on Graph, Functions, History, and Settings is an
  **event log**, not a list of keyboard shortcuts. It shows events such as
  `Switched to imaginary mode`, `Functions tab opened`, `Slot 1 graphed`,
  and parser errors. The **only** key hint permanently shown there is
  **`H Help`**.
- Press **H** while not editing an equation to open a **full-screen Help menu**
  with the controls for all screens. Press **H again** to return to the exact
  screen you were on. **Esc** also closes Help.
- The exact credit text **`made by carlos 2026`** appears at the bottom of
  the Help screen.
- **Equation history has moved from H to R**, since H now opens Help.
- While typing an expression, the letter **h** stays available for functions
  like `sinh`, `cosh`, and `tanh`. Finish or cancel editing before opening Help.
- Trace readouts and intersection coordinates appear as small overlays on the
  graph; they do not overwrite the persistent bottom event log.
- Previous graphing functionality from v0.3 remains unchanged.

## Core design: dedicated full-screen FUNCTIONS tab

- **Physical Catalog / open-book key:** from graph → full-screen functions
  list; press the **same key again** → graph. Also works from Settings/History.
- Six graph slots in **each** of the two modes: **Imaginary** and **Real**.
  Slots are blank on first launch, so you can enter an entire equation,
  including `x=...`, `y=...`, or `z=...`.
- Use **Up/Down** to select a slot, **Enter** to edit it, type the full
  expression, and **Enter** again to parse and graph. You can also start
  typing when a slot is selected. **Esc** cancels an unfinished edit.
- **V** on the functions tab toggles a slot's visibility. **C** cycles its
  color. **Delete** removes the selected slot when *not* editing.
- Each slot preserves its expression while switching modes. Graphs overlay
  simultaneously in different colors. Maximum six graphs per mode.

The **top-right light** reflects the *selected slot*: green for recognized
single-valued functions (or numerically certified ones), dark for non-functions,
invalid/empty slots, or uncertain relations. For example `y=x` is green;
`x=2` is dark but still draws. `z=t*(1+i)` is green as a function of real `t`.
The numerical implicit-relation test is not a formal mathematical proof.

## Controls: graph screen

| Button | Action |
| --- | --- |
| Catalog (open book) | Open/close full-screen Functions tab |
| Tab | Switch Real / Imaginary (each has its own six slots) |
| T | Trace: selected curve/root; Up/Down selects a slot, Left/Right moves the trace |
| F | Auto-fit visible graphs (sampled bounds) |
| I | Toggle numerical intersections: the first two *visible real explicit* functions |
| H | Toggle the full-screen Help screen (not while entering text) |
| R | Open equation history (newest first, 12 entries per mode) |
| S | Open full-screen Settings |
| Up/Down (normal) | Zoom in/out |
| Left/Right (normal) | Pan left/right |
| PageUp/PageDown | Pan up/down (if the device maps them) |
| Home | Recenter graph |
| Esc | Exit native app |

**Trace details:** For `y=f(x)`, Left/Right changes x and displays `(x,y)`.
For `z=g(t)` or `x=x(t);y=y(t)`, Left/Right changes t. For polynomial roots,
Left/Right chooses the root and displays real part, imaginary part, magnitude
and argument. Implicit loci don't yet have a point-following trace engine.

**Intersections:** Red dots mark numerical crossings of the first two visible
real **explicit** functions in the current window. This does not yet find
intersections of implicit curves, near tangencies, or intersections outside the
viewport. Coordinates for the first crossing appear at the bottom.

**Auto-fit:** Samples visible graphs to estimate a common viewport. It is
reliable for finite roots and bounded parametric curves; sharp singularities,
tiny implicit components, and discontinuities can lead to imperfect fitting.

## Controls: Functions tab

| Button | Action |
| --- | --- |
| Up/Down | Select slot |
| Enter | Start editing the selected slot |
| Type / Backspace | Edit expression (max 180 characters) |
| Enter while editing | Compile/graph; errors stay in the editor |
| Esc while editing | Cancel without overwriting stored expression |
| V | Show/hide selected graph |
| C | Cycle selected graph color |
| Delete | Delete selected graph (while not editing) |
| H | Toggle full-screen Help (when not editing) |
| R | Full-screen history |
| S | Full-screen settings |
| Tab | Switch Real / Imaginary |
| Catalog | Return to graph; discards unfinished edit |

**History screen:** Up/Down selects a past expression, Enter loads it into the
selected Functions slot for editing. This is **session history only**, not
saved to a calculator document or permanent storage.

**Settings screen:** Use Up/Down to select and Left/Right to change: grid,
axes, radians/degrees, root guide lines, parameter range, and mode. Trigonometry
is interpreted in degrees only for *real-valued* trig arguments when degrees
are enabled; complex trig arguments always follow radian-based definitions.
`asin`, `acos`, `atan` of real arguments return degrees in degree mode. Changing
from radians to degrees adjusts the default t-range from ±2π to ±360.
Settings are not saved across launches.

## Examples

**Imaginary:**

- `z^4=1` — four isolated roots (the corners of a rotated square)
- `z^2-2*z+2=0` — two complex roots
- `z=t*(1+i)` — continuous diagonal line (real and imaginary parts equal)
- `z=cos(t)+i*sin(t)` — unit circle for t in radian range
- `re(z)=im(z)` — diagonal complex locus
- `abs(z)=1` — unit circle, function light off
- `z=sin(i)` — computed complex point

**Real:**

- `y=x`, `y=x^2`, `f(x)=sin(x)` — explicit real functions
- `x=2` — vertical line, function light off
- `x^2+y^2=4` — implicit real circle
- `x=cos(t);y=sin(t)` — real parametric unit circle
- `x=y` — implicit diagonal line

**Supported numeric operations:** `+`, `-`, `*`, `/`, `^`, parentheses; `sin`,
`cos`, `tan`, `asin`, `acos`, `atan`, `sinh`, `cosh`, `tanh`, `abs`, `log`,
`ln`, `exp`, `sqrt`, `re`, `im`, `conj`, `arg`, `floor`, `ceil`; constants `pi`,
`e`, `i`. `log` and `ln` both mean **natural logarithm**. Polynomial root
solver still handles degree <= 10; general complex equations are not a
full symbolic CAS. Trig functions in complex mode use principal values.

## Project files

- `.tnsproject.json`: TNS Tool Portable project settings / four C sources
- `ndgraph_ndless.c`: interface, physical Catalog key, graph rendering
- `ndgraph_expr.c/.h`: expression parser, graph classification, angle setting
- `ndgraph_features.c/.h`: history, real-curve samples, numeric intersections
- `complex_math.c/.h`: polynomial root solver
- `Makefile`: optional official Ndless SDK build
- `test_ndgraph.c`, `test_complex.c`, `test_features.c`, `test_angles.c`:
  host-side math tests; `make test` runs these with a desktop C compiler.

## Current test status

- Original expression/mode tests: **45 passed**
- Added features/history/intersection tests: **13 passed**
- Angle-mode tests: **7 passed**
- Original polynomial examples: passed (see test log)
- Graphical UI: passed a **host C99 syntax check** with substitute headers
  and a simulated SDL input/render smoke test under AddressSanitizer/
  UndefinedBehaviorSanitizer. A targeted v0.31 UI regression test confirmed
  H toggles Help from all four screens, H remains typable in the editor,
  Catalog navigation is preserved, and redrawing graphs does not erase the log.
  These tests do **not** establish SDK compatibility.
- **Unverified:** actual Windows ARM toolchain build, hardware input,
  appearance/performance on a real TI-Nspire CX II CAS.

When updating from v0.3, keep your older working `.tns` until the new
v0.31 build has been verified on your calculator.
