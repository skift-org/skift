# Contributing

First thank you for considering contributing to the operating system :hugs: ❤️.

## Table of Contents

- [Contributing](#contributing)
  - [Table of Contents](#table-of-contents)
  - [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Before Submitting A Bug Report](#before-submitting-a-bug-report)
  - [How Do I Submit A (GOOD) Bug Report?](#how-do-i-submit-a-good-bug-report)
  - [Your First Code Contribution](#your-first-code-contribution)
  - [Getting Your Pull Request Merged](#getting-your-pull-request-merged)
- [Acknowledgements](#acknowledgements)

## Code of Conduct

This project and everyone participating in it is governed by the [skiftOS Code of Conduct](code_of_conduct.md). By participating, you are expected to uphold this code.
Please report unacceptable behavior to [conduct@skiftos.org](mailto:conduct@skiftos.org).

# How Can I Contribute?

## Reporting Bugs

This section guides you through submitting a bug report for skiftOS. Following these guidelines helps maintainers and the community understand your report, reproduce the behavior, and find related reports.

Before creating bug reports, please check [this list](#before-submitting-a-bug-report) as you might find out that you don't need to create one. When you are creating a bug report, please [include as many details as possible](#how-do-i-submit-a-good-bug-report).

> **Note:** If you find a **Closed** issue that seems like it is the same thing that you're experiencing, open a new issue and include a link to the original issue in the body of your new one.

## Before Submitting A Bug Report

* **Perform a [cursory search](https://github.com/search?q=is%3Aissue+user%3AskiftOS)** to see if the problem has already been reported. If it has **and the issue is still open**, add a comment to the existing issue instead of opening a new one.

## How Do I Submit A (GOOD) Bug Report?

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/).

Explain the problem and include additional details to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as many details as possible.
* **Describe the behavior you observed after following the steps** and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **Include screenshots** which clearly demonstrate the problem if needed.
* **Include a stack trace/panic report**
* **If the problem wasn't triggered by a specific action**, describe what you were doing before the problem happened and share more information using the guidelines below.

Provide more context by answering these questions:

* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.

Include details about your configuration and environment:

* **Which version of skift are you using?**
* **Are you running skift in a virtual machine?** If so, which VM software are you using ?
* **Are you running skift on real hardware?** If so, provide information about it.


## Your First Code Contribution

Unsure where to begin contributing to skift? You can start by looking through these beginner and help-wanted issues:

Beginner issues - issues which should only require a few lines of code, and a test or two.
Help wanted issues - issues which should be a bit more involved than beginner issues.
Both issue lists are sorted by total number of comments. While not perfect, number of comments is a reasonable proxy for impact a given change will have.

## Getting Your Pull Request Merged

In order to get your PR through the review process as fast as possible, here are some do's and don'ts to try to stick to:

- Conform to the project [coding style](coding_style.md). Please use `clang-format` with the `.clang-format` file included with the project.
- When committing, the first line of your commit message should be "category: Brief description of what's being changed.". The "category" can be a subdirectory, but also something like "POSIX compliance" or "module-name". Whatever seems logical.
- Don't touch anything outside the stated scope of the PR.
- Don't iterate excessively on your design across multiple commits.
- "Cleanup" PR are accepted but only if they improve the code objectively by reducing complexity or help the code follow more closely the KISS principles.
- Avoid using function and types from the libc and the std, prefer native skift libraries like libsystem and libutils.

# Acknowledgements

This text is based on the Atom's CONTRIBUTING.md and Fukurō's CONTRIBUTING.md
