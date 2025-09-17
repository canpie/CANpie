# GitHub Security Advisory Draft — CANpie

This template captures the details GitHub requests when you publish a security advisory for CANpie. Replace the
placeholders before submitting via the repository’s **Security » Advisories » New draft security advisory** workflow.

## Summary

- **Title:** `<Short, action-oriented summary>`
- **Identifier:** `<GitHub advisory ID or CVE number if assigned>`
- **Severity:** `<CVSS v3.1 base score (e.g., 7.5 High)>`
- **CWE(s):** `<Relevant CWE ID(s)>`

## Affected Components

- **Impacted versions:** `<e.g., CANpie <= v3.09>`
- **Introduced in:** `<commit hash or version where the bug first appeared>`
- **Fixed in:** `<commit hash / branch / tag that contains the patch>`

## Impact

Describe what an attacker can achieve and any prerequisites (local access, valid CAN frame, authenticated user, etc.).
Note data integrity, confidentiality, availability implications, and whether exploitation requires specific hardware.

## Proof of Concept / Reproduction

Provide deterministic reproduction steps or PoC code. Include build configuration, platform, compiler, and CAN interface
setup (if applicable). Mention log output or crash signatures the maintainer team should expect when validating.

## Mitigations

- `<Primary fix: e.g., apply patch abc1234 or upgrade to v3.10>`
- `<Secondary mitigations: e.g., disable feature X, add sanity checks in application code>`
- `<Monitoring guidance: logs, counters, CAN bus diagnostics>`

## Workarounds

If a full upgrade is not immediately possible, document any temporary mitigations, configuration toggles, or deployment
patterns that reduce exposure. Explicitly state when no workaround exists.

## Acknowledgements

Credit the reporter(s) and any contributors who helped validate or patch the issue. Mention if credit should be
anonymous.

## Disclosure Timeline

Use UTC dates.

- `<2025-01-05>`: Report received via `<GitHub advisory portal | support@microcontrol.net>`
- `<2025-01-07>`: Maintainers confirmed reproduction
- `<2025-01-20>`: Patch released in `<branch/tag>`
- `<2025-01-22>`: Advisory published

## References

Link to commits, pull requests, release notes, CVE entries, or external write-ups.

- `<https://github.com/canpie/CANpie/commit/abc1234>`
- `<https://github.com/canpie/CANpie/releases/tag/v3.10>`
- `<Additional documentation>`

---

### How to Publish on GitHub

1. Navigate to **Security → Advisories** in the CANpie repository.
2. Click **New draft security advisory**.
3. Paste the completed sections above into the form fields (Summary, Severity, Affected versions, etc.).
4. Attach patches or link pull requests under the **Patches** section.
5. Save the draft and request a CVE ID from GitHub if needed.
6. Coordinate embargo, release planning, and public disclosure with the reporter per `.github/SECURITY.md`.
7. Publish the advisory once the fix is available and linked releases are ready.
