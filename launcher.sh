#!/bin/bash

cd $(dirname $0)

./debug.main ./assets/$(xprop | grep "WM_CLASS(STRING)" | cut -d',' -f2 | xargs).ini

