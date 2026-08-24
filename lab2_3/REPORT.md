# Lab 2_3 — Group A__

> Copy this file to `lab2_3/REPORT.md` in your fork and fill it in. Keep the eight
> headings exactly as they are — they are the marking scheme, in order. Delete this
> quote block and every `` when you are done.

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Antigravity IDE Agent |
| `ubiquitous-language` install route | Pasted SKILL.md (Fallback method) |
| `refactoring/` pack install route | Pasted SKILL.md (Fallback method) |

No skills folder support in Antigravity Agent, so we used the pasted SKILL.md fallback method.

---

## 2. What I changed in the glossary — [4]

The generated file is at `lab2_3/UBIQUITOUS_LANGUAGE.md`. Three or four lines here on what
you corrected and why: terms it invented, definitions it got wrong, ambiguities it missed.



---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (the code as you received it) and `lab2_3/audits/lab1-head.md`
(after your Lab-1 PR).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** |  |  |
| Smells my Lab-1 PR **left untouched** |  |  |
| Smells my Lab-1 PR **removed** |  |  |

The third row will probably be zero. Leave it in.

---

## 4. Rejected candidates — [6]

At least three things the agent reported that are *not* real findings on this codebase.

| smell reported | `file:line` | why it does not hold |
|---|---|---|
| Hardcoded UI Strings | `code/game.cpp:379` | Extracting basic terminal layout strings to a config file is overengineering for a simple console app. |
| Primitive Obsession | `code/game.cpp:410` | The scores are just simple counters for a single session; wrapping them in a class adds unnecessary complexity. |
| Data Clump | `code/game.cpp:525` | Passing basic coordinates directly to `board->place()` is perfectly fine here and combining them into a payload object isn't strictly necessary. |
|  |  |  |
|  |  |  |

---

## 5. Commit map — [7]

Run `lab2_3/check-lab2_3.sh` and paste the table it prints.

| # | sha | subject | what it is |
|---|---|---|---|
| 1 |  |  | glossary |
| 2 |  |  | smell report |
| 3 |  |  | **the refactor, alone** |
| 4 |  |  | **the feature, alone** |

---

## 6. Two-run measurement — [4]

Run 1 is your Lab-1 branch — the numbers you already reported. Run 2 is commit 4 alone.

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced |  |  |
| Lines changed, `git diff --shortstat -w` |  |  |
| Lines changed, **raw** (no `-w`) |  |  |
| Functions reached |  |  |
| Prompts to working code |  |  |
| Wall-clock time |  |  |

Commit 3 (the refactor) on its own:  lines `-w`,  raw.

`check-lab2_3.sh` prints the four line-count numbers for run 2. Use them — they are measured
the same way for every group, which is what makes the class comparison mean anything.

---

## 7. Analysis Q1–Q2 — [5]

**Q1. Which smell did commit 3 actually fix?** Name it from your section 3 report. What was
expensive before, what does it cost now.



**Q2. Compare commit 4 to your Lab-1 diff.** Same feature, same codebase. What changed in
the cost and what did not? If it got worse, say so and explain — that marks the same.



---

## 8. Analysis Q3–Q4 — [5]

**Q3. Go back through your Lab-1 `LLM-LOG.md`. Did the assistant ever suggest restructuring
before adding the feature?** Quote it if it did. If it did not, what would have had to be
different in your prompt?



**Q4. How do you know commit 3 did not change behaviour?** Answer honestly. Most of you will
find that you do not know. Say that plainly if it is true, and describe what you would have
needed in order to actually know.



---

Sections 7 and 8 together: **500 words maximum.**

## If you did not finish

Say so here and lose no marks. What you got to, where it broke, what you tried.


