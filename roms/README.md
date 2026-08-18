# ROM Preservation Policy

This folder is for ROM images that are worth preserving in git.

## What Belongs Here

Commit a ROM image here only when it is a real milestone, for example:

- the first known-good ASSIST09 boot image
- a verified ASSIST09 + BASIC combined image
- an image actually programmed into EPROM with the GTEK 7228
- an image tied to a documented hardware or software milestone

## What Does Not Belong Here

Do not commit routine build outputs from `src/m6809` here.

Normal development artifacts should stay ignored:

- `*.bin`
- `*.lst`
- `*.s19`
- `*.sym`
- `*.crf`

Those files are disposable unless they represent a milestone image.

## What To Commit With A Milestone ROM

For each preserved ROM, include:

- the ROM image itself
- a checksum file such as `.sha256`
- a short Markdown note describing what it is
- the build date
- the source inputs used to create it
- whether it was tested in RAM before burn
- whether it was programmed into EPROM with the GTEK 7228

## Which Build Output Is For Burning

For the GTEK 7228 burn step, the file that belongs here is the final ROM image binary produced by the build, for example:

- `src/m6809/assist09.bin`

That `.bin` file is the EPROM payload to preserve, checksum, and program.

The other build outputs have different roles:

- `*.s19`
  - useful for serial or RAM loading through ASSIST09
- `*.lst`
  - listing file for inspection and debug
- `*.sym`
  - symbol output from the build
- `*.crf`
  - cross-reference output from the build

Only the final ROM image `.bin` should normally be copied into `roms/` for the burn path.

## Suggested Naming

Use names that carry both purpose and date, for example:

- `assist09-2026-08-18.bin`
- `assist09-basic-combined-2026-08-18.bin`
- `assist09-2026-08-18.bin.sha256`
- `assist09-2026-08-18.md`

## Recommended Workflow

1. Build in `src/m6809`.
2. Test over serial in RAM, typically using the generated `.s19` file.
3. Burn with the GTEK 7228 only after the image is trusted.
4. Copy the final preserved `.bin` ROM image into `roms/`.
5. Add the checksum and short milestone note.

This keeps the main source tree clean while still preserving important ROM history.
