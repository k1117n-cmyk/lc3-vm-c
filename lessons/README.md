# Lesson Snapshots

This directory stores fixed snapshots for each article in the LC-3 VM series.

The repository root is the working version that keeps moving forward. Each `lessons/NN-topic/` directory is a stable copy that matches a published article.

## Workflow

For each new article:

1. Implement and verify the feature in the repository root.
2. Copy the finished source, Makefile, examples, and expected output into the matching lesson directory.
3. Commit the root changes, article materials, and lesson snapshot together.
4. Tag the finished point, for example `article-07-profile`.

## Directories

```text
03-trap-terminal-games/
06-trace/
07-profile/
08-step/
09-breakpoint/
10-dump/
11-debug-example/
12-release/
```

Each lesson should include only files needed to reproduce that article.
