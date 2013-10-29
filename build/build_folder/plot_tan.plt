ftan(x) = a1 * x**4 + b1 * x**3 + c1 * x**2 + d1 * x + e1

a1              = -1.15545e-19
b1              = 9.65035e-15
c1              = -2.30784e-11
d1              = 4.18391e-06
e1              = -1.07535


#fit ftan(x) 'data.dat' using 1:4 via a1, b1, c1, d1, e1

plot 'data.dat' using 1:4 title 'tan', 'data2.dat' using 1:4 title 'tan_NN'
