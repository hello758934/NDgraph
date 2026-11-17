NDgraph v0.31
Real and complex-plane graphing for the TI-Nspire CX II CAS.  
Free and open source · MIT licensed · Made by Carlos (2026)
> **Experimental release:** This repository contains **source code**, not a ready-to-run `NDgraph.tns`. You must compile it before using it on a calculator. The native ARM build and physical calculator behavior have **not yet been verified**.
What can NDgraph do?
Real mode	Imaginary (complex-plane) mode
`y=x`	`z^4=1`
`y=sin(x)`	`z=t*(1+i)`
`x=2`	`re(z)=im(z)`
`x^2+y^2=4`	`abs(z)=1`
Both modes offer six colored graph slots, trace, auto-fit, a function-status light, basic math functions, and more. Press the calculator's Catalog (open-book) key to switch between the graph and the full-screen equation list. Press H for the full-screen help menu.
Install on Windows
1. What you'll need
A TI-Nspire CX II CAS with compatible Ndless already installed.
A Windows PC and USB data cable.
TNS Tool Portable — a free Windows program that builds a calculator app from NDgraph's source code. Download the Windows x64 Portable version, not “Ndless Web Compiler.”
TI-Nspire CX II Connect — to copy the finished app to the calculator.
How it works: `NDgraph source ZIP` → TNS Tool Portable → `NDgraph.tns` → TI-Nspire calculator.
2. Download the source
On this GitHub page, select Code → Download ZIP. Save the ZIP to your PC, not your calculator.
If TNS Tool cannot import GitHub's ZIP directly, extract it, select all files inside the extracted `NDgraph-main` folder, and create a new ZIP. The `.tnsproject.json` file must be at the top level of that ZIP.
3. Compile NDgraph
Download and extract TNS Tool Portable for Windows x64 from the releases page.
Run `TNS Tool Portable.exe` on your PC.
Open Syntax Doctor in its sidebar.
Click the green `+ File` control inside Syntax Doctor → Open Ndless Project.
Select the NDgraph source ZIP and choose Build TNS.
If the build succeeds, save the generated `NDgraph.tns`.
Important: The tiny File menu at the top of the application may contain only Exit. That is not the project importer.
4. Transfer it to the calculator
Back up your calculator files and check that your version of Ndless works with your calculator OS.
Connect the calculator to your PC using USB.
Open TI-Nspire CX II Connect → File Transfer → To Calculator.
Transfer the compiled `NDgraph.tns`, then open it on the calculator.
Try `y=x` in Real mode or `z^4=1` in Imaginary mode. Press Esc to exit. A successful compile does not guarantee that the app is stable on real hardware; do not update the calculator OS just to run this experiment.
Quick controls
Key	Does what?
Catalog (book)	Graph ↔ full-screen Functions list
Tab	Switch Real / Imaginary
Enter	Edit or confirm a graph
H	Show / hide full-screen Help
T	Trace a graph
F	Auto-fit graph
R	Equation history
S	Settings
Esc	Back / Exit
The bottom strip shows the event log, with just `H Help` as its key hint. The Help screen ends with `made by carlos 2026`. For every hotkey, math limits, and more examples, see USAGE.md.
Common problems
Problem	Fix
Downloaded “Ndless Web Compiler”	Use TNS Tool Portable for Windows x64 instead.
File menu only has Exit	Go to Syntax Doctor → green + File.
ZIP is rejected; picker lists `.tns`, `.py`, `.lua`...	You're using the normal file picker. Choose Open Ndless Project.
Build failed	Don't transfer anything. Save the error log and open an issue.
Only have a ZIP, not a `.tns`	The ZIP is source code. It must be compiled, not renamed.
About this project
License: MIT. Third-party Ndless/nSDL terms are listed in THIRD_PARTY_NOTICES.md.
Status: Experimental; math tests passed on desktop, but the native ARM build and on-calculator operation are unverified. Some graphing and function-detection results are numerical approximations.
Contribute: Bug reports and pull requests are welcome. Read CONTRIBUTING.md.
Credits: Made by Carlos (2026), with AI-assisted development. Not affiliated with Texas Instruments.
