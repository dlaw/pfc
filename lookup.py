#!/usr/bin/python

from numpy import *
vset = 40
vdd = arange(256) * 50 / 256
cboost = 1002e-6
t = 1/120
e_cycle = 1/2 * cboost * (vset**2 - vdd**2)
vinb = 16  # vin rms
i_over_v = e_cycle / (t * vinb**2)
ard_i_over_v = i_over_v * 36/5
print((ard_i_over_v * 64).astype(int))
