#!/bin/bash

cd $(dirname $0)

./debug.main $(xprop | grep "WM_CLASS(STRING)" | sed "s/,/\\n/g" | tail -n 1 | xargs)

