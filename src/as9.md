# AS9 Assembler

This repository keeps a salvaged, Linux-portable copy of Motorola's AS9 assembler in [assembler/](assembler/). It is used to build the ASSIST09 monitor and fig-Forth sources without converting their Motorola syntax.

## Build

From the repository root:

```bash
cd src/assembler
make as9
```

The resulting `as9` executable is used by [forth-09/Makefile](forth-09/Makefile) to build ASSIST09 and Forth artifacts.

## Compatibility Notes

- Do not put whitespace inside an operand expression: `bar - foo` is parsed as `bar` followed by a comment.
- The warning option is enabled by default. Use `now` for legacy sources, such as ASSIST09, whose `*` comments otherwise produce warnings.
- `as9n.c` is an alternative entry point for systems without `strncasecmp()`.

## References

- Motorola assembler reference: [as11v2.pdf](../docs/as11v2.pdf)
- Canonical AS9 porting history and caveats: [as9_changes.txt](assembler/as9_changes.txt)
