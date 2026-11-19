
# NDgraph v0.31
**Real and complex-plane graphing for the TI-Nspire CX II CAS.**  
Free and open source · MIT licensed · Made by Carlos (2026)

> **Experimental release:** Compiled `.tns` downloads are now available for v0.1, v0.2, v0.31, and v0.41. The repository also contains source code if you want to build it yourself. Calculator compatibility and stability may vary.

## What can NDgraph do?

| Real mode | Imaginary (complex-plane) mode |
| --- | --- |
| `y=x` | `z^4=1` |
| `y=sin(x)` | `z=t*(1+i)` |
| `x=2` | `re(z)=im(z)` |
| `x^2+y^2=4` | `abs(z)=1` |

Both modes offer **six colored graph slots**, trace, auto-fit, a function-status light, basic math functions, and more.

Press the calculator's **Catalog (open-book)** key to switch between the graph and the full-screen equation list. Press **H** for the full-screen help menu.

## Install on Windows

### Easy installation (compiled releases available)

1. Go to the [NDgraph Releases page](https://github.com/hello758934/NDgraph/releases).
2. Download a compiled ZIP: [v0.1 (Complex)](https://github.com/user-attachments/files/35046338/Complex.zip), [v0.2](https://github.com/user-attachments/files/35046339/NDgraphV0.2.zip), [v0.31](https://github.com/user-attachments/files/35046341/NDgraphV0.31.zip), or [v0.41](https://github.com/user-attachments/files/35046496/NDgraphV0.41.zip).
3. Extract the ZIP on your computer and locate the included `.tns` file.
4. Connect your TI-Nspire CX II CAS using USB.
5. Open [TI-Nspire CX II Connect](https://nspireconnect.ti.com/).
6. Transfer the extracted `.tns` file to your calculator.
7. Open NDgraph with a compatible version of Ndless installed.

**No compiler or TNS Tool required!**

**Note:** v0.1, v0.2, and v0.31 now have compiled `.tns` ZIP downloads too! You don't have to build those versions yourself.


## Method 2: Build from Source (Optional)

> **Note:** This method is completely optional for all four compiled versions.
>
> If you just want to use NDgraph on your calculator, follow **Method 1: Easy Installation** instead.
>
> This method is intended for developers, contributors, or anyone who wants to compile or modify NDgraph themselves.


### 1. What you'll need

- A **TI-Nspire CX II CAS** with compatible [Ndless](https://github.com/ndless-nspire/Ndless) already installed.
- A **Windows PC** and USB data cable.
- [TNS Tool Portable](https://github.com/acewalt/tns-tool-wasm/releases) — a free Windows program that builds a calculator app from NDgraph's source code.
- [TI-Nspire CX II Connect](https://nspireconnect.ti.com/) — to transfer the finished app.

**Important:** Download **TNS Tool Portable for Windows x64**, NOT “Ndless Web Compiler.”

**How it works:**

`NDgraph source ZIP` → `TNS Tool Portable` → `NDgraph.tns` → `TI-Nspire calculator`

### 2. Download the source

1. On this GitHub page, click **Code → Download ZIP**.
2. Save the ZIP to your computer.
3. Do not transfer the ZIP directly to your calculator.

If TNS Tool cannot import GitHub's ZIP, extract it and create a new ZIP containing the files inside the `NDgraph-main` folder.

The `.tnsproject.json` file must be at the top level of the ZIP.

### 3. Compile NDgraph

1. Download and extract [TNS Tool Portable for Windows x64](https://github.com/acewalt/tns-tool-wasm/releases).
2. Run `TNS Tool Portable.exe`.
3. Open **Syntax Doctor** in the sidebar.
4. Click the green **+ File** button inside Syntax Doctor.
5. Select **Open Ndless Project**.
6. Choose the NDgraph source ZIP.
7. Click **Build TNS**.
8. If compilation succeeds, save the resulting `NDgraph.tns`.

**Note:** The File menu at the very top of the application may contain only **Exit**. That's not the project importer.

### 4. Transfer NDgraph to your calculator

1. Back up your calculator files.
2. Make sure Ndless is compatible with your calculator's OS.
3. Connect your calculator to your PC using USB.
4. Open [TI-Nspire CX II Connect](https://nspireconnect.ti.com/).
5. Select **File Transfer → To Calculator**.
6. Transfer the compiled `NDgraph.tns`.
7. Open the file on your calculator.

Try these equations:

**Real mode:** `y=x`

**Imaginary mode:** `z^4=1`

Press **Esc** to exit.

And you're done!

## Quick controls

| Key | Action |
| --- | --- |
| **Catalog (book)** | Switch between Graph and Functions |
| **Tab** | Switch Real / Imaginary |
| **Enter** | Edit or confirm a graph |
| **H** | Open / close Help |
| **T** | Trace a graph |
| **F** | Auto-fit |
| **R** | Equation history |
| **S** | Settings |
| **Esc** | Back / Exit |

The bottom strip displays the event log, with only `H Help` shown as a shortcut.

See [USAGE.md](USAGE.md) for more controls, examples, and limitations.

## Troubleshooting

| Problem | Solution |
| --- | --- |
| Downloaded Ndless Web Compiler | Download TNS Tool Portable instead. |
| File menu only shows Exit | Use Syntax Doctor → green + File. |
| ZIP file is rejected | Choose Open Ndless Project instead of the regular file picker. |
| Compilation fails | Save the error log and [report the issue](https://github.com/hello758934/NDgraph/issues). |
| Only have a ZIP | If it's a compiled release ZIP, extract the `.tns` inside. Only source ZIPs need compiling. |

## About this project

**License:** [MIT License](LICENSE)

**Status:** Experimental. Compiled `.tns` releases are available; desktop mathematical tests have passed, but compatibility and behavior on every calculator setup have not been independently verified. Some graphing and function-detection results are numerical approximations.

**Third-party software:** See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for Ndless and nSDL licensing information.

**Contributing:** Bug reports and pull requests are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md).

**Credits:** Made by Carlos (2026), with AI-assisted development.

Not affiliated with Texas Instruments.
