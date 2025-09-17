# Security Policy

## Supported Versions

Security fixes are provided for the actively maintained code lines:

- The `master` branch.
- The most recent tagged release (currently `v3.10`).

Older releases remain available for reference but do not receive security updates. Consider upgrading before reporting
new issues so we can reproduce on a supported version.

## Reporting a Vulnerability

We strongly prefer coordinated, private disclosure. Please **do not** open a public issue describing a potential
vulnerability.

1. Use the GitHub "Report a vulnerability" workflow for this repository. It creates a private security advisory visible
   only to the maintainer team.
2. If the advisory channel is unavailable, contact us via the MicroControl security desk at
   `support@microcontrol.net` and mention "CANpie security" in the subject/message.

When you reach out, include as much detail as possible:

- The CANpie version/commit you tested (output of `git describe --tags --dirty`).
- Your platform, compiler, and CAN interface setup.
- Step-by-step reproduction instructions, including any required configuration or sample data.
- The impact you believe the issue has (e.g., remote code execution, denial of service, data leak).
- Suggested mitigations or patches if you have them.

If your report includes sensitive attachments, let us know so we can arrange a secure transfer method.

## What to Expect

- We normally acknowledge new reports within 3 business days.
- We will work with you to reproduce and triage the issue. Expect follow-up questions while we investigate.
- Once validated, we will agree on a disclosure timeline. Our default goal is to publish a fix and a coordinated
  disclosure notice within 30 days, but complex issues may require more time.
- You will be credited in the release notes and advisory unless you request otherwise.

## Coordinated Disclosure

Please keep vulnerability details confidential until a fix is released. We will communicate milestone dates and provide
pre-release patches to reporters under responsible-disclosure terms when feasible. After publishing a security advisory
we encourage affected users to upgrade promptly.

## Dependencies & Downstream Projects

If you maintain downstream projects or packages that embed CANpie, please contact us after a fix is available so we can
coordinate release timing. We appreciate being notified about security issues that originate in upstream dependencies as
well.

Thank you for helping keep CANpie users safe!
