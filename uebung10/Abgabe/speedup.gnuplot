set title 'Weak scaling Jacobi vs Gauss-Seidel'
set yrange [0:150]
set xrange [1:100]
set logscale x
set pointsize 1
set grid
set xlabel 'Prozesse'
set ylabel 'Berechnungszeit [s]'
plot 'WEAK_SCALING_JA.dat' using 1:4 title "Jacobi" with points, \
'WEAK_SCALING_GS.dat' using 1:4 title "Gauss-Seidel" with points
set term png
set output "weak-scaling.png"
replot



set title 'Strong scaling Jacobi vs Gauss-Seidel'
set yrange [0:600]
set xrange [1:100]
set pointsize 1
set grid
set xlabel 'Prozesse'
set ylabel 'Berechnungszeit [s]'
plot 'STRONG_SCALING_JA.dat' using 1:4 title "Jacobi" with points, \
'STRONG_SCALING_GS.dat' using 1:4 title "Gauss-Seidel" with points
set term png
set output "strong-scaling.png"
replot


set title 'Communication Jacobi vs Gauss-Seidel'
set yrange [0:100]
set xrange [1:10]
set pointsize 1
set grid
set xlabel 'Knoten'
set ylabel 'Berechnungszeit [s]'
plot 'COMMUNICATION_A_JA.dat' using 2:4 title "Jacobi" with points, \
'COMMUNICATION_A_GS.dat' using 2:4 title "Gauss-Seidel" with points
set term png
set output "communication.png"
replot
