NDgraph v0.31
A free, open-source real and complex-number grapher for the TI-Nspire CX II CAS.  
Made by Carlos — 2026 · MIT License
> **Current release: experimental source code, not a ready-to-install calculator app.** There is no prebuilt `NDgraph.tns` in this repository yet. If you want to try it on a calculator, you must first turn the source code into a `.tns` app with the free Windows program described below. The build and hardware behavior have **not yet been verified** for v0.31.
What does NDgraph do?
NDgraph has two graphing modes:
Real: graph `y=x`, `y=sin(x)`, `x=2`, `x^2+y^2=4`, and other supported real equations.
Imaginary (complex plane): graph roots such as `z^4=1`, curves such as `z=t*(1+i)`, and loci such as `re(z)=im(z)` or `abs(z)=1`.
It includes six individually colored graph slots per mode, tracing, auto-fit, numerical intersections for certain real functions, history for the current session, basic math functions, and settings. The physical Catalog / open-book key swaps between the graph and a full-screen function editor. H opens the full-screen key guide. A small light indicates whether the selected graph is recognized as a function (the test is approximate for some implicit equations). The input field starts blank, so enter the whole equation, including `x=`, `y=`, or `z=`.
How do I get NDgraph onto my calculator?
Read this first: Because the compiled calculator app is not available yet, these instructions are for Windows users who want to build and test an experimental version. If a future GitHub Release provides a verified `NDgraph.tns`, you will be able to skip the compiler and transfer that file directly.
What you need (all free)
Item	What it is / why you need it
TI-Nspire CX II CAS	The calculator NDgraph targets. Other models have not been verified.
Ndless	A separate homebrew runtime/loader that lets the calculator launch native apps. It must already be installed and compatible with your calculator's OS. Official Ndless project.
TNS Tool Portable for Windows	A free desktop app that compiles the NDgraph C source into a real `.tns` calculator program. It includes the ARM compiler and the Ndless SDK. This is not NDgraph itself, and it does not have to be installed on the calculator. Download TNS Tool Portable.
NDgraph source ZIP	The files from this repository that TNS Tool will compile.
USB data cable + TI-Nspire CX II Connect	Used to transfer the resulting `.tns` app to the calculator. TI-Nspire CX II Connect.
Step 1 — Download the NDgraph source
Click the green Code button at the top of this GitHub repository, then Download ZIP.
Save the ZIP on your Windows computer. Do not send the ZIP to the calculator.
If TNS Tool cannot read the GitHub ZIP, extract it and create a new ZIP of the files inside the `NDgraph-main` folder, so `.tnsproject.json` is at the ZIP's top level.
Step 2 — Download the correct TNS Tool
Open TNS Tool's official GitHub Releases page.
Download the TNS Tool Portable for Windows x64 ZIP (not the separate Ndless Web Compiler or a browser-only version).
Extract the downloaded TNS Tool ZIP into a folder. Don't run it inside the compressed ZIP.
Double-click `TNS Tool Portable.exe` to open the desktop application.
What is TNS Tool? Think of it as the builder. NDgraph is the source code, TNS Tool turns it into an app, and Ndless lets the calculator run that app. You do not need Ubuntu or WSL.
Step 3 — Build NDgraph.tns
Inside TNS Tool Portable, open Syntax Doctor from its sidebar.
Find the green `+ File` button inside Syntax Doctor (not the small Windows menu bar's `File`, which may only show Exit).
Choose Open Ndless Project. This is the project importer, not the regular Open File dialog.
Select the NDgraph source ZIP from Step 1.
In the Ndless project workspace, choose Build TNS.
If the build succeeds, save the generated `NDgraph.tns` file. This `.tns` is the actual calculator program.
If the dialog says `Supported formats: .tns, .xml, .py, .lua, .pdf...`, you clicked the wrong Open File action. Close it and use Syntax Doctor → green + File → Open Ndless Project. If you can't find these controls, check the version of TNS Tool Portable and its official documentation; the program's layout can change.
Step 4 — Transfer and test on the calculator
Back up important calculator files and make sure Ndless is active for your OS version.
Connect the calculator to your computer with a USB data cable.
Open TI-Nspire CX II Connect, then File Transfer → To Calculator.
Select the compiled `NDgraph.tns` from Step 3, not the source ZIP or a `.c` file.
Open it on the calculator. Try `y=x` in Real mode or `z^4=1` in Imaginary mode, and press Esc to leave the app.
A successful computer build does not guarantee that the app works on real hardware. Treat the first launch as a test and keep a backup. Do not update the calculator OS merely to run NDgraph—that may break Ndless compatibility.
If something goes wrong
Problem	What to do
“What is TNS Tool?”	It's the free Windows compiler app, linked in Step 2. It's not part of TI's official calculator software.
I downloaded “Ndless Web Compiler”	That's not the Portable app needed for the steps here. Download TNS Tool Portable for Windows x64 from the releases page.
The top-left File menu only has Exit	Use Syntax Doctor → green + File, inside the program, instead.
ZIP file is rejected; only `.tns`, `.xml`, `.py`, etc. accepted	You opened the regular file picker. Select Open Ndless Project instead.
Build TNS reports a compiler error	Don't transfer anything. Save the complete error log and open an issue.
I only have a ZIP, no `NDgraph.tns`	The ZIP is source code, not a runnable `.tns`. Build it first; never just rename the ZIP.
App crashes or freezes	Stop testing, restart the calculator using TI's official recovery instructions if needed, and report your OS/Ndless version and symptoms.
For a more detailed reference, see USAGE.md. Developers can run desktop tests with `make test` if GCC/Make are installed, but these tests do not verify the ARM build.
Calculator controls
Button	Action
Catalog / open-book	Switch between graph and full-screen Functions menu
Tab	Switch Real / Imaginary modes
H	Open/close full-screen Help (not during text editing)
Enter	Edit or confirm a function
V / C / Delete	Toggle selected graph, change its color, delete it
T / F / I	Trace, auto-fit, numerical intersections for eligible real functions
R / S	Equation history / settings
Esc	Cancel, close a menu, or exit
In Help, the footer reads `made by carlos 2026`. The bottom strip of other screens shows the event log plus the `H Help` hint, not a long hotkey list.
License, development and limitations
License: MIT, see LICENSE. NDgraph is free to use, study, improve, and share under that license. External Ndless/nSDL components have their own licenses; see THIRD_PARTY_NOTICES.md.
Current state: Experimental C source; no verified binary included, and no verified CX II CAS hardware test for v0.31. Do not describe it as confirmed to run on every OS version.
Math limits: Numerical roots for supported polynomials up to degree 10, approximate implicit plots and function detection, and real explicit-function intersections with limitations. This is not a full symbolic CAS. History and settings are currently session-only.
Contributing: Issues and pull requests welcome; see CONTRIBUTING.md. Some code was created with AI assistance; copyright protection depends on qualifying human authorship and applicable law.
This is an independent project, not affiliated with Texas Instruments.
