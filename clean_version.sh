#!/bin/sh
sed -e "s/\$Rev.*\\$/\$Rev\\$/g;s/\$Date.*\\$/\$Date\\$/g;s/\$Author.*\\$/\$Author\\$/" $1
