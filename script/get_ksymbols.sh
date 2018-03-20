#!/bin/sh

grep " _text" /proc/kallsyms
grep "prepare_kernel_cred" /proc/kallsyms
grep "commit_creds" /proc/kallsyms
