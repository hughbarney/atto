#!/bin/sh
exec >&2

make atto
equivs-build atto.equivs
