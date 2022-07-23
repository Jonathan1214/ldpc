set terminal svg size 500 400
set output 'outsvg.svg'

# set logscale
set logscale y
set format y "%0.1e"

# set coordinate range
set xrange[0:8]
set yrange[1e-8:1]

set grid

# plot
# when draw mulit line in a axes, using ''after first line
# using -> u
# lp    -> linepoints
plot "outdata.dat" u 1:2 title "6x20 BLER" with lp,''u 1:3 title "6x20 BER" with lp,\
"out.dat" u 1:2 title "6x16 BLER" with lp,''u 1:3 title "6x16 BER" with lp
