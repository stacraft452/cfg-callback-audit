# Data files

## empirical_results.csv

Seven APIs empirically tested for CFG enforcement (Windows 10 Build 22631).

Columns: `api`, `callback_type`, `origin_dll`, `cfg_outcome`, `exit_code`, `reverse_tcp`, `windows_build`, `test_date`

## callback_audit_high_priority.csv

Thirty-three high-priority callback APIs with predicted CFG category (A/B/C) and `empirically_tested` flag.

Derived from manuscript Appendix B.

## callback_audit_full_catalog.csv

Structured output of MinGW-W64 user-mode header scan (2026-06-09).

Columns: `Header`, `Line`, `Category`, `Identifier`, `Signature`

Categories include `TYPEDEF_CALLBACK`, `TYPEDEF_WINAPI`, `TYPEDEF_NTAPI`, and selected `API_DECL` entries.

Total: **157 rows**.

## mingw_header_scan_raw.txt

Human-readable raw scan log: target header list, per-file line matches, regex patterns, and summary notes.

Scan environment documented in file header (WinLibs MinGW-W64, 62 target headers).
