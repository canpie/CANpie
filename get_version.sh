#!/bin/sh
author=$(git log -1 --format=%an)
version=$(git describe --tags --dirty --always)
datum=$(git log -1 --format=%cd)
sed -e "s/\$Rev\\$/\$Rev: $version\\$/g;s/\$Date\\$/\$Date: $datum\\$/g;s/\$Author\\$/\$Author: $author\\$/" $1
