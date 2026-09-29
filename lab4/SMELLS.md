# Test smells — the short list

## Mystery guest

The test depends on something it does not show: a file, database, or hidden fixture.
Fix: build the input in the test or commit a small visible fixture.

## Resource optimism

The test assumes an environment or resource it did not create.
Fix: supply the resource or run the test in a separate suite that declares it.

## General fixture

Every test shares setup that most tests do not use.
Fix: give each test only the data it needs.

## Eager test

One test exercises several behaviours, so a failure is hard to diagnose.
Fix: share arrangement if useful, but split the acts.

## Assertion roulette

Several assertions have no diagnostic context, especially inside loops.
Fix: name the case or compare a complete value that produces a useful diff.

## A test that asserts nothing

A check such as `result != nullptr`, `count >= 0`, or an assertion guarded by the
condition it is meant to verify passes regardless of meaningful behaviour.
