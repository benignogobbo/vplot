# -*- mode: sh -*- ################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
###################################################################

include( qwt.prf )

TARGET       = vplot

HEADERS =        \
    VPlot.h      \
    VDisplay.h   \
    Dvm.h        \
    VNS.h

SOURCES =         \
    VPlot.cc      \
    VDisplay.cc   \
    Dvm.cc        \
    VNS.cc        \
    main.cc
